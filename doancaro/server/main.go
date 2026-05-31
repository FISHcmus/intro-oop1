package main

import (
	"bufio"
	"encoding/json"
	"flag"
	"fmt"
	"log"
	"math/rand"
	"net"
	"net/http"
	"strconv"
	"strings"
	"sync"
	"time"
)

const (
	boardSize = 15
	cellEmpty = 0
	cellX     = 1
	cellO     = 2
)

type server struct {
	mu    sync.Mutex
	rooms map[string]*room
	rnd   *rand.Rand
}

type room struct {
	code      string
	server    *server
	players   [2]*client
	board     [boardSize][boardSize]byte
	turn      byte
	moveCount int
	finished  bool
	closed    bool
	mu        sync.Mutex
}

type client struct {
	conn      net.Conn
	server    *server
	send      chan string
	name      string
	room      *room
	mark      byte
	closeOnce sync.Once
}

func main() {
	addr := flag.String("addr", ":34567", "listen address")
	healthAddr := flag.String("health-addr", "", "optional HTTP health listen address")
	flag.Parse()

	srv := &server{
		rooms: make(map[string]*room),
		rnd:   rand.New(rand.NewSource(time.Now().UnixNano())),
	}
	startedAt := time.Now()

	if *healthAddr != "" {
		go serveHealth(*healthAddr, srv, startedAt)
	}

	ln, err := net.Listen("tcp", *addr)
	if err != nil {
		log.Fatalf("listen %s: %v", *addr, err)
	}
	defer ln.Close()

	log.Printf("caro room server listening on %s", *addr)
	for {
		conn, err := ln.Accept()
		if err != nil {
			log.Printf("accept: %v", err)
			continue
		}
		c := &client{
			conn:   conn,
			server: srv,
			send:   make(chan string, 32),
		}
		go c.writeLoop()
		go c.readLoop()
	}
}

func (c *client) readLoop() {
	defer c.close("client disconnected")

	scanner := bufio.NewScanner(c.conn)
	buf := make([]byte, 0, 4096)
	scanner.Buffer(buf, 1024*1024)
	for scanner.Scan() {
		c.handleLine(scanner.Text())
	}
}

func (c *client) writeLoop() {
	w := bufio.NewWriter(c.conn)
	for msg := range c.send {
		if _, err := w.WriteString(msg + "\n"); err != nil {
			return
		}
		if err := w.Flush(); err != nil {
			return
		}
	}
}

func (c *client) handleLine(line string) {
	fields := strings.Split(line, "\t")
	if len(fields) == 0 {
		return
	}

	switch fields[0] {
	case "CREATE":
		if len(fields) < 2 {
			c.sendLine("ERROR\tCREATE requires a player name")
			return
		}
		c.name = sanitizeName(fields[1])
		c.server.createRoom(c)
	case "JOIN":
		if len(fields) < 3 {
			c.sendLine("ERROR\tJOIN requires room code and player name")
			return
		}
		c.name = sanitizeName(fields[2])
		c.server.joinRoom(strings.ToUpper(fields[1]), c)
	case "MOVE":
		if len(fields) < 3 {
			c.sendLine("ERROR\tMOVE requires row and col")
			return
		}
		row, err1 := strconv.Atoi(fields[1])
		col, err2 := strconv.Atoi(fields[2])
		if err1 != nil || err2 != nil {
			c.sendLine("ERROR\tMOVE row/col must be integers")
			return
		}
		if c.room == nil {
			c.sendLine("ERROR\tNot in a room")
			return
		}
		c.room.handleMove(c, row, col)
	case "QUIT":
		c.close("client quit")
	default:
		c.sendLine("ERROR\tUnknown command")
	}
}

func (c *client) sendLine(msg string) {
	select {
	case c.send <- msg:
	default:
		log.Printf("dropping message to %s: %s", c.name, msg)
	}
}

func (c *client) close(reason string) {
	c.closeOnce.Do(func() {
		if c.room != nil {
			c.room.handleDisconnect(c, reason)
		}
		close(c.send)
		_ = c.conn.Close()
	})
}

func (s *server) createRoom(c *client) {
	s.mu.Lock()
	defer s.mu.Unlock()

	code := s.newRoomCodeLocked()
	r := &room{
		code:   code,
		server: s,
		turn:   cellX,
	}
	r.players[0] = c
	c.room = r
	c.mark = cellX
	s.rooms[code] = r

	c.sendLine("ROOM\t" + code)
	c.sendLine("WAIT\tRoom created. Waiting for opponent...")
}

func (s *server) joinRoom(code string, c *client) {
	s.mu.Lock()
	r := s.rooms[code]
	s.mu.Unlock()
	if r == nil {
		c.sendLine("ERROR\tRoom not found")
		return
	}

	r.mu.Lock()
	defer r.mu.Unlock()

	if r.closed {
		c.sendLine("ERROR\tRoom closed")
		return
	}
	if r.players[1] != nil {
		c.sendLine("ERROR\tRoom already full")
		return
	}
	if r.finished || r.players[0] == nil {
		c.sendLine("ERROR\tRoom already finished")
		return
	}

	r.players[1] = c
	c.room = r
	c.mark = cellO

	host := r.players[0]
	hostName := host.name
	guestName := c.name

	host.sendLine(fmt.Sprintf("START\tX\t%s\t%s", hostName, guestName))
	c.sendLine(fmt.Sprintf("START\tO\t%s\t%s", hostName, guestName))
}

func (s *server) removeRoom(code string) {
	s.mu.Lock()
	defer s.mu.Unlock()
	delete(s.rooms, code)
}

func (s *server) roomCount() int {
	s.mu.Lock()
	defer s.mu.Unlock()
	return len(s.rooms)
}

func (s *server) newRoomCodeLocked() string {
	const alphabet = "ABCDEFGHJKLMNPQRSTUVWXYZ23456789"
	for {
		var b strings.Builder
		for i := 0; i < 6; i++ {
			b.WriteByte(alphabet[s.rnd.Intn(len(alphabet))])
		}
		code := b.String()
		if _, exists := s.rooms[code]; !exists {
			return code
		}
	}
}

func (r *room) handleMove(c *client, row, col int) {
	r.mu.Lock()
	defer r.mu.Unlock()

	if r.finished {
		c.sendLine("ERROR\tMatch already finished")
		return
	}
	if row < 0 || row >= boardSize || col < 0 || col >= boardSize {
		c.sendLine("ERROR\tMove out of bounds")
		return
	}
	if c.mark != r.turn {
		c.sendLine("ERROR\tNot your turn")
		return
	}
	if r.board[row][col] != cellEmpty {
		c.sendLine("ERROR\tCell already occupied")
		return
	}

	r.board[row][col] = c.mark
	r.moveCount++
	r.broadcast(fmt.Sprintf("APPLY\t%s\t%d\t%d", markLabel(c.mark), row, col))

	if r.hasWinner(row, col, c.mark) || r.moveCount == boardSize*boardSize {
		r.finished = true
		r.turn = cellEmpty
		if r.hasWinner(row, col, c.mark) {
			r.broadcast("INFO\tMatch finished. Winner: " + markLabel(c.mark))
		} else {
			r.broadcast("INFO\tMatch finished. Draw.")
		}
		return
	}

	r.turn = otherMark(c.mark)
}

func (r *room) handleDisconnect(c *client, reason string) {
	r.mu.Lock()
	if r.closed {
		r.mu.Unlock()
		return
	}

	r.closed = true
	r.finished = true
	peer := r.other(c)
	if r.players[0] == c {
		r.players[0] = nil
	}
	if r.players[1] == c {
		r.players[1] = nil
	}
	c.room = nil
	c.mark = cellEmpty

	if peer != nil {
		peer.room = nil
		peer.mark = cellEmpty
	}

	srv := r.server
	code := r.code
	r.mu.Unlock()

	if peer != nil {
		peer.sendLine("QUIT\tPeer disconnected")
		_ = peer.conn.Close()
	}

	if srv != nil {
		srv.removeRoom(code)
	}
	_ = reason
}

func (r *room) broadcast(msg string) {
	for _, p := range r.players {
		if p != nil {
			p.sendLine(msg)
		}
	}
}

func (r *room) other(c *client) *client {
	if r.players[0] == c {
		return r.players[1]
	}
	if r.players[1] == c {
		return r.players[0]
	}
	return nil
}

func (r *room) hasWinner(row, col int, mark byte) bool {
	dirs := [][2]int{{1, 0}, {0, 1}, {1, 1}, {1, -1}}
	for _, d := range dirs {
		count := 1
		count += r.countDir(row, col, d[0], d[1], mark)
		count += r.countDir(row, col, -d[0], -d[1], mark)
		if count >= 5 {
			return true
		}
	}
	return false
}

func (r *room) countDir(row, col, dr, dc int, mark byte) int {
	count := 0
	for {
		row += dr
		col += dc
		if row < 0 || row >= boardSize || col < 0 || col >= boardSize {
			return count
		}
		if r.board[row][col] != mark {
			return count
		}
		count++
	}
}

func sanitizeName(name string) string {
	name = strings.TrimSpace(name)
	if name == "" {
		return "Player"
	}
	if len(name) > 24 {
		return name[:24]
	}
	return name
}

func markLabel(mark byte) string {
	if mark == cellX {
		return "X"
	}
	return "O"
}

func otherMark(mark byte) byte {
	if mark == cellX {
		return cellO
	}
	return cellX
}

func serveHealth(addr string, srv *server, startedAt time.Time) {
	mux := http.NewServeMux()
	mux.HandleFunc("/healthz", func(w http.ResponseWriter, r *http.Request) {
		if r.Method != http.MethodGet {
			w.WriteHeader(http.StatusMethodNotAllowed)
			return
		}
		w.Header().Set("Content-Type", "text/plain; charset=utf-8")
		_, _ = w.Write([]byte("ok\n"))
	})
	mux.HandleFunc("/status", func(w http.ResponseWriter, r *http.Request) {
		if r.Method != http.MethodGet {
			w.WriteHeader(http.StatusMethodNotAllowed)
			return
		}
		w.Header().Set("Content-Type", "application/json")
		payload := map[string]any{
			"activeRooms":   srv.roomCount(),
			"startedAt":     startedAt.UTC().Format(time.RFC3339),
			"uptimeSeconds": int(time.Since(startedAt).Seconds()),
		}
		_ = json.NewEncoder(w).Encode(payload)
	})

	server := &http.Server{
		Addr:              addr,
		Handler:           mux,
		ReadHeaderTimeout: 5 * time.Second,
	}
	log.Printf("caro health server listening on %s", addr)
	if err := server.ListenAndServe(); err != nil && err != http.ErrServerClosed {
		log.Printf("health server: %v", err)
	}
}

<script setup lang="ts">
import { ref } from 'vue'
import { onSlideEnter } from '@slidev/client'

const mountKey = ref(0)
onSlideEnter(() => { mountKey.value++ })
</script>

<template>
  <div class="s-tt" :key="mountKey">
    <div class="chrome">
      <div class="brand"
           v-motion
           :initial="{ opacity: 0, scale: 1.15, rotate: -2 }"
           :enter="{ opacity: 1, scale: 1, rotate: 0, transition: { duration: 200, delay: 100 } }">
        <span class="dot"></span>CARO
      </div>
      <div class="num"
           v-motion
           :initial="{ opacity: 0, scale: 1.15, rotate: 2 }"
           :enter="{ opacity: 1, scale: 1, rotate: 0, transition: { duration: 200, delay: 180 } }">
        <b>07</b> &middot; Minimax &middot; Zobrist + TT
      </div>
    </div>

    <div class="eyebrow"
         v-motion
         :initial="{ opacity: 0, x: -6 }"
         :enter="{ opacity: 1, x: 0, transition: { duration: 220, delay: 250 } }">
      Cùng một thế cờ, từ những con đường khác nhau
    </div>

    <h2 v-motion
        :initial="{ opacity: 0, y: 18 }"
        :enter="{ opacity: 1, y: 0, transition: { duration: 260, delay: 320 } }">
      <em>Hash một lần</em>. Cache lại kết quả tìm kiếm.
    </h2>

    <div class="body-grid">
      <div class="zobrist"
           v-motion
           :initial="{ opacity: 0, y: 14 }"
           :enter="{ opacity: 1, y: 0, transition: { duration: 280, delay: 480 } }">
        <div class="pc-label">Zobrist hash</div>
        <div class="zobrist-diagram">
          <div class="zb-line">
            <code>zobristTable[15][15][2]</code>
            <span class="arrow">←</span>
            <span class="note">uint64 ngẫu nhiên cho từng (ô, quân), khởi tạo một lần</span>
          </div>
          <div class="zb-line">
            <code>hash = ⊕ zobristTable[r][c][mark]</code>
            <span class="note">một phép XOR cho mỗi ô đã đặt</span>
          </div>
          <div class="zb-line highlight">
            <code>place(r,c)</code> → <code>hash ^= zobristTable[r][c][mark]</code>
          </div>
          <div class="zb-line highlight">
            <code>undo(r,c)</code>  → <code>hash ^= zobristTable[r][c][mark]</code>
            <span class="note">cùng phép XOR, vì XOR là phép nghịch đảo của chính nó</span>
          </div>
        </div>
        <div class="pc-label label-small">Board.cpp:6&ndash;55 &middot; cập nhật tăng tiến O(1)</div>
      </div>

      <div class="tt"
           v-motion
           :initial="{ opacity: 0, x: 14 }"
           :enter="{ opacity: 1, x: 0, transition: { duration: 280, delay: 680 } }">
        <div class="pc-label">Mục transposition table</div>
        <pre class="tt-code"><code><span class="kw">struct</span> <span class="ty">TTEntry</span> {
    <span class="ty">int</span>     depth;     <span class="cm">// depth của cây con đã tìm</span>
    <span class="ty">int</span>     score;     <span class="cm">// kết quả minimax</span>
    <span class="ty">TTFlag</span>  flag;      <span class="cm">// Exact / LowerBound / UpperBound</span>
    <span class="ty">Move</span>    bestMove;  <span class="cm">// dùng để sắp nước đi khi gặp lại</span>
};

<span class="ty">unordered_map</span>&lt;<span class="ty">uint64_t</span>, <span class="ty">TTEntry</span>&gt; transTable;</code></pre>

        <ul class="tt-notes">
          <li>
            <span class="pill teal">hit</span>
            <div>Nếu <code>entry.depth &ge; depth</code> và flag cho phép, <b>trả về ngay</b>.</div>
          </li>
          <li>
            <span class="pill coral">order</span>
            <div>Khi hit một phần, <b>đẩy</b> <code>bestMove</code> lên đầu danh sách ứng viên — α&ndash;β cắt sớm hơn.</div>
          </li>
          <li>
            <span class="pill gold">flags</span>
            <div>Các flag cận cho phép tái dùng kết quả <b>một phần</b> từ cắt α&ndash;β, không chỉ điểm chính xác.</div>
          </li>
        </ul>
      </div>
    </div>
  </div>
</template>

<style scoped>
.s-tt {
  position: absolute;
  inset: 0;
  background: var(--paper);
  padding: 90px 60px 70px;
  overflow: hidden;
  display: flex;
  flex-direction: column;
}

.eyebrow {
  font-family: var(--mono);
  font-size: 11px;
  color: var(--muted);
  letter-spacing: 0.1em;
  text-transform: uppercase;
  margin-bottom: 14px;
  display: flex; align-items: center; gap: 7px;
  will-change: transform, opacity;
}
.eyebrow::before {
  content: ""; width: 24px; height: 1px; background: var(--ink); opacity: 0.5;
}

h2 {
  font-size: 34px;
  letter-spacing: -0.025em;
  margin-bottom: 26px;
  will-change: transform, opacity;
}
h2 em { color: var(--teal); font-style: normal; }

.body-grid {
  display: grid;
  grid-template-columns: 1fr 1fr;
  gap: 32px;
  flex: 1;
}

.pc-label {
  font-family: var(--mono);
  font-size: 10px;
  text-transform: uppercase;
  letter-spacing: 0.12em;
  color: var(--muted);
  margin-bottom: 8px;
}
.label-small { margin-top: 8px; font-size: 9px; opacity: 0.8; }

.zobrist, .tt {
  will-change: transform, opacity;
}

.zobrist-diagram {
  background: var(--paper-2);
  border: 1px solid var(--line);
  border-radius: 6px;
  padding: 14px 16px;
  display: flex;
  flex-direction: column;
  gap: 8px;
}
.zb-line {
  font-family: var(--mono);
  font-size: 11px;
  line-height: 1.45;
  color: var(--ink);
  display: flex;
  flex-wrap: wrap;
  gap: 6px;
  align-items: center;
}
.zb-line code {
  font-family: var(--mono);
  background: var(--paper);
  padding: 2px 6px;
  border-radius: 3px;
  border: 1px solid var(--line);
  font-weight: 600;
}
.zb-line .note {
  color: var(--muted);
  font-family: var(--sans);
  font-size: 11px;
}
.zb-line .arrow { color: var(--coral); font-weight: 700; }
.zb-line.highlight code {
  background: var(--teal-tint);
  border-color: var(--teal);
}

.tt-code {
  background: var(--ink);
  color: var(--paper);
  padding: 14px 18px;
  border-radius: 6px;
  font-family: var(--mono);
  font-size: 12px;
  line-height: 1.55;
  margin: 0 0 14px;
  white-space: pre-wrap;
  word-break: break-word;
}
.tt-code .cm { color: rgba(250,250,247,0.45); }
.tt-code .kw { color: var(--gold); }
.tt-code .ty { color: var(--coral); }

.tt-notes {
  list-style: none;
  padding: 0;
  margin: 0;
  display: flex;
  flex-direction: column;
  gap: 8px;
}
.tt-notes li {
  font-size: 12px;
  line-height: 1.45;
  color: var(--ink);
  display: flex;
  gap: 8px;
  align-items: flex-start;
}
.tt-notes b { font-weight: 700; }
.tt-notes code {
  font-family: var(--mono);
  font-size: 11px;
  background: var(--paper-2);
  padding: 1px 5px;
  border-radius: 3px;
}

.pill {
  flex: 0 0 auto;
  display: grid;
  place-items: center;
  padding: 2px 8px;
  border-radius: 10px;
  font-family: var(--mono);
  font-size: 9px;
  font-weight: 700;
  text-transform: uppercase;
  letter-spacing: 0.08em;
  color: var(--paper);
  background: var(--muted);
  margin-top: 2px;
}
.pill.teal { background: var(--teal); }
.pill.coral { background: var(--coral); }
.pill.gold { background: var(--gold); color: var(--ink); }

@media (prefers-reduced-motion: reduce) {
  .chrome .brand, .chrome .num, .eyebrow, h2, .zobrist, .tt {
    transform: none !important;
    opacity: 1 !important;
  }
}
</style>

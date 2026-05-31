<script setup lang="ts">
import { ref } from 'vue'
import { onSlideEnter } from '@slidev/client'

const mountKey = ref(0)
onSlideEnter(() => { mountKey.value++ })
</script>

<template>
  <div class="s-mp" :key="mountKey">
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
        <b>10</b> &middot; Đối kháng qua mạng
      </div>
    </div>

    <div class="eyebrow"
         v-motion
         :initial="{ opacity: 0, x: -6 }"
         :enter="{ opacity: 1, x: 0, transition: { duration: 220, delay: 250 } }">
      Socket TCP &middot; luồng nền &middot; hàng đợi NetEvent
    </div>

    <h2 v-motion
        :initial="{ opacity: 0, y: 18 }"
        :enter="{ opacity: 1, y: 0, transition: { duration: 260, delay: 320 } }">
      Đối thủ ở xa = <em>một nguồn nước đi khác</em>.
    </h2>

    <div class="body-grid">
      <div class="format"
           v-motion
           :initial="{ opacity: 0, y: 14 }"
           :enter="{ opacity: 1, y: 0, transition: { duration: 280, delay: 480 } }">
        <div class="pc-label">Bàn giao network &rarr; game &mdash; cùng mẫu với AI thread</div>
<pre class="layout"><code><span class="cm">// NetworkSession: socket TCP + luồng nền recv()</span>
<span class="ty">struct</span> NetEvent { Type type; Move move; };
<span class="ty">std::queue</span>&lt;NetEvent&gt; events;   <span class="cm">// luồng nền push, main rút</span>

<span class="ty">void</span> Game::updateMultiplayer() {
    NetEvent ev;
    <span class="kw">while</span> (net.poll(ev))            <span class="cm">// rút ở 60 FPS, không chặn UI</span>
        <span class="kw">if</span> (ev.type == Move)
            <span class="hi">applyMove</span>(ev.move);     <span class="cm">// CÙNG pipeline với người &amp; AI</span>
}</code></pre>
        <div class="topo-row">
          <div class="topo host">
            <div class="tp-label">LAN P2P</div>
            <div class="tp-tag">host-authoritative</div>
          </div>
          <div class="topo">
            <div class="tp-label">Online</div>
            <div class="tp-tag">relay &middot; CREATE / JOIN</div>
          </div>
        </div>
      </div>

      <div class="notes-col">
        <div class="note-card"
             v-motion
             :initial="{ opacity: 0, x: 10 }"
             :enter="{ opacity: 1, x: 0, transition: { duration: 240, delay: 700 } }">
          <div class="kicker">Giao thức dòng văn bản</div>
          <ul class="enum">
            <li><code>HELLO</code> / <code>START</code> &mdash; bắt tay, chốt X / O</li>
            <li><code>MOVE</code> / <code>APPLY</code> &mdash; gửi nước, host xác nhận</li>
            <li><code>CREATE</code> / <code>JOIN</code> / <code>ROOM</code> / <code>WAIT</code> &mdash; sảnh relay</li>
            <li><code>INFO</code> / <code>ERROR</code> / <code>QUIT</code> &mdash; trạng thái, lỗi, rời ván</li>
          </ul>
        </div>

        <div class="note-card"
             v-motion
             :initial="{ opacity: 0, x: 10 }"
             :enter="{ opacity: 1, x: 0, transition: { duration: 240, delay: 860 } }">
          <div class="kicker">Đa nền tảng, không treo UI</div>
          <p class="tiny">
            Luồng nền <code>recv()</code> đẩy <code>NetEvent</code> vào hàng đợi có khóa;
            main loop rút mỗi frame. Socket POSIX (Linux/macOS) và Winsock2
            (<code>ws2_32</code>, Windows) sau cùng một interface.
          </p>
          <p class="tiny">
            Cổng mặc định <code>34567</code>; địa chỉ relay đọc từ biến môi trường
            (<code>ServerConfig</code>). Lõi <code>Board</code> / <code>AIPlayer</code>
            <b>không sửa một dòng</b> &mdash; đúng tinh thần Open-Closed.
          </p>
        </div>
      </div>
    </div>
  </div>
</template>

<style scoped>
.s-mp {
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
  font-size: 32px;
  letter-spacing: -0.025em;
  margin-bottom: 24px;
  will-change: transform, opacity;
}
h2 em { color: var(--coral); font-style: normal; }

.body-grid {
  display: grid;
  grid-template-columns: 1.2fr 0.8fr;
  gap: 28px;
  flex: 1;
}

.pc-label {
  font-family: var(--mono);
  font-size: 10px;
  text-transform: uppercase;
  letter-spacing: 0.12em;
  color: var(--muted);
  margin-bottom: 6px;
}

.format {
  will-change: transform, opacity;
  display: flex;
  flex-direction: column;
}

pre.layout {
  background: var(--ink);
  color: var(--paper);
  padding: 14px 18px;
  border-radius: 6px;
  font-family: var(--mono);
  font-size: 11.5px;
  line-height: 1.55;
  margin: 0 0 14px;
  white-space: pre-wrap;
  word-break: break-word;
}
pre.layout .cm { color: rgba(250,250,247,0.42); }
pre.layout .ty { color: var(--coral); }
pre.layout .kw { color: var(--gold); }
pre.layout .hi { color: var(--teal-tint); font-weight: 700; }

.topo-row {
  display: grid;
  grid-template-columns: 1fr 1fr;
  gap: 8px;
}
.topo {
  border: 1px solid var(--line);
  border-radius: 4px;
  padding: 8px 10px;
  background: var(--paper-2);
  text-align: center;
}
.topo.host {
  border-color: var(--teal);
  background: var(--teal-tint);
}
.tp-label {
  font-family: var(--mono);
  font-size: 11px;
  font-weight: 700;
  color: var(--ink);
}
.tp-tag {
  font-family: var(--mono);
  font-size: 9px;
  letter-spacing: 0.08em;
  text-transform: uppercase;
  color: var(--muted);
  margin-top: 2px;
}

.notes-col {
  display: flex;
  flex-direction: column;
  gap: 14px;
}
.note-card {
  border: 1px solid var(--line);
  border-radius: 6px;
  padding: 12px 14px;
  background: var(--paper-2);
  will-change: transform, opacity;
}
.kicker {
  font-family: var(--mono);
  font-size: 10px;
  text-transform: uppercase;
  letter-spacing: 0.12em;
  color: var(--coral);
  margin-bottom: 8px;
}

ul.enum {
  list-style: none;
  padding: 0;
  margin: 0;
  display: flex;
  flex-direction: column;
  gap: 4px;
}
ul.enum li {
  font-size: 12px;
  line-height: 1.4;
  color: var(--ink);
}
ul.enum code {
  font-family: var(--mono);
  font-size: 11px;
  background: var(--paper);
  padding: 1px 5px;
  border-radius: 3px;
  border: 1px solid var(--line);
  font-weight: 700;
  color: var(--teal);
}

.tiny {
  font-size: 11px;
  color: var(--muted);
  margin: 0 0 8px;
  line-height: 1.45;
}
.tiny:last-child { margin-bottom: 0; }
.tiny code {
  font-family: var(--mono);
  font-size: 10.5px;
  background: var(--paper);
  padding: 1px 4px;
  border-radius: 3px;
  border: 1px solid var(--line);
  color: var(--ink);
}
.tiny b { color: var(--ink); font-weight: 700; }

@media (prefers-reduced-motion: reduce) {
  .chrome .brand, .chrome .num, .eyebrow, h2, .format, .note-card {
    transform: none !important;
    opacity: 1 !important;
  }
}
</style>

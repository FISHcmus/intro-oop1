<script setup lang="ts">
import { ref } from 'vue'
import { onSlideEnter } from '@slidev/client'

const mountKey = ref(0)
onSlideEnter(() => { mountKey.value++ })
</script>

<template>
  <div class="s-tree" :key="mountKey">
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
        <b>05</b> &middot; Minimax &middot; cây trò chơi
      </div>
    </div>

    <div class="eyebrow"
         v-motion
         :initial="{ opacity: 0, x: -6 }"
         :enter="{ opacity: 1, x: 0, transition: { duration: 220, delay: 250 } }">
      Mức 2+ &middot; Nhìn xa nhiều nước
    </div>

    <h2 v-motion
        :initial="{ opacity: 0, y: 18 }"
        :enter="{ opacity: 1, y: 0, transition: { duration: 260, delay: 320 } }">
      Hai bên đi <em>luân phiên</em>. Lạc quan gặp <em>bi quan</em>.
    </h2>

    <div class="body-grid">
      <div class="tree"
           v-motion
           :initial="{ opacity: 0, y: 10 }"
           :enter="{ opacity: 1, y: 0, transition: { duration: 280, delay: 480 } }">
<pre><code>              <span class="max">MAX  (AI)</span>                      depth 0
              ╱     ╲     ╲
           <span class="min">MIN</span>    <span class="min">MIN</span>    <span class="min">MIN</span>                depth 1
          ╱  ╲   ╱ ╲    ╱ ╲
        <span class="max">MAX</span> <span class="max">MAX</span> <span class="max">MAX</span> <span class="max">MAX</span> <span class="max">MAX</span> <span class="max">MAX</span>              depth 2
         │   │   │   │   │   │
        <span class="leaf">eval eval eval eval eval eval</span>     heuristic chấm điểm</code></pre>
      </div>

      <ul class="notes">
        <li v-motion
            :initial="{ opacity: 0, x: 10 }"
            :enter="{ opacity: 1, x: 0, transition: { duration: 240, delay: 700 } }">
          <span class="tag max-tag">MAX</span>
          <div>Lượt AI — chọn con có điểm <em>cao nhất</em>.</div>
        </li>
        <li v-motion
            :initial="{ opacity: 0, x: 10 }"
            :enter="{ opacity: 1, x: 0, transition: { duration: 240, delay: 820 } }">
          <span class="tag min-tag">MIN</span>
          <div>Lượt đối thủ — giả định họ chọn điểm <em>thấp nhất</em> cho ta.</div>
        </li>
        <li v-motion
            :initial="{ opacity: 0, x: 10 }"
            :enter="{ opacity: 1, x: 0, transition: { duration: 240, delay: 940 } }">
          <span class="tag leaf-tag">LEAF</span>
          <div><code>evaluate()</code> cộng các tra cứu <code>patternScore</code> 243 mục trên mọi cửa sổ 5 ô của bàn cờ.</div>
        </li>
        <li v-motion
            :initial="{ opacity: 0, x: 10 }"
            :enter="{ opacity: 1, x: 0, transition: { duration: 240, delay: 1060 } }">
          <span class="tag cost-tag">COST</span>
          <div><code>O(b<sup>d</sup>)</code> — tăng <em>theo hàm mũ</em> với depth. Trên bàn 15&times;15, depth-3 ngây thơ sẽ bùng nổ nếu không cắt tỉa.</div>
        </li>
      </ul>
    </div>
  </div>
</template>

<style scoped>
.s-tree {
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
  margin-bottom: 34px;
  line-height: 1.2;
  will-change: transform, opacity;
}
h2 em:nth-of-type(1) { color: var(--teal); font-style: normal; }
h2 em:nth-of-type(2) { color: var(--coral); font-style: normal; }

.body-grid {
  display: grid;
  grid-template-columns: 1.1fr 0.9fr;
  gap: 48px;
  flex: 1;
  align-items: center;
}

.tree {
  background: var(--paper-2);
  border: 1px solid var(--line);
  border-radius: 6px;
  padding: 24px 26px;
  will-change: transform, opacity;
}
.tree pre {
  margin: 0;
  font-family: var(--mono);
  font-size: 14px;
  line-height: 1.55;
  color: var(--ink);
  font-weight: 600;
}
.tree .max { color: var(--teal); }
.tree .min { color: var(--coral); }
.tree .leaf { color: var(--muted); }

.notes {
  list-style: none;
  padding: 0;
  margin: 0;
  display: flex;
  flex-direction: column;
  gap: 14px;
}
.notes li {
  font-size: 14px;
  line-height: 1.45;
  color: var(--ink);
  display: flex;
  gap: 10px;
  align-items: flex-start;
  will-change: transform, opacity;
}
.notes em { color: var(--teal); font-style: normal; font-weight: 700; }
.notes code {
  font-family: var(--mono);
  font-size: 12px;
  background: var(--paper-2);
  padding: 1px 5px;
  border-radius: 3px;
}

.tag {
  flex: 0 0 auto;
  font-family: var(--mono);
  font-size: 10px;
  font-weight: 700;
  text-transform: uppercase;
  letter-spacing: 0.08em;
  padding: 2px 7px;
  border-radius: 3px;
  margin-top: 2px;
  color: var(--paper);
}
.max-tag { background: var(--teal); }
.min-tag { background: var(--coral); }
.leaf-tag { background: var(--muted); }
.cost-tag { background: var(--gold); color: var(--ink); }

@media (prefers-reduced-motion: reduce) {
  .chrome .brand, .chrome .num, .eyebrow, h2, .tree, .notes li {
    transform: none !important;
    opacity: 1 !important;
  }
}
</style>

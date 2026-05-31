<script setup lang="ts">
import { ref } from 'vue'
import { onSlideEnter } from '@slidev/client'
import { slideBackgrounds } from './slideBackgrounds'

const mountKey = ref(0)
const bg = slideBackgrounds.minimaxTT
onSlideEnter(() => { mountKey.value++ })
</script>

<template>
  <div class="s-tt scene-slide" :key="mountKey">
    <div class="scene-bg warm">
      <img :src="bg" alt="Transposition table background" />
    </div>

    <div class="scene-shell">
      <div class="chrome dark">
        <div class="brand"
             v-motion
             :initial="{ opacity: 0, scale: 1.12, rotate: -2 }"
             :enter="{ opacity: 1, scale: 1, rotate: 0, transition: { duration: 200, delay: 100 } }">
          <span class="dot"></span>CARO
        </div>
        <div class="num"
             v-motion
             :initial="{ opacity: 0, scale: 1.12, rotate: 2 }"
             :enter="{ opacity: 1, scale: 1, rotate: 0, transition: { duration: 200, delay: 180 } }">
          <b>09</b> &middot; TT &middot; Zobrist hash
        </div>
      </div>

      <div class="glass-panel tt-panel"
           v-motion
           :initial="{ opacity: 0, y: 24 }"
           :enter="{ opacity: 1, y: 0, transition: { duration: 320, delay: 320 } }">
        <div class="eyebrow on-image">Cùng trạng thái &middot; không tính lại từ đầu</div>
        <h2 class="scene-title">Transposition Table giúp AI <em>nhớ lại</em>.</h2>

        <div class="tt-grid">
          <div class="paper-panel flow-card">
            <div class="card-head">Hash tăng tiến</div>
            <div class="flow-line"><code>place(r,c)</code> &rarr; <code>hash ^= table[r][c][mark]</code></div>
            <div class="flow-line"><code>undo(r,c)</code> &rarr; <code>hash ^= table[r][c][mark]</code></div>
            <p>Undo quay lại trạng thái cũ rất rẻ.</p>
          </div>

          <div class="paper-panel flow-card">
            <div class="card-head">Transposition Table</div>
            <div class="flow-line"><code>hash</code> &rarr; <code>{ depth, score, flag, bestMove }</code></div>
            <p>Gặp lại trạng thái cũ là tái dùng kết quả ngay.</p>
          </div>
        </div>

        <div class="scene-chip-row">
          <div class="scene-chip">giảm lặp tính toán</div>
          <div class="scene-chip">Normal: depth 2</div>
          <div class="scene-chip">Hard: depth 3</div>
        </div>
      </div>
    </div>
  </div>
</template>

<style scoped>
.tt-panel {
  margin-top: 34px;
  width: min(820px, 78%);
  padding: 24px 26px 26px;
}

.tt-panel h2 {
  margin-bottom: 14px;
}

.tt-grid {
  display: grid;
  grid-template-columns: 1fr 1fr;
  gap: 12px;
  margin-bottom: 14px;
}

.flow-card {
  padding: 14px 16px;
}

.card-head {
  font-family: var(--mono);
  font-size: 10px;
  text-transform: uppercase;
  letter-spacing: 0.12em;
  color: var(--coral);
  margin-bottom: 10px;
}

.flow-line {
  font-family: var(--mono);
  font-size: 13px;
  line-height: 1.4;
  color: var(--ink);
  margin-bottom: 8px;
}

.flow-card p {
  margin: 0;
  font-size: 13px;
  line-height: 1.4;
  color: var(--ink-2);
}

@media (prefers-reduced-motion: reduce) {
  .chrome .brand, .chrome .num, .tt-panel {
    transform: none !important;
    opacity: 1 !important;
  }
}
</style>

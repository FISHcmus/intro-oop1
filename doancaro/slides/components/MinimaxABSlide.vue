<script setup lang="ts">
import { ref } from 'vue'
import { onSlideEnter } from '@slidev/client'
import { slideBackgrounds } from './slideBackgrounds'

const mountKey = ref(0)
const bg = slideBackgrounds.minimaxAB
onSlideEnter(() => { mountKey.value++ })
</script>

<template>
  <div class="s-ab scene-slide" :key="mountKey">
    <div class="scene-bg cool">
      <img :src="bg" alt="Alpha beta background" />
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
          <b>08</b> &middot; Minimax &middot; alpha-beta
        </div>
      </div>

      <div class="glass-panel ab-panel"
           v-motion
           :initial="{ opacity: 0, x: 24 }"
           :enter="{ opacity: 1, x: 0, transition: { duration: 320, delay: 320 } }">
        <div class="eyebrow on-image">Cùng kết quả &middot; ít nhánh hơn</div>
        <h2 class="scene-title">Alpha-beta cắt bớt <em>nhánh thừa</em>.</h2>

        <div class="ab-grid">
          <div class="code-box">
            <div class="scene-kicker">Ý chính</div>
            <pre><code>for child in orderedMoves:
  update alpha or beta
  if alpha >= beta:
    break</code></pre>
          </div>

          <div class="ab-notes">
            <div class="paper-panel note-card">
              <div class="note-head">alpha</div>
              <p>Mốc tốt nhất hiện tại của MAX.</p>
            </div>
            <div class="paper-panel note-card">
              <div class="note-head">beta</div>
              <p>Mốc tốt nhất hiện tại của MIN.</p>
            </div>
          </div>
        </div>

        <div class="scene-chip-row">
          <div class="scene-chip">kết quả không đổi</div>
          <div class="scene-chip">cây nhỏ hơn</div>
          <div class="scene-chip">duyệt nhanh hơn</div>
        </div>
      </div>
    </div>
  </div>
</template>

<style scoped>
.ab-panel {
  margin-top: auto;
  margin-left: auto;
  width: min(860px, 82%);
  padding: 30px 32px 32px;
}

.ab-panel h2 {
  margin-bottom: 18px;
}

.ab-grid {
  display: grid;
  grid-template-columns: 1fr 0.94fr;
  gap: 18px;
  margin-bottom: 18px;
}

.code-box pre {
  margin: 10px 0 0;
  padding: 18px 20px;
  border-radius: 22px;
  background: rgba(255, 251, 242, 0.58);
  border: 1px solid rgba(11, 22, 24, 0.10);
  color: var(--ink);
  font-size: 14px;
  line-height: 1.6;
}

.ab-notes {
  display: grid;
  gap: 12px;
}

.note-card {
  padding: 16px 18px;
}

.note-head {
  font-family: var(--mono);
  font-size: 10px;
  text-transform: uppercase;
  letter-spacing: 0.12em;
  color: var(--coral);
  margin-bottom: 8px;
}

.note-card p {
  margin: 0;
  font-size: 14px;
  line-height: 1.5;
  color: var(--ink-2);
}

@media (prefers-reduced-motion: reduce) {
  .chrome .brand, .chrome .num, .ab-panel {
    transform: none !important;
    opacity: 1 !important;
  }
}
</style>

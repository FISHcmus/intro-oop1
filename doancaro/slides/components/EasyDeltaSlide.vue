<script setup lang="ts">
import { ref } from 'vue'
import { onSlideEnter } from '@slidev/client'
import { slideBackgrounds } from './slideBackgrounds'

const mountKey = ref(0)
const bg = slideBackgrounds.easyDelta
onSlideEnter(() => { mountKey.value++ })
</script>

<template>
  <div class="s-delta scene-slide" :key="mountKey">
    <div class="scene-bg cool">
      <img :src="bg" alt="Debug background" />
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
          <b>05</b> &middot; Easy &middot; vùng nhìn
        </div>
      </div>

      <div class="glass-panel delta-panel"
           v-motion
           :initial="{ opacity: 0, x: 24 }"
           :enter="{ opacity: 1, x: 0, transition: { duration: 320, delay: 320 } }">
        <div class="eyebrow on-image">Không quét cả bàn cờ</div>
        <h2 class="scene-title">Easy chỉ nhìn <em>chỗ đáng nhìn</em>.</h2>

        <div class="delta-grid">
          <div class="code-box">
            <div class="scene-kicker">Pseudocode</div>
            <pre><code>checkWinner(lastMove):
  for dir in [ngang, doc, cheo1, cheo2]:
    if count(lastMove, dir) >= 5:
      return WIN
  return NONE

candidateMoves():
  return emptyCells near existing stones</code></pre>
          </div>

          <div class="delta-notes">
            <div class="paper-panel note-card">
              <div class="note-head">Kiểm tra thắng</div>
              <p>Chỉ xét từ nước vừa đánh ra 4 hướng.</p>
            </div>
            <div class="examples">
              <div class="paper-panel ex-card">
                <div class="ex-label">Candidate</div>
                <div class="ex-math">ô trống gần vùng cờ</div>
                <div class="ex-result">ít nhánh hơn</div>
              </div>
              <div class="paper-panel ex-card strong">
                <div class="ex-label">Kết quả</div>
                <div class="ex-math">ít tính toán hơn</div>
                <div class="ex-result">nhanh hơn rõ rệt</div>
              </div>
            </div>
          </div>
        </div>
      </div>
    </div>
  </div>
</template>

<style scoped>
.delta-panel {
  margin-top: 12px;
  margin-bottom: 18px;
  margin-left: auto;
  width: min(900px, 86%);
  padding: 20px 22px 22px;
}

.delta-panel .scene-title {
  font-size: 48px;
  margin-bottom: 12px;
}

.delta-grid {
  display: grid;
  grid-template-columns: 1fr 0.95fr;
  gap: 14px;
}

.code-box pre {
  margin: 10px 0 0;
  padding: 14px 16px;
  border-radius: 22px;
  background: rgba(255, 251, 242, 0.58);
  border: 1px solid rgba(11, 22, 24, 0.10);
  color: var(--ink);
  font-size: 12px;
  line-height: 1.5;
}

.delta-notes {
  display: grid;
  gap: 10px;
}

.note-card,
.ex-card {
  padding: 12px 14px;
}

.note-head,
.ex-label {
  font-family: var(--mono);
  font-size: 10px;
  text-transform: uppercase;
  letter-spacing: 0.12em;
  color: var(--coral);
  margin-bottom: 8px;
}

.note-card p {
  margin: 0;
  font-size: 13px;
  line-height: 1.35;
  color: var(--ink-2);
}

.note-card code {
  font-family: var(--mono);
  font-size: 12px;
}

.examples {
  display: grid;
  grid-template-columns: 1fr 1fr;
  gap: 12px;
}

.ex-math {
  font-family: var(--mono);
  font-size: 14px;
  font-weight: 700;
  color: var(--ink);
}

.ex-result {
  margin-top: 6px;
  font-size: 16px;
  font-weight: 800;
  color: var(--teal-deep);
}

.ex-card.strong .ex-result {
  color: var(--coral);
}

@media (prefers-reduced-motion: reduce) {
  .chrome .brand, .chrome .num, .delta-panel {
    transform: none !important;
    opacity: 1 !important;
  }
}
</style>

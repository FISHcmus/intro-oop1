<script setup lang="ts">
import { ref } from 'vue'
import { onSlideEnter } from '@slidev/client'
import { slideBackgrounds } from './slideBackgrounds'

const mountKey = ref(0)
const bg = slideBackgrounds.easyOverview
onSlideEnter(() => { mountKey.value++ })
</script>

<template>
  <div class="s-easy scene-slide" :key="mountKey">
    <div class="scene-bg warm">
      <img :src="bg" alt="Board background" />
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
          <b>04</b> &middot; Easy &middot; tốc độ
        </div>
      </div>

      <div class="glass-panel easy-panel"
           v-motion
           :initial="{ opacity: 0, y: 24 }"
           :enter="{ opacity: 1, y: 0, transition: { duration: 320, delay: 320 } }">
        <div class="eyebrow on-image">Mức 1 &middot; ưu tiên phản hồi nhanh</div>
        <h2 class="scene-title">Easy ưu tiên <em>tốc độ</em>.</h2>

        <div class="easy-grid">
          <div class="bullets">
            <div class="bullet"><span>1</span><p>Phản hồi nhanh.</p></div>
            <div class="bullet"><span>2</span><p>Dễ tiếp cận với người mới.</p></div>
            <div class="bullet"><span>3</span><p>AI vẫn đủ khôn để tạo thử thách.</p></div>
          </div>

          <div class="paper-panel diagram-box">
            <div class="diagram-head">Goal(Easy)</div>
            <div class="board">
              <div v-for="i in 81" :key="i" class="cell"
                   :class="{ ring: [22,23,24,25,26,31,32,34,35,40,42,49,50,51,58,59,60].includes(i), x: i === 41 }">
                <span v-if="i === 41">×</span>
              </div>
            </div>
            <div class="diagram-note"><code>move nhanh</code> &middot; <code>phản hồi mượt</code> &middot; <code>không nhìn quá sâu</code></div>
          </div>
        </div>
      </div>
    </div>
  </div>
</template>

<style scoped>
.easy-panel {
  margin-top: 12px;
  margin-bottom: 18px;
  width: min(940px, 88%);
  padding: 20px 22px 22px;
}

.easy-panel .scene-title {
  font-size: 48px;
  margin-bottom: 12px;
}

.easy-grid {
  display: grid;
  grid-template-columns: 1.1fr 0.9fr;
  gap: 14px;
  align-items: center;
}

.bullets {
  display: grid;
  gap: 10px;
}

.bullet {
  display: grid;
  grid-template-columns: 34px 1fr;
  gap: 12px;
  align-items: start;
}

.bullet span {
  width: 34px;
  height: 34px;
  display: grid;
  place-items: center;
  border-radius: 999px;
  background: rgba(255, 251, 242, 0.62);
  border: 1px solid rgba(11, 22, 24, 0.12);
  font-family: var(--mono);
  font-size: 13px;
  font-weight: 700;
  color: var(--teal-deep);
}

.bullet p {
  margin: 0;
  font-size: 15px;
  line-height: 1.35;
  color: var(--ink-2);
}

.diagram-box {
  padding: 14px 14px 12px;
}

.diagram-head {
  font-family: var(--mono);
  font-size: 10px;
  text-transform: uppercase;
  letter-spacing: 0.12em;
  color: var(--coral);
  margin-bottom: 10px;
}

.board {
  display: grid;
  grid-template-columns: repeat(9, 20px);
  grid-template-rows: repeat(9, 20px);
  background: rgba(11, 22, 24, 0.04);
  border: 1px solid rgba(11, 22, 24, 0.12);
  padding: 6px;
  width: fit-content;
}

.cell {
  border: 1px solid rgba(11, 22, 24, 0.08);
  display: grid;
  place-items: center;
  font-family: var(--mono);
  font-weight: 800;
  color: var(--coral);
  font-size: 13px;
}

.cell.ring { background: rgba(255, 92, 60, 0.12); }
.cell.x { background: var(--coral); color: var(--paper); }

.diagram-note {
  margin-top: 10px;
  font-size: 13px;
  line-height: 1.35;
  color: var(--ink-2);
}

.diagram-note code {
  font-family: var(--mono);
  font-size: 12px;
}

@media (prefers-reduced-motion: reduce) {
  .chrome .brand, .chrome .num, .easy-panel {
    transform: none !important;
    opacity: 1 !important;
  }
}
</style>

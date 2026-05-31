<script setup lang="ts">
import { ref } from 'vue'
import { onSlideEnter } from '@slidev/client'
import { slideBackgrounds } from './slideBackgrounds'

const mountKey = ref(0)
const bg = slideBackgrounds.minimaxTree
onSlideEnter(() => { mountKey.value++ })
</script>

<template>
  <div class="s-tree scene-slide" :key="mountKey">
    <div class="scene-bg warm">
      <img :src="bg" alt="Tree search background" />
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
          <b>07</b> &middot; Minimax &middot; nhìn trước
        </div>
      </div>

      <div class="glass-panel tree-panel"
           v-motion
           :initial="{ opacity: 0, y: 24 }"
           :enter="{ opacity: 1, y: 0, transition: { duration: 320, delay: 320 } }">
        <div class="eyebrow on-image">Normal &middot; Hard</div>
        <h2 class="scene-title">Thuật toán bắt đầu <em>nhìn trước</em>.</h2>

        <div class="tree-grid">
          <div class="paper-panel tree-box">
<pre><code>MAX (AI)
 ├─ MIN
 │   ├─ MAX
 │   └─ MAX
 ├─ MIN
 │   ├─ MAX
 │   └─ MAX
 └─ MIN
     ├─ MAX
     └─ MAX</code></pre>
            <div class="tree-note">Mỗi tầng đổi vai chọn.</div>
          </div>

          <div class="tree-notes">
            <div class="paper-panel note-card">
              <div class="note-head">MAX / MIN</div>
              <p>AI chọn điểm cao, đối thủ kéo điểm xuống thấp.</p>
            </div>
            <div class="paper-panel note-card">
              <div class="note-head">Stop</div>
              <p>Đến lá cây thì dừng và chấm heuristic.</p>
            </div>
            <div class="paper-panel note-card">
              <div class="note-head">Cái giá</div>
              <p>Số nhánh tăng rất nhanh nếu không cắt tỉa.</p>
            </div>
          </div>
        </div>
      </div>
    </div>
  </div>
</template>

<style scoped>
.tree-panel {
  margin-top: 12px;
  margin-bottom: 18px;
  width: min(780px, 74%);
  padding: 20px 22px 22px;
}

.tree-panel .scene-title {
  font-size: 44px;
  margin-bottom: 12px;
}

.tree-grid {
  display: grid;
  grid-template-columns: 0.92fr 1.08fr;
  gap: 14px;
  align-items: start;
}

.tree-box {
  padding: 12px 14px;
}

.tree-box pre {
  margin: 0;
  font-size: 14px;
  line-height: 1.24;
  color: var(--ink);
}

.tree-note {
  margin-top: 8px;
  font-size: 13px;
  line-height: 1.35;
  color: var(--ink-2);
}

.tree-notes {
  display: grid;
  gap: 10px;
}

.note-card {
  padding: 10px 12px;
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
  font-size: 12px;
  line-height: 1.35;
  color: var(--ink-2);
}

@media (prefers-reduced-motion: reduce) {
  .chrome .brand, .chrome .num, .tree-panel {
    transform: none !important;
    opacity: 1 !important;
  }
}
</style>

<script setup lang="ts">
import { ref } from 'vue'
import { onSlideEnter } from '@slidev/client'
import { slideBackgrounds } from './slideBackgrounds'

const mountKey = ref(0)
const bg = slideBackgrounds.bigIdea
onSlideEnter(() => { mountKey.value++ })
</script>

<template>
  <div class="s-big-idea scene-slide" :key="mountKey">
    <div class="scene-bg cool">
      <img :src="bg" alt="Big idea background" />
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
          <b>03</b> &middot; Ý tưởng chính
        </div>
      </div>

      <div class="glass-panel idea-panel"
           v-motion
           :initial="{ opacity: 0, y: 24 }"
           :enter="{ opacity: 1, y: 0, transition: { duration: 320, delay: 320 } }">
        <div class="eyebrow on-image">Gameplay &middot; AI &middot; bản sắc Việt</div>
        <h2 class="scene-title">Từ bàn cờ quen thuộc thành một <em>game có bản sắc</em>.</h2>

        <div class="idea-grid">
          <div class="idea-copy">
            <p class="scene-copy">
              Lõi vẫn là <em>Caro 15&times;15</em>, nhưng nhóm mở rộng thành một sản phẩm có
              AI nhiều mức độ, Story Mode và một hướng hình ảnh đi từ <em>Sơn Tinh - Thủy Tinh</em>.
            </p>

            <div class="scene-chip-row">
              <div class="scene-chip">Caro 15&times;15</div>
              <div class="scene-chip">AI nhiều mức độ</div>
              <div class="scene-chip">Story Mode</div>
            </div>
          </div>

          <div class="paper-panel idea-note">
            <div class="note-head">Mục tiêu</div>
            <p>Không chỉ làm game chạy được, mà làm nó thành một thế giới có màu riêng.</p>
          </div>
        </div>
      </div>
    </div>
  </div>
</template>

<style scoped>
.idea-panel {
  margin-top: auto;
  width: min(860px, 82%);
  padding: 30px 32px 32px;
}

.idea-panel h2 {
  margin-bottom: 18px;
}

.idea-grid {
  display: grid;
  grid-template-columns: 1.2fr 0.8fr;
  gap: 18px;
  align-items: end;
}

.idea-copy p {
  margin: 0 0 18px;
}

.idea-note {
  padding: 18px;
}

.note-head {
  font-family: var(--mono);
  font-size: 10px;
  text-transform: uppercase;
  letter-spacing: 0.12em;
  color: var(--coral);
  margin-bottom: 8px;
}

.idea-note p {
  margin: 0;
  font-size: 14px;
  line-height: 1.5;
  color: var(--ink-2);
}

@media (prefers-reduced-motion: reduce) {
  .chrome .brand, .chrome .num, .idea-panel {
    transform: none !important;
    opacity: 1 !important;
  }
}
</style>

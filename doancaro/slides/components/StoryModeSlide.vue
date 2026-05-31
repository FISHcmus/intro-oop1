<script setup lang="ts">
import { ref } from 'vue'
import { onSlideEnter } from '@slidev/client'
import { slideBackgrounds } from './slideBackgrounds'

const mountKey = ref(0)
const bg = slideBackgrounds.story
onSlideEnter(() => { mountKey.value++ })
</script>

<template>
  <div class="s-story scene-slide" :key="mountKey">
    <div class="scene-bg warm">
      <img :src="bg" alt="Story mode background" />
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
          <b>10</b> &middot; Story Mode
        </div>
      </div>

      <div class="glass-panel story-panel"
           v-motion
           :initial="{ opacity: 0, x: 24 }"
           :enter="{ opacity: 1, x: 0, transition: { duration: 320, delay: 320 } }">
        <div class="eyebrow on-image">Sơn Tinh - Thủy Tinh &middot; Mỵ Nương</div>
        <h2 class="scene-title">Story Mode không phải phần <em>gắn thêm cho đẹp</em>.</h2>

        <p class="scene-copy story-copy">
          Cốt truyện được xây từ <em>Sơn Tinh - Thủy Tinh</em>, ký ức mùa nước và một
          góc nhìn mới về Mỵ Nương, chứ không chỉ thêm fantasy để trang trí.
        </p>

        <div class="scene-chip-row">
          <div class="scene-chip">lõi vẫn là Caro</div>
          <div class="scene-chip">truyền thuyết thành gameplay</div>
        </div>

        <div class="paper-panel story-note">
          <div class="note-head">Điểm chính</div>
          <p>Phần khác nhất của đồ án là cách kể chuyện được đặt ngay vào trải nghiệm chơi.</p>
        </div>
      </div>
    </div>
  </div>
</template>

<style scoped>
.story-panel {
  margin-top: 20px;
  margin-bottom: 18px;
  width: min(860px, 80%);
  padding: 20px 22px 22px;
}

.story-panel .scene-title {
  font-size: 50px;
  margin-bottom: 10px;
}

.story-copy {
  margin: 0 0 14px;
  font-size: 15px;
  line-height: 1.42;
}

.story-note {
  margin-top: 14px;
  padding: 12px 14px;
}

.note-head {
  font-family: var(--mono);
  font-size: 10px;
  text-transform: uppercase;
  letter-spacing: 0.12em;
  color: var(--coral);
  margin-bottom: 8px;
}

.story-note p {
  margin: 0;
  font-size: 13px;
  line-height: 1.4;
  color: var(--ink-2);
}

.story-note code {
  font-family: var(--mono);
  font-size: 12px;
}

@media (prefers-reduced-motion: reduce) {
  .chrome .brand, .chrome .num, .story-panel {
    transform: none !important;
    opacity: 1 !important;
  }
}
</style>

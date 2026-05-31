<script setup lang="ts">
import { ref } from 'vue'
import { onSlideEnter } from '@slidev/client'

const mountKey = ref(0)
onSlideEnter(() => { mountKey.value++ })
</script>

<template>
  <div class="s-demo" :key="mountKey">
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
        <b>02</b> &middot; Demo
      </div>
    </div>

    <div class="eyebrow"
         v-motion
         :initial="{ opacity: 0, x: -6 }"
         :enter="{ opacity: 1, x: 0, transition: { duration: 220, delay: 250 } }">
      Giới thiệu nhanh 20 giây
    </div>

    <h2 v-motion
        :initial="{ opacity: 0, y: 18 }"
        :enter="{ opacity: 1, y: 0, transition: { duration: 260, delay: 320 } }">
      Xem trò chơi <em>vận hành</em>.
    </h2>

    <div class="video-frame"
         v-motion
         :initial="{ opacity: 0, scale: 0.96 }"
         :enter="{ opacity: 1, scale: 1, transition: { type: 'spring', stiffness: 180, damping: 20, delay: 500 } }">
      <img class="video" src="/demo.png" alt="Caro game screenshot" />
      <div class="video-caption">
        Bàn 3D &middot; lượt AI &middot; lưu/tải &middot; hiệu ứng hạt
      </div>
    </div>
  </div>
</template>

<style scoped>
.s-demo {
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
  font-size: 38px;
  letter-spacing: -0.03em;
  margin-bottom: 28px;
  will-change: transform, opacity;
}
h2 em { color: var(--coral); font-style: normal; }

.video-frame {
  flex: 1;
  min-height: 0;
  display: flex;
  flex-direction: column;
  align-items: center;
  justify-content: center;
  gap: 12px;
  will-change: transform, opacity;
}

.video {
  display: block;
  max-width: 100%;
  max-height: 100%;
  width: auto;
  height: auto;
  object-fit: contain;
  background: var(--ink);
  border: 1px solid var(--ink);
  border-radius: 4px;
  box-shadow: 0 22px 48px -16px rgba(11,22,24,0.35);
}

.video-caption {
  font-family: var(--mono);
  font-size: 11px;
  letter-spacing: 0.08em;
  text-transform: uppercase;
  color: var(--muted);
}

@media (prefers-reduced-motion: reduce) {
  .chrome .brand, .chrome .num, .eyebrow, h2, .video-frame {
    transform: none !important;
    opacity: 1 !important;
  }
}
</style>

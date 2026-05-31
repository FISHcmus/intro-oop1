<script setup lang="ts">
import { ref } from 'vue'
import { onSlideEnter } from '@slidev/client'
import gameplayShot from '../../public/gameplay.png'
import menuShot from '../../public/menu.png'

const mountKey = ref(0)
onSlideEnter(() => { mountKey.value++ })
</script>

<template>
  <div class="s-demo scene-slide" :key="mountKey">
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
          <b>02</b> &middot; Game thật
        </div>
      </div>

      <div class="glass-panel demo-panel"
           v-motion
           :initial="{ opacity: 0, y: 24 }"
           :enter="{ opacity: 1, y: 0, transition: { duration: 320, delay: 320 } }">
        <div class="eyebrow">Menu &middot; Story &middot; Gameplay</div>
        <h2 class="scene-title">Game đã <em>chạy thật</em>.</h2>

        <div class="showcase-grid">
          <figure class="paper-panel shot-card">
            <img :src="gameplayShot" alt="Ảnh chụp gameplay chính" />
            <figcaption>Gameplay chính</figcaption>
          </figure>

          <figure class="paper-panel shot-card">
            <img :src="menuShot" alt="Ảnh chụp menu chính" />
            <figcaption>Menu mở đầu</figcaption>
          </figure>
        </div>
      </div>
    </div>
  </div>
</template>

<style scoped>
.demo-panel {
  margin-top: 36px;
  width: min(1040px, 98%);
  padding: 24px 26px 26px;
}

.demo-panel .scene-title {
  font-size: 50px;
  margin-bottom: 14px;
}

.showcase-grid {
  display: grid;
  grid-template-columns: 1.15fr 0.85fr;
  gap: 14px;
  align-items: stretch;
}

.shot-card {
  margin: 0;
  padding: 8px;
  overflow: hidden;
  display: flex;
  flex-direction: column;
  gap: 8px;
}

.shot-card img {
  width: 100%;
  aspect-ratio: 16 / 8.4;
  object-fit: cover;
  border-radius: 18px;
  border: 1px solid rgba(11, 22, 24, 0.10);
}

.shot-card figcaption {
  font-family: var(--mono);
  font-size: 10px;
  letter-spacing: 0.08em;
  text-transform: uppercase;
  color: var(--ink);
}

@media (max-width: 900px) {
  .showcase-grid {
    grid-template-columns: 1fr;
  }
}

@media (prefers-reduced-motion: reduce) {
  .chrome .brand, .chrome .num, .demo-panel {
    transform: none !important;
    opacity: 1 !important;
  }
}
</style>

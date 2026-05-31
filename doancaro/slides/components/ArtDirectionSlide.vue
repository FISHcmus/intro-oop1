<script setup lang="ts">
import { ref } from 'vue'
import { onSlideEnter } from '@slidev/client'
import gameplayShot from '../../public/gameplay.png'
import storyShot from '../../public/story-telling.png'

const mountKey = ref(0)
onSlideEnter(() => { mountKey.value++ })
</script>

<template>
  <div class="s-art-direction scene-slide" :key="mountKey">
    <div class="scene-shell">
      <div class="chrome dark">
        <div class="brand">
          <span class="dot"></span>CARO
        </div>
        <div class="num">
          <b>14</b> &middot; Hình ảnh trong game
        </div>
      </div>

      <div class="glass-panel art-panel">
        <div class="eyebrow">Story &middot; Campaign &middot; Gameplay</div>
        <h2 class="scene-title">Hình ảnh <em>trong game</em>.</h2>

        <div class="gallery-grid">
          <figure class="paper-panel gallery-card">
            <img :src="storyShot" alt="Ảnh chụp màn hình kể chuyện trong game" />
            <figcaption>Khung kể chuyện</figcaption>
          </figure>

          <figure class="paper-panel gallery-card">
            <img :src="gameplayShot" alt="Ảnh chụp gameplay trên bàn cờ 3D" />
            <figcaption>Bàn cờ trong trận</figcaption>
          </figure>
        </div>
      </div>
    </div>
  </div>
</template>

<style scoped>
.art-panel {
  margin-top: 36px;
  width: min(1040px, 98%);
  padding: 24px 26px 26px;
}

.art-panel .scene-title {
  font-size: 50px;
  margin-bottom: 14px;
}

.gallery-grid {
  display: grid;
  grid-template-columns: 1fr 1fr;
  gap: 12px;
}

.gallery-card {
  margin: 0;
  padding: 8px;
  overflow: hidden;
  display: flex;
  flex-direction: column;
  gap: 8px;
}

.gallery-card img {
  width: 100%;
  aspect-ratio: 16 / 8.2;
  object-fit: cover;
  border-radius: 18px;
  border: 1px solid rgba(11, 22, 24, 0.10);
}

.gallery-card figcaption {
  font-family: var(--mono);
  font-size: 10px;
  letter-spacing: 0.08em;
  text-transform: uppercase;
  color: var(--ink);
}

@media (max-width: 900px) {
  .gallery-grid {
    grid-template-columns: 1fr;
  }
}
</style>

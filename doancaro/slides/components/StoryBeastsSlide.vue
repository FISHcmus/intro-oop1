<script setup lang="ts">
import { ref } from 'vue'
import { onSlideEnter } from '@slidev/client'
import { slideBackgrounds } from './slideBackgrounds'

const mountKey = ref(0)
const bg = slideBackgrounds.storyBeasts
onSlideEnter(() => { mountKey.value++ })
</script>

<template>
  <div class="s-story-beasts scene-slide" :key="mountKey">
    <div class="scene-bg cool">
      <img :src="bg" alt="Story beasts background" />
    </div>

    <div class="scene-shell">
      <div class="chrome dark">
        <div class="brand">
          <span class="dot"></span>CARO
        </div>
        <div class="num">
          <b>13</b> &middot; Linh vật
        </div>
      </div>

      <div class="glass-panel beasts-panel">
        <div class="eyebrow on-image">Ba linh vật &middot; ba cơ chế hỗ trợ</div>
        <h2 class="scene-title">Ba linh vật, ba <em>điểm nhấn</em> của Story Mode.</h2>

        <div class="beasts-grid">
          <div class="paper-panel beast-card">
            <div class="beast-name">Voi chín ngà</div>
            <p>Gắn với hoàn tác và giữ thế cờ.</p>
          </div>
          <div class="paper-panel beast-card">
            <div class="beast-name">Gà chín cựa</div>
            <p>Gây nhiễu nước đi của AI.</p>
          </div>
          <div class="paper-panel beast-card">
            <div class="beast-name">Ngựa chín hồng mao</div>
            <p>Hỗ trợ hồi sinh ở trận cuối.</p>
          </div>
        </div>
      </div>
    </div>
  </div>
</template>

<style scoped>
.beasts-panel {
  margin-top: auto;
  width: min(900px, 84%);
  padding: 30px 32px 32px;
}

.beasts-panel h2 {
  margin-bottom: 18px;
}

.beasts-grid {
  display: grid;
  grid-template-columns: repeat(3, 1fr);
  gap: 12px;
}

.beast-card {
  padding: 18px;
}

.beast-name {
  font-family: var(--mono);
  font-size: 11px;
  text-transform: uppercase;
  letter-spacing: 0.1em;
  color: var(--coral);
  margin-bottom: 10px;
}

.beast-card p {
  margin: 0;
  font-size: 14px;
  line-height: 1.5;
  color: var(--ink-2);
}
</style>

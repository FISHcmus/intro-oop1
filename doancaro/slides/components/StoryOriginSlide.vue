<script setup lang="ts">
import { ref } from 'vue'
import { onSlideEnter } from '@slidev/client'
import { slideBackgrounds } from './slideBackgrounds'

const mountKey = ref(0)
const bg = slideBackgrounds.storyOrigin
onSlideEnter(() => { mountKey.value++ })
</script>

<template>
  <div class="s-story-origin scene-slide" :key="mountKey">
    <div class="scene-bg warm">
      <img :src="bg" alt="Story origin background" />
    </div>

    <div class="scene-shell">
      <div class="chrome dark">
        <div class="brand">
          <span class="dot"></span>CARO
        </div>
        <div class="num">
          <b>11</b> &middot; Diễn giải thần thoại
        </div>
      </div>

      <div class="glass-panel origin-panel">
        <div class="eyebrow on-image">Lạc Long Quân &middot; Sơn Tinh &middot; Thủy Tinh</div>
        <h2 class="scene-title">Diễn giải lại <em>Sơn Tinh - Thủy Tinh</em>.</h2>

        <div class="origin-flow paper-panel">
          <div class="flow-line"><span>Lạc Long Quân</span><b>&rarr;</b><span>gửi thân vào đất</span></div>
          <div class="flow-line"><span>Đại Việt</span><b>&rarr;</b><span>thành nơi tranh quyền của núi và nước</span></div>
          <div class="flow-line"><span>Mỵ Nương</span><b>&rarr;</b><span>người phán xử và ghi chép</span></div>
        </div>

        <div class="scene-chip-row">
          <div class="scene-chip">không còn là món hôn ước</div>
          <div class="scene-chip">truyền thuyết được đổi góc nhìn</div>
        </div>
      </div>
    </div>
  </div>
</template>

<style scoped>
.origin-panel {
  margin-top: auto;
  width: min(880px, 82%);
  padding: 30px 32px 32px;
}

.origin-panel h2 {
  margin-bottom: 18px;
}

.origin-flow {
  padding: 18px;
  display: grid;
  gap: 12px;
  margin-bottom: 18px;
}

.flow-line {
  display: grid;
  grid-template-columns: 180px 28px 1fr;
  gap: 10px;
  align-items: center;
  font-size: 15px;
  line-height: 1.45;
  color: var(--ink);
}

.flow-line span:first-child {
  font-family: var(--mono);
  font-size: 12px;
  text-transform: uppercase;
  letter-spacing: 0.08em;
  color: var(--coral);
}

.flow-line b {
  font-family: var(--mono);
  color: var(--teal-deep);
  text-align: center;
}
</style>

<script setup lang="ts">
import { ref } from 'vue'
import { onSlideEnter } from '@slidev/client'
import { slideBackgrounds } from './slideBackgrounds'

const mountKey = ref(0)
const bg = slideBackgrounds.storyProgression
onSlideEnter(() => { mountKey.value++ })
</script>

<template>
  <div class="s-story-prog scene-slide" :key="mountKey">
    <div class="scene-bg warm">
      <img :src="bg" alt="Story progression background" />
    </div>

    <div class="scene-shell">
      <div class="chrome dark">
        <div class="brand">
          <span class="dot"></span>CARO
        </div>
        <div class="num">
          <b>12</b> &middot; Campaign
        </div>
      </div>

      <div class="glass-panel prog-panel">
        <div class="eyebrow on-image">4 chặng &middot; nhịp leo thang rõ ràng</div>
        <h2 class="scene-title">Campaign <em>4 chặng</em>.</h2>

        <div class="prog-grid">
          <div class="paper-panel prog-card">
            <div class="prog-head">Set 1</div>
            <div class="prog-line">nước tràn vào phố</div>
            <div class="prog-note">mở đầu cho hành trình</div>
          </div>
          <div class="paper-panel prog-card">
            <div class="prog-head">Set 2</div>
            <div class="prog-line">nứt long mạch</div>
            <div class="prog-note">khó dần và căng hơn</div>
          </div>
          <div class="paper-panel prog-card">
            <div class="prog-head">Set 3</div>
            <div class="prog-line">biển dâng</div>
            <div class="prog-note">quy mô xung đột mở rộng</div>
          </div>
          <div class="paper-panel prog-card">
            <div class="prog-head">Final Boss</div>
            <div class="prog-line">đối đầu Thủy Tinh</div>
            <div class="prog-note">cao trào của campaign</div>
          </div>
        </div>

        <div class="scene-chip-row">
          <div class="scene-chip">Set 1 &rarr; Set 2 &rarr; Set 3 &rarr; Final Boss</div>
          <div class="scene-chip">AI khó dần</div>
        </div>
      </div>
    </div>
  </div>
</template>

<style scoped>
.prog-panel {
  margin-top: 34px;
  width: min(860px, 80%);
  padding: 24px 26px 26px;
}

.prog-panel h2 {
  margin-bottom: 14px;
}

.prog-grid {
  display: grid;
  grid-template-columns: 1fr 1fr;
  gap: 10px;
  margin-bottom: 14px;
}

.prog-card {
  padding: 14px 16px;
}

.prog-head {
  font-family: var(--mono);
  font-size: 10px;
  text-transform: uppercase;
  letter-spacing: 0.12em;
  color: var(--coral);
  margin-bottom: 10px;
}

.prog-line {
  font-family: var(--mono);
  font-size: 16px;
  font-weight: 700;
  color: var(--ink);
}

.prog-note {
  margin-top: 8px;
  font-size: 14px;
  line-height: 1.45;
  color: var(--ink-2);
}

</style>

<script setup lang="ts">
import { ref } from 'vue'
import { onSlideEnter } from '@slidev/client'

const mountKey = ref(0)
onSlideEnter(() => { mountKey.value++ })
</script>

<template>
  <div class="s-close" :key="mountKey">
    <div class="chrome chrome-dark">
      <div class="brand"
           v-motion
           :initial="{ opacity: 0, scale: 1.15, rotate: -2 }"
           :enter="{ opacity: 1, scale: 1, rotate: 0, transition: { duration: 220, delay: 100 } }">
        <span class="dot"></span>CARO
      </div>
      <div class="num"
           v-motion
           :initial="{ opacity: 0, scale: 1.15, rotate: 2 }"
           :enter="{ opacity: 1, scale: 1, rotate: 0, transition: { duration: 220, delay: 180 } }">
        <b>10</b> &middot; kết &middot; Q&amp;A
      </div>
    </div>

    <div class="center">
      <div class="eyebrow"
           v-motion
           :initial="{ opacity: 0, y: -6 }"
           :enter="{ opacity: 1, y: 0, transition: { duration: 240, delay: 280 } }">
        Cảm ơn thầy cô đã lắng nghe
      </div>

      <h1 v-motion
          :initial="{ opacity: 0, y: 24 }"
          :enter="{ opacity: 1, y: 0, transition: { duration: 320, delay: 400 } }">
        <span class="ca">C</span><span class="ca">a</span><span class="ca">r</span><span class="ca">o</span>
        <span class="plus">+</span>
        <span class="ai">AI</span>
      </h1>

      <div class="subtitle"
           v-motion
           :initial="{ opacity: 0, y: 12 }"
           :enter="{ opacity: 1, y: 0, transition: { duration: 280, delay: 620 } }">
        Câu hỏi &amp; thảo luận
      </div>

      <div class="hints"
           v-motion
           :initial="{ opacity: 0, y: 12 }"
           :enter="{ opacity: 1, y: 0, transition: { duration: 280, delay: 780 } }">
        <div class="hint">
          <div class="h-label">Mã nguồn</div>
          <code>doancaro/</code>
          <div class="h-note">Visual Studio &middot; C++14 &middot; raylib</div>
        </div>
        <div class="hint">
          <div class="h-label">Build</div>
          <code>CaroGame.sln</code>
          <div class="h-note">mở trong VS 2022 → F5</div>
        </div>
        <div class="hint">
          <div class="h-label">Điểm vào</div>
          <code>src/main.cpp</code>
          <div class="h-note">Game::run() chạy vòng lặp chính</div>
        </div>
      </div>

      <div class="team"
           v-motion
           :initial="{ opacity: 0, y: 10 }"
           :enter="{ opacity: 1, y: 0, transition: { duration: 260, delay: 960 } }">
        <span class="t-name">Nhân <span class="t-id">25310023</span></span>
        <span class="t-sep">·</span>
        <span class="t-name">Hằng <span class="t-id">25310057</span></span>
        <span class="t-sep">·</span>
        <span class="t-name">Trâm <span class="t-id">25310043</span></span>
      </div>
    </div>

    <div class="foot foot-dark">
      <div>HCMUS &middot; Nhập môn OOP 1 &middot; Học kỳ 2 / 2025&ndash;2026</div>
      <div>cảm ơn</div>
    </div>
  </div>
</template>

<style scoped>
.s-close {
  position: absolute;
  inset: 0;
  background: var(--ink);
  color: var(--paper);
  padding: 90px 60px 70px;
  overflow: hidden;
  display: flex;
  flex-direction: column;
}

.chrome-dark {
  color: var(--paper);
}
.chrome-dark :deep(.brand) { color: var(--paper); }
.chrome-dark :deep(.dot) { background: var(--coral); }
.chrome-dark :deep(.num) { color: rgba(250,250,247,0.72); }
.chrome-dark :deep(.num b) { color: var(--paper); }

.center {
  flex: 1;
  display: flex;
  flex-direction: column;
  align-items: flex-start;
  justify-content: center;
  gap: 18px;
  max-width: 900px;
}

.eyebrow {
  font-family: var(--mono);
  font-size: 11px;
  letter-spacing: 0.14em;
  text-transform: uppercase;
  color: rgba(250,250,247,0.6);
  display: flex; align-items: center; gap: 8px;
  will-change: transform, opacity;
}
.eyebrow::before {
  content: ""; width: 28px; height: 1px; background: var(--coral);
}

h1 {
  font-size: 120px;
  font-weight: 800;
  letter-spacing: -0.04em;
  line-height: 1;
  margin: 0;
  display: flex;
  align-items: baseline;
  gap: 0;
  will-change: transform, opacity;
}
h1 .ca { color: var(--paper); }
h1 .plus {
  color: var(--coral);
  font-size: 96px;
  margin: 0 18px;
}
h1 .ai {
  color: var(--teal);
  letter-spacing: -0.02em;
}

.subtitle {
  font-size: 22px;
  color: rgba(250,250,247,0.82);
  font-weight: 500;
  margin-top: -6px;
  will-change: transform, opacity;
}

.hints {
  display: grid;
  grid-template-columns: repeat(3, auto);
  gap: 28px;
  margin-top: 16px;
  will-change: transform, opacity;
}
.hint {
  display: flex;
  flex-direction: column;
  gap: 3px;
  border-left: 2px solid var(--coral);
  padding-left: 12px;
}
.h-label {
  font-family: var(--mono);
  font-size: 9px;
  text-transform: uppercase;
  letter-spacing: 0.14em;
  color: var(--coral);
}
.hint code {
  font-family: var(--mono);
  font-size: 13px;
  color: var(--paper);
  font-weight: 700;
}
.h-note {
  font-size: 10.5px;
  color: rgba(250,250,247,0.55);
  letter-spacing: 0.02em;
}

.team {
  margin-top: 18px;
  display: flex;
  align-items: baseline;
  gap: 14px;
  font-family: var(--mono);
  font-size: 13px;
  color: rgba(250,250,247,0.78);
  will-change: transform, opacity;
}
.t-name { font-weight: 600; }
.t-id { color: rgba(250,250,247,0.48); font-size: 11px; margin-left: 4px; }
.t-sep { color: rgba(250,250,247,0.35); }

.foot-dark {
  color: rgba(250,250,247,0.55);
  border-top: 1px solid rgba(250,250,247,0.14);
}
.foot-dark :deep(div) { color: rgba(250,250,247,0.55); }

@media (prefers-reduced-motion: reduce) {
  .chrome .brand, .chrome .num, .eyebrow, h1, .subtitle, .hints, .team {
    transform: none !important;
    opacity: 1 !important;
  }
}
</style>

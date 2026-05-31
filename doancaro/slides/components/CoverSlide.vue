<script setup lang="ts">
import { ref } from 'vue'
import { onSlideEnter } from '@slidev/client'
import { slideBackgrounds } from './slideBackgrounds'

const mountKey = ref(0)
const bg = slideBackgrounds.cover
onSlideEnter(() => { mountKey.value++ })
</script>

<template>
  <div class="s-cover scene-slide" :key="mountKey">
    <div class="scene-bg warm">
      <img :src="bg" alt="Story world background" />
    </div>

    <div class="scene-shell">
      <div class="chrome dark">
        <div class="brand"
             v-motion
             :initial="{ opacity: 0, scale: 1.12, rotate: -2 }"
             :enter="{ opacity: 1, scale: 1, rotate: 0, transition: { duration: 220, delay: 120 } }">
          <span class="dot"></span>CARO
        </div>
        <div class="num"
             v-motion
             :initial="{ opacity: 0, scale: 1.12, rotate: 2 }"
             :enter="{ opacity: 1, scale: 1, rotate: 0, transition: { duration: 220, delay: 180 } }">
          HCMUS &middot; OOP1 &middot; Đồ án cuối kỳ
        </div>
      </div>

      <div class="cover-layout">
        <div class="glass-panel cover-panel"
             v-motion
             :initial="{ opacity: 0, y: 24, scale: 0.98 }"
             :enter="{ opacity: 1, y: 0, scale: 1, transition: { duration: 320, delay: 320 } }">
          <div class="eyebrow on-image">Đồ án cuối kỳ &middot; nhóm 13</div>
          <h1 class="scene-title cover-title">CARO</h1>
          <p class="scene-copy cover-copy">
            Từ bàn cờ quen thuộc, nhóm phát triển thành một game có AI nhiều mức độ,
            Story Mode và thế giới hình ảnh lấy cảm hứng từ thần thoại Việt.
          </p>
          <div class="scene-chip-row">
            <div class="scene-chip">15&times;15</div>
            <div class="scene-chip">Greedy + Minimax + TT</div>
            <div class="scene-chip">Story Mode</div>
            <div class="scene-chip">Wuxia Storm</div>
          </div>
        </div>

        <div class="paper-panel cover-meta"
             v-motion
             :initial="{ opacity: 0, x: 24 }"
             :enter="{ opacity: 1, x: 0, transition: { duration: 280, delay: 520 } }">
          <div class="meta-kicker">Thông tin</div>
          <div class="meta-line"><b>GVHD</b> &middot; Tiến sĩ Trương Toàn Thịnh</div>
          <div class="meta-line"><b>Nhóm 13</b></div>
          <div class="meta-line"><b>Nguyễn Hữu Thiện Nhân</b> &middot; 25310023</div>
          <div class="meta-line"><b>Bùi Thị Minh Hằng</b> &middot; 25310057</div>
          <div class="meta-line"><b>Phạm Ngọc Trâm</b> &middot; 25310043</div>
          <div class="meta-foot">C++14 &middot; raylib &middot; Slidev</div>
        </div>
      </div>

      <div class="foot dark">
        <div>Nhập môn Lập trình Hướng đối tượng 1</div>
        <div>slide nộp cuối kỳ</div>
      </div>
    </div>
  </div>
</template>

<style scoped>
.cover-layout {
  margin-top: auto;
  margin-bottom: auto;
  display: grid;
  grid-template-columns: minmax(0, 760px) 280px;
  gap: 22px;
  align-items: end;
}

.cover-panel {
  padding: 34px 36px 36px;
}

.cover-title {
  font-size: 118px;
  margin-bottom: 14px;
}

.cover-copy {
  max-width: 620px;
  margin: 0 0 20px;
  font-size: 20px;
  line-height: 1.42;
}

.cover-meta {
  padding: 22px 22px 20px;
  align-self: center;
}

.meta-kicker {
  font-family: var(--mono);
  font-size: 11px;
  text-transform: uppercase;
  letter-spacing: 0.12em;
  color: var(--coral);
  margin-bottom: 12px;
}

.meta-line {
  font-size: 14px;
  line-height: 1.5;
  color: var(--ink);
}

.meta-line + .meta-line {
  margin-top: 8px;
}

.meta-foot {
  margin-top: 16px;
  padding-top: 12px;
  border-top: 1px solid rgba(11, 22, 24, 0.12);
  font-family: var(--mono);
  font-size: 11px;
  color: var(--muted);
  text-transform: uppercase;
  letter-spacing: 0.08em;
}

@media (prefers-reduced-motion: reduce) {
  .chrome .brand, .chrome .num, .cover-panel, .cover-meta {
    transform: none !important;
    opacity: 1 !important;
  }
}
</style>

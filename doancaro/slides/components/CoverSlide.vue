<script setup lang="ts">
import { ref } from 'vue'
import { onSlideEnter } from '@slidev/client'

const mountKey = ref(0)
onSlideEnter(() => { mountKey.value++ })
</script>

<template>
  <div class="s-cover" :key="mountKey">
    <div class="rule rule-top"></div>
    <div class="rule rule-bottom"></div>

    <div class="chrome">
      <div class="brand"
           v-motion
           :initial="{ opacity: 0, scale: 1.15, rotate: -2 }"
           :enter="{ opacity: 1, scale: 1, rotate: 0, transition: { duration: 200, delay: 150 } }">
        <span class="dot"></span>CARO
      </div>
      <div class="num"
           v-motion
           :initial="{ opacity: 0, scale: 1.15, rotate: 2 }"
           :enter="{ opacity: 1, scale: 1, rotate: 0, transition: { duration: 200, delay: 220 } }">
        HCMUS &middot; OOP1 &middot; Đồ án cuối kỳ
      </div>
    </div>

    <div class="cover-grid">
      <div class="cover-text">
        <div class="eyebrow">
          <span class="eb-text"
                v-motion
                :initial="{ opacity: 0, x: -6 }"
                :enter="{ opacity: 1, x: 0, transition: { duration: 220, delay: 500 } }">
            Buổi bảo vệ cuối kỳ &middot; ~10 phút
          </span>
        </div>
        <div class="wordmark">
          <span class="l" v-motion
                :initial="{ opacity: 0, y: 20 }"
                :enter="{ opacity: 1, y: 0, transition: { duration: 220, delay: 600 } }">C</span><span class="l" v-motion
                :initial="{ opacity: 0, y: 20 }"
                :enter="{ opacity: 1, y: 0, transition: { duration: 220, delay: 660 } }">a</span><span class="l" v-motion
                :initial="{ opacity: 0, y: 20 }"
                :enter="{ opacity: 1, y: 0, transition: { duration: 220, delay: 720 } }">r</span><span class="l" v-motion
                :initial="{ opacity: 0, y: 20 }"
                :enter="{ opacity: 1, y: 0, transition: { duration: 220, delay: 780 } }">o</span><span class="plus" v-motion
                :initial="{ opacity: 0, scale: 0, rotate: -15 }"
                :enter="{ opacity: 1, scale: 1, rotate: 0, transition: { type: 'spring', stiffness: 300, damping: 12, delay: 880 } }">+</span><span class="ai-l" v-motion
                :initial="{ opacity: 0, y: 20 }"
                :enter="{ opacity: 1, y: 0, transition: { duration: 220, delay: 1000 } }">A</span><span class="ai-l" v-motion
                :initial="{ opacity: 0, y: 20 }"
                :enter="{ opacity: 1, y: 0, transition: { duration: 220, delay: 1060 } }">I</span>
        </div>
        <div class="tag"
             v-motion
             :initial="{ opacity: 0, y: 12 }"
             :enter="{ opacity: 1, y: 0, transition: { duration: 280, delay: 1150 } }">
          Cờ Caro 3D viết bằng raylib —<br/>
          ba mức AI từ <em>greedy one-ply</em> tới <em>minimax + TT</em>.
        </div>
      </div>

      <div class="cover-board"
           v-motion
           :initial="{ opacity: 0, scale: 0.92 }"
           :enter="{ opacity: 1, scale: 1, transition: { type: 'spring', stiffness: 200, damping: 18, delay: 1200 } }">
        <div class="board-grid">
          <div v-for="i in 49" :key="i" class="cell"
               :class="{
                 'x': [17, 25, 33].includes(i),
                 'o': [24, 32].includes(i)
               }">
            <span v-if="[17, 25, 33].includes(i)">×</span>
            <span v-else-if="[24, 32].includes(i)">○</span>
          </div>
        </div>
      </div>
    </div>

    <div class="meta">
      <div>
        <div class="team"
             v-motion
             :initial="{ opacity: 0, y: 10 }"
             :enter="{ opacity: 1, y: 0, transition: { duration: 220, delay: 1250 } }">Nhóm 13 &middot; HCMUS FIT</div>
        <div class="members"
             v-motion
             :initial="{ opacity: 0, y: 10 }"
             :enter="{ opacity: 1, y: 0, transition: { duration: 220, delay: 1320 } }">Nhân 25310023 &middot; Hằng 25310057 &middot; Trâm 25310043</div>
      </div>
      <div class="stamp"
           v-motion
           :initial="{ opacity: 0, scale: 1.15, rotate: -1.5 }"
           :enter="{ opacity: 1, scale: 1, rotate: 0, transition: { type: 'spring', stiffness: 260, damping: 14, delay: 1400 } }">raylib &middot; C++14</div>
    </div>
  </div>
</template>

<style scoped>
.s-cover {
  position: absolute;
  inset: 0;
  background:
    radial-gradient(ellipse 700px 450px at 85% 110%, rgba(26,138,122,0.18), transparent 60%),
    radial-gradient(ellipse 500px 350px at 0% 0%, rgba(255,92,60,0.08), transparent 50%),
    var(--paper);
  display: flex;
  flex-direction: column;
  justify-content: center;
  padding: 0 70px;
  overflow: hidden;
}

.rule {
  position: absolute;
  height: 1px;
  background: var(--ink);
  opacity: 0.12;
  z-index: 4;
  left: 36px;
  right: 36px;
  transform: scaleX(0);
  transform-origin: left center;
  animation: rule-draw 520ms cubic-bezier(.2,.8,.2,1) 40ms forwards;
}
.rule-top { top: 62px; }
.rule-bottom {
  bottom: 76px;
  transform-origin: right center;
  animation-delay: 80ms;
}

@keyframes rule-draw { to { transform: scaleX(1); } }

.cover-grid {
  display: grid;
  grid-template-columns: 1.1fr 0.7fr;
  align-items: center;
  gap: 40px;
}

.eyebrow {
  font-family: var(--mono);
  font-size: 11px;
  color: var(--muted);
  letter-spacing: 0.1em;
  text-transform: uppercase;
  margin-bottom: 20px;
  display: flex; align-items: center; gap: 7px;
}
.eyebrow::before {
  content: ""; width: 30px; height: 1px; background: var(--ink);
  transform: scaleX(0);
  transform-origin: left center;
  animation: rule-draw 320ms cubic-bezier(.2,.8,.2,1) 380ms forwards;
}
.eb-text { display: inline-block; will-change: transform, opacity; }

.wordmark {
  font-size: 108px;
  line-height: 1;
  font-weight: 900;
  letter-spacing: -0.06em;
  color: var(--ink);
  margin-bottom: 18px;
  font-family: var(--sans);
}
.wordmark .l,
.wordmark .plus,
.wordmark .ai-l {
  display: inline-block;
  will-change: transform, opacity;
}
.wordmark .plus {
  color: var(--coral);
  font-size: 72px;
  vertical-align: middle;
  margin: 0 8px;
  letter-spacing: 0;
}
.wordmark .ai-l { color: var(--teal); }

.tag {
  font-size: 20px;
  font-weight: 500;
  line-height: 1.25;
  max-width: 520px;
  letter-spacing: -0.01em;
  color: var(--ink);
  font-family: var(--sans);
  will-change: transform, opacity;
}
.tag em { color: var(--teal); font-style: normal; font-weight: 700; }

.cover-board {
  display: flex;
  align-items: center;
  justify-content: center;
  will-change: transform, opacity;
}

.board-grid {
  display: grid;
  grid-template-columns: repeat(7, 28px);
  grid-template-rows: repeat(7, 28px);
  gap: 0;
  background: var(--paper-2);
  border: 2px solid var(--ink);
  padding: 8px;
  box-shadow: 0 14px 32px -12px rgba(26,138,122,0.28);
}
.cell {
  border: 1px solid rgba(11,22,24,0.12);
  display: grid;
  place-items: center;
  font-family: var(--mono);
  font-size: 18px;
  font-weight: 700;
  color: transparent;
}
.cell.x { color: var(--coral); }
.cell.o { color: var(--teal); }

.meta {
  position: absolute;
  bottom: 36px; left: 70px; right: 70px;
  display: flex; justify-content: space-between; align-items: flex-end;
  font-family: var(--mono); font-size: 11px; color: var(--muted);
}
.meta .team { color: var(--ink); font-weight: 700; will-change: transform, opacity; }
.meta .members { margin-top: 4px; will-change: transform, opacity; }

.stamp {
  border: 1px solid var(--ink);
  padding: 5px 9px;
  border-radius: 2px;
  font-size: 11px;
  text-transform: uppercase;
  letter-spacing: 0.15em;
  color: var(--ink);
  font-weight: 600;
  font-family: var(--mono);
  will-change: transform, opacity;
}

@media (prefers-reduced-motion: reduce) {
  .rule, .eyebrow::before {
    animation: none;
    transform: scaleX(1);
  }
  .chrome .brand, .chrome .num,
  .eb-text, .wordmark .l, .wordmark .plus, .wordmark .ai-l,
  .tag, .cover-board, .meta .team, .meta .members, .stamp {
    transform: none !important;
    opacity: 1 !important;
  }
}
</style>

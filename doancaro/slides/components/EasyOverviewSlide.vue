<script setup lang="ts">
import { ref } from 'vue'
import { onSlideEnter } from '@slidev/client'

const mountKey = ref(0)
onSlideEnter(() => { mountKey.value++ })
</script>

<template>
  <div class="s-easy" :key="mountKey">
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
        <b>03</b> &middot; Chế độ Easy &middot; tổng quan
      </div>
    </div>

    <div class="eyebrow"
         v-motion
         :initial="{ opacity: 0, x: -6 }"
         :enter="{ opacity: 1, x: 0, transition: { duration: 220, delay: 250 } }">
      Mức 1 &middot; Greedy one-ply
    </div>

    <h2 v-motion
        :initial="{ opacity: 0, y: 18 }"
        :enter="{ opacity: 1, y: 0, transition: { duration: 260, delay: 320 } }">
      Không phải <em>look-ahead</em> &mdash;<br/>chỉ <em>look-around</em>.
    </h2>

    <div class="body-grid">
      <ul class="bullets">
        <li v-motion
            :initial="{ opacity: 0, x: -10 }"
            :enter="{ opacity: 1, x: 0, transition: { duration: 240, delay: 600 } }">
          <span class="num-pill">1</span>
          <div>
            <b>Không có tree search.</b> Không minimax, không α–β, không transposition table &mdash; cũng không có opening book.
          </div>
        </li>
        <li v-motion
            :initial="{ opacity: 0, x: -10 }"
            :enter="{ opacity: 1, x: 0, transition: { duration: 240, delay: 780 } }">
          <span class="num-pill">2</span>
          <div>
            <b>Chỉ tính điểm cục bộ.</b> Với mỗi ô trống, đánh giá các cửa sổ 5 ô đi qua nó theo cả 4 hướng.
          </div>
        </li>
        <li v-motion
            :initial="{ opacity: 0, x: -10 }"
            :enter="{ opacity: 1, x: 0, transition: { duration: 240, delay: 960 } }">
          <span class="num-pill">3</span>
          <div>
            <b>Chọn delta lớn nhất.</b> Cho điểm bàn cờ <em>before</em> và <em>after</em> khi thử đặt một quân &mdash; bước nhảy lớn nhất thắng.
          </div>
        </li>
      </ul>

      <div class="diagram"
           v-motion
           :initial="{ opacity: 0, scale: 0.9 }"
           :enter="{ opacity: 1, scale: 1, transition: { type: 'spring', stiffness: 200, damping: 20, delay: 1150 } }">
        <div class="board">
          <div v-for="i in 81" :key="i" class="cell"
               :class="{
                 'x': i === 41,
                 'ring': [22, 23, 24, 25, 26, 31, 32, 34, 35, 40, 42, 49, 50, 51, 58, 59, 60].includes(i),
                 'farthest': [14, 15, 16, 17, 18, 19, 20, 29, 38, 47, 56, 65, 74, 73, 72, 71, 70, 69, 68, 59].includes(i)
               }">
            <span v-if="i === 41">×</span>
          </div>
        </div>
        <div class="caption">
          Các cửa sổ đi qua quân trung tâm<br/>
          <span class="note">(4 hướng &times; tối đa 5 vị trí = 17 cửa sổ được đánh giá)</span>
        </div>
      </div>
    </div>
  </div>
</template>

<style scoped>
.s-easy {
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
  margin-bottom: 38px;
  line-height: 1.15;
  will-change: transform, opacity;
}
h2 em { color: var(--teal); font-style: normal; }

.body-grid {
  display: grid;
  grid-template-columns: 1.2fr 0.8fr;
  gap: 48px;
  align-items: center;
  flex: 1;
}

.bullets {
  list-style: none;
  padding: 0;
  margin: 0;
  display: flex;
  flex-direction: column;
  gap: 20px;
}
.bullets li {
  display: flex;
  gap: 16px;
  align-items: flex-start;
  font-size: 16px;
  line-height: 1.45;
  color: var(--ink);
  will-change: transform, opacity;
}
.bullets li b { font-weight: 700; }
.bullets li em { color: var(--teal); font-style: normal; font-weight: 700; }

.num-pill {
  display: grid;
  place-items: center;
  flex: 0 0 auto;
  width: 28px; height: 28px;
  border-radius: 50%;
  background: var(--ink);
  color: var(--paper);
  font-family: var(--mono);
  font-size: 13px;
  font-weight: 700;
  margin-top: 1px;
}

.diagram {
  display: flex;
  flex-direction: column;
  align-items: center;
  gap: 12px;
  will-change: transform, opacity;
}

.board {
  display: grid;
  grid-template-columns: repeat(9, 24px);
  grid-template-rows: repeat(9, 24px);
  background: var(--paper-2);
  border: 2px solid var(--ink);
  padding: 6px;
}
.cell {
  border: 1px solid rgba(11,22,24,0.1);
  display: grid;
  place-items: center;
  font-family: var(--mono);
  font-size: 14px;
  font-weight: 800;
  color: var(--coral);
  background: transparent;
}
.cell.ring { background: var(--coral-tint); }
.cell.farthest { background: rgba(255, 227, 219, 0.45); }
.cell.x { background: var(--coral); color: var(--paper); }

.caption {
  font-family: var(--mono);
  font-size: 11px;
  color: var(--muted);
  letter-spacing: 0.03em;
  text-align: center;
  line-height: 1.5;
}
.caption .note { opacity: 0.75; }

@media (prefers-reduced-motion: reduce) {
  .chrome .brand, .chrome .num, .eyebrow, h2,
  .bullets li, .diagram {
    transform: none !important;
    opacity: 1 !important;
  }
}
</style>

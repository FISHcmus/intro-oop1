<script setup lang="ts">
import { ref } from 'vue'
import { onSlideEnter } from '@slidev/client'

const mountKey = ref(0)
onSlideEnter(() => { mountKey.value++ })
</script>

<template>
  <div class="s-tiers" :key="mountKey">
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
        <b>08</b> &middot; Ba mức khó
      </div>
    </div>

    <div class="eyebrow"
         v-motion
         :initial="{ opacity: 0, x: -6 }"
         :enter="{ opacity: 1, x: 0, transition: { duration: 220, delay: 250 } }">
      Easy &middot; Normal &middot; Hard
    </div>

    <h2 v-motion
        :initial="{ opacity: 0, y: 18 }"
        :enter="{ opacity: 1, y: 0, transition: { duration: 260, delay: 320 } }">
      Cùng một engine. <em>Chỉ khác depth.</em>
    </h2>

    <div class="table-wrap"
         v-motion
         :initial="{ opacity: 0, y: 14 }"
         :enter="{ opacity: 1, y: 0, transition: { duration: 280, delay: 480 } }">
      <table class="tiers">
        <thead>
          <tr>
            <th>Mức</th>
            <th><code>aiDepth</code></th>
            <th>Thuật toán</th>
            <th>Cắt tỉa</th>
            <th>TT</th>
            <th>Cảm giác</th>
          </tr>
        </thead>
        <tbody>
          <tr class="row-easy">
            <td><span class="tag easy">Easy</span></td>
            <td><code>1</code></td>
            <td>Greedy one-ply &mdash; chấm <em>delta</em></td>
            <td>&mdash;</td>
            <td>&mdash;</td>
            <td>Phản xạ, nhưng nhìn được đe doạ</td>
          </tr>
          <tr class="row-normal">
            <td><span class="tag normal">Normal</span></td>
            <td><code>2</code></td>
            <td>Minimax &mdash; 2 tầng</td>
            <td>α&ndash;β</td>
            <td>Zobrist + TT</td>
            <td>Tính trước một nước</td>
          </tr>
          <tr class="row-hard">
            <td><span class="tag hard">Hard</span></td>
            <td><code>3</code></td>
            <td>Minimax &mdash; 3 tầng</td>
            <td>α&ndash;β</td>
            <td>Zobrist + TT</td>
            <td>Nhận ra đôi đe doạ</td>
          </tr>
        </tbody>
      </table>
    </div>

    <div class="insights">
      <div class="insight"
           v-motion
           :initial="{ opacity: 0, x: -10 }"
           :enter="{ opacity: 1, x: 0, transition: { duration: 240, delay: 720 } }">
        <div class="kicker">Một đường code duy nhất</div>
        <p>
          Chỉ một hàm <code>AIPlayer::getMove</code> phân nhánh theo <code>searchDepth</code>.
          Phân nhánh cứng duy nhất là <b>depth &lt; 2 → nhánh greedy</b>;
          phần còn lại đi qua cùng một vòng minimax/α&ndash;β/TT.
        </p>
      </div>
      <div class="insight"
           v-motion
           :initial="{ opacity: 0, x: 10 }"
           :enter="{ opacity: 1, x: 0, transition: { duration: 240, delay: 860 } }">
        <div class="kicker">Cố ý không thêm vào</div>
        <p>
          Không iterative deepening, không opening book, không aspiration windows.
          Phạm vi bám sát <em>đồ án sinh viên</em> &mdash; ưu tiên đúng hơn mạnh.
        </p>
      </div>
    </div>
  </div>
</template>

<style scoped>
.s-tiers {
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
  font-size: 34px;
  letter-spacing: -0.025em;
  margin-bottom: 28px;
  will-change: transform, opacity;
}
h2 em { color: var(--teal); font-style: normal; }

.table-wrap {
  will-change: transform, opacity;
  margin-bottom: 22px;
}

table.tiers {
  width: 100%;
  border-collapse: collapse;
  font-size: 13px;
  background: var(--paper-2);
  border: 1px solid var(--line);
  border-radius: 6px;
  overflow: hidden;
}
table.tiers th,
table.tiers td {
  padding: 11px 14px;
  text-align: left;
  border-bottom: 1px solid var(--line);
  vertical-align: middle;
}
table.tiers th {
  font-family: var(--mono);
  font-size: 10px;
  text-transform: uppercase;
  letter-spacing: 0.12em;
  color: var(--muted);
  background: var(--paper);
  font-weight: 700;
}
table.tiers tbody tr:last-child td { border-bottom: none; }
table.tiers code {
  font-family: var(--mono);
  font-size: 12px;
  background: var(--paper);
  padding: 1px 6px;
  border-radius: 3px;
  border: 1px solid var(--line);
  font-weight: 700;
}
table.tiers em { color: var(--coral); font-style: normal; font-weight: 700; }

.tag {
  display: inline-block;
  padding: 3px 10px;
  border-radius: 10px;
  font-family: var(--mono);
  font-size: 11px;
  font-weight: 700;
  letter-spacing: 0.06em;
  text-transform: uppercase;
  color: var(--paper);
}
.tag.easy { background: var(--teal); }
.tag.normal { background: var(--gold); color: var(--ink); }
.tag.hard { background: var(--coral); }

.insights {
  display: grid;
  grid-template-columns: 1fr 1fr;
  gap: 24px;
}
.insight {
  will-change: transform, opacity;
}
.kicker {
  font-family: var(--mono);
  font-size: 10px;
  text-transform: uppercase;
  letter-spacing: 0.12em;
  color: var(--coral);
  margin-bottom: 4px;
}
.insight p {
  font-size: 12.5px;
  line-height: 1.5;
  margin: 0;
  color: var(--ink);
}
.insight p b { font-weight: 700; }
.insight p em { color: var(--teal); font-style: normal; font-weight: 700; }
.insight p code {
  font-family: var(--mono);
  font-size: 11px;
  background: var(--paper-2);
  padding: 1px 5px;
  border-radius: 3px;
}

@media (prefers-reduced-motion: reduce) {
  .chrome .brand, .chrome .num, .eyebrow, h2, .table-wrap, .insight {
    transform: none !important;
    opacity: 1 !important;
  }
}
</style>

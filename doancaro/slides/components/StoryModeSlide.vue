<script setup lang="ts">
import { ref } from 'vue'
import { onSlideEnter } from '@slidev/client'

const mountKey = ref(0)
onSlideEnter(() => { mountKey.value++ })
</script>

<template>
  <div class="s-story" :key="mountKey">
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
        <b>09</b> &middot; Chế độ cốt truyện
      </div>
    </div>

    <div class="eyebrow"
         v-motion
         :initial="{ opacity: 0, x: -6 }"
         :enter="{ opacity: 1, x: 0, transition: { duration: 220, delay: 250 } }">
      Cô Sử Tiên &middot; 4 set &middot; linh-vật &middot; trùm cuối
    </div>

    <h2 v-motion
        :initial="{ opacity: 0, y: 18 }"
        :enter="{ opacity: 1, y: 0, transition: { duration: 260, delay: 320 } }">
      Bốn tầng AI thành một <em>chiến dịch</em>.
    </h2>

    <div class="flow"
         v-motion
         :initial="{ opacity: 0, x: -6 }"
         :enter="{ opacity: 1, x: 0, transition: { duration: 220, delay: 420 } }">
      <code>StoryPickSet</code> &rarr; <code>StoryIntro</code> &rarr; <code>Playing</code> (best-of-3) &rarr; <code>StoryBeat</code>
    </div>

    <div class="table-wrap"
         v-motion
         :initial="{ opacity: 0, y: 14 }"
         :enter="{ opacity: 1, y: 0, transition: { duration: 280, delay: 520 } }">
      <table class="tiers">
        <thead>
          <tr>
            <th>Set</th>
            <th>Độ khó</th>
            <th><code>aiDepth</code></th>
            <th>Linh-vật mở khóa</th>
          </tr>
        </thead>
        <tbody>
          <tr>
            <td><b>1</b></td>
            <td><span class="tag easy">Dễ</span></td>
            <td><code>1</code></td>
            <td>&mdash;</td>
          </tr>
          <tr>
            <td><b>2</b></td>
            <td><span class="tag normal">Vừa</span></td>
            <td><code>2</code></td>
            <td>Voi 9 ngà &mdash; <em>undo &times;1</em></td>
          </tr>
          <tr>
            <td><b>3</b></td>
            <td><span class="tag hard">Khó</span></td>
            <td><code>3</code></td>
            <td>Gà 9 cựa &mdash; <em>ép AI đánh ngẫu nhiên &times;3</em></td>
          </tr>
          <tr>
            <td><b>4</b></td>
            <td><span class="tag boss">Trùm</span></td>
            <td><code>4</code></td>
            <td>Ngựa 9 hồng mao &mdash; <em>tự hồi sinh</em></td>
          </tr>
        </tbody>
      </table>
    </div>

    <div class="insights">
      <div class="insight"
           v-motion
           :initial="{ opacity: 0, x: -10 }"
           :enter="{ opacity: 1, x: 0, transition: { duration: 240, delay: 760 } }">
        <div class="kicker">Tái dùng, không viết lại</div>
        <p>
          Mỗi set chỉ đổi <code>aiDepth</code> của <code>AIPlayer</code> đã có.
          Ba linh-vật ánh xạ vào cơ chế sẵn: <b>undo</b> (Voi), <b>ép AI ngẫu nhiên</b> (Gà),
          <b>tự hồi sinh</b> (Ngựa). Charges reset đầu mỗi set, lưu trong save <em>v4</em>.
        </p>
      </div>
      <div class="insight"
           v-motion
           :initial="{ opacity: 0, x: 10 }"
           :enter="{ opacity: 1, x: 0, transition: { duration: 240, delay: 900 } }">
        <div class="kicker">Trùm cuối, vẫn cân bằng</div>
        <p>
          Depth 4 chạy trên <b>luồng riêng</b> &mdash; không treo UI. Cứ sau mỗi 4 nước
          của người chơi, trùm &ldquo;ăn&rdquo; tối đa 4 quân gần nhất. Thể thức
          <em>best-of-3</em> cộng Ngựa hồi sinh giữ trận khó nhưng <b>thắng được</b>.
        </p>
      </div>
    </div>
  </div>
</template>

<style scoped>
.s-story {
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
  margin-bottom: 14px;
  will-change: transform, opacity;
}
h2 em { color: var(--teal); font-style: normal; }

.flow {
  font-family: var(--mono);
  font-size: 11.5px;
  color: var(--muted);
  margin-bottom: 22px;
  will-change: transform, opacity;
}
.flow code {
  font-family: var(--mono);
  background: var(--paper-2);
  padding: 1px 6px;
  border-radius: 3px;
  color: var(--ink);
  font-weight: 700;
}

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
table.tiers td b { font-weight: 800; }

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
.tag.boss { background: var(--ink); }

.insights {
  display: grid;
  grid-template-columns: 1fr 1fr;
  gap: 24px;
}
.insight { will-change: transform, opacity; }
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
  .chrome .brand, .chrome .num, .eyebrow, h2, .flow, .table-wrap, .insight {
    transform: none !important;
    opacity: 1 !important;
  }
}
</style>

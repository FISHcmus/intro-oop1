<script setup lang="ts">
import { ref } from 'vue'
import { onSlideEnter } from '@slidev/client'

const mountKey = ref(0)
onSlideEnter(() => { mountKey.value++ })
</script>

<template>
  <div class="s-ab" :key="mountKey">
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
        <b>06</b> &middot; Minimax &middot; cắt tỉa α&ndash;β
      </div>
    </div>

    <div class="eyebrow"
         v-motion
         :initial="{ opacity: 0, x: -6 }"
         :enter="{ opacity: 1, x: 0, transition: { duration: 220, delay: 250 } }">
      Cùng kết quả &middot; Nhanh hơn nhiều lần
    </div>

    <h2 v-motion
        :initial="{ opacity: 0, y: 18 }"
        :enter="{ opacity: 1, y: 0, transition: { duration: 260, delay: 320 } }">
      Cắt các nhánh <em>không thể cải thiện</em> kết quả.
    </h2>

    <div class="body-grid">
      <div class="code-wrap"
           v-motion
           :initial="{ opacity: 0, y: 16 }"
           :enter="{ opacity: 1, y: 0, transition: { duration: 280, delay: 480 } }">
        <div class="pc-label">Giả mã &mdash; C++</div>
        <pre><code><span class="ty">int</span> <span class="fn">minimax</span>(Board&amp; b, <span class="ty">int</span> depth,
            <span class="ty">int</span> α, <span class="ty">int</span> β, <span class="ty">bool</span> maxPlayer) {
    <span class="kw">if</span> (depth == 0 || b.<span class="fn">isTerminal</span>())
        <span class="kw">return</span> <span class="fn">evaluate</span>(b);

    <span class="kw">for</span> (Move m : <span class="fn">candidates</span>(b)) {
        b.<span class="fn">play</span>(m);
        <span class="ty">int</span> v = <span class="fn">minimax</span>(b, depth&minus;1, α, β, !maxPlayer);
        b.<span class="fn">undo</span>(m);

        <span class="kw">if</span> (maxPlayer) α = <span class="fn">max</span>(α, v);
        <span class="kw">else</span>          β = <span class="fn">min</span>(β, v);

        <span class="kw">if</span> (α &gt;= β) <span class="kw">break</span>;  <span class="cm">// cắt nhánh</span>
    }
    <span class="kw">return</span> maxPlayer ? α : β;
}</code></pre>
      </div>

      <ul class="notes">
        <li v-motion
            :initial="{ opacity: 0, x: 10 }"
            :enter="{ opacity: 1, x: 0, transition: { duration: 240, delay: 680 } }">
          <span class="pill">α</span>
          <div><b>Tốt nhất hiện thời của MAX.</b> Nếu một nút MIN tìm được giá trị &leq; α, nút MAX cha sẽ không bao giờ chọn nhánh này &mdash; dừng duyệt.</div>
        </li>
        <li v-motion
            :initial="{ opacity: 0, x: 10 }"
            :enter="{ opacity: 1, x: 0, transition: { duration: 240, delay: 820 } }">
          <span class="pill coral">β</span>
          <div><b>Tốt nhất hiện thời của MIN.</b> Đối xứng: nếu nút MAX vượt β, nút MIN cha sẽ né ta.</div>
        </li>
        <li v-motion
            :initial="{ opacity: 0, x: 10 }"
            :enter="{ opacity: 1, x: 0, transition: { duration: 240, delay: 960 } }">
          <span class="pill gold">10&ndash;100&times;</span>
          <div>Tăng tốc điển hình khi <em>sắp xếp nước đi tốt</em> — thử nước mạnh nhất trước để cắt xảy ra sớm hơn.</div>
        </li>
        <li v-motion
            :initial="{ opacity: 0, x: 10 }"
            :enter="{ opacity: 1, x: 0, transition: { duration: 240, delay: 1100 } }">
          <span class="pill mute">=</span>
          <div>Kết quả <em>hệt nguyên</em> như minimax thuần &mdash; ta chỉ bỏ qua các nhánh chắc chắn không ảnh hưởng.</div>
        </li>
      </ul>
    </div>
  </div>
</template>

<style scoped>
.s-ab {
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
  margin-bottom: 30px;
  will-change: transform, opacity;
}
h2 em { color: var(--coral); font-style: normal; }

.body-grid {
  display: grid;
  grid-template-columns: 1.1fr 0.9fr;
  gap: 40px;
  flex: 1;
}

.code-wrap {
  will-change: transform, opacity;
}
.pc-label {
  font-family: var(--mono);
  font-size: 10px;
  text-transform: uppercase;
  letter-spacing: 0.12em;
  color: var(--muted);
  margin-bottom: 6px;
}
.code-wrap pre {
  background: var(--ink);
  color: var(--paper);
  padding: 18px 22px;
  border-radius: 6px;
  font-family: var(--mono);
  font-size: 13px;
  line-height: 1.6;
  margin: 0;
  white-space: pre-wrap;
  word-break: break-word;
}
.code-wrap .cm { color: rgba(250,250,247,0.45); }
.code-wrap .kw { color: var(--gold); }
.code-wrap .ty { color: var(--coral); }
.code-wrap .fn { color: var(--teal-tint); }

.notes {
  list-style: none;
  padding: 0;
  margin: 0;
  display: flex;
  flex-direction: column;
  gap: 14px;
}
.notes li {
  font-size: 13px;
  line-height: 1.45;
  color: var(--ink);
  display: flex;
  gap: 10px;
  align-items: flex-start;
  will-change: transform, opacity;
}
.notes b { font-weight: 700; }
.notes em { color: var(--teal); font-style: normal; font-weight: 700; }

.pill {
  flex: 0 0 auto;
  display: grid;
  place-items: center;
  min-width: 36px;
  height: 22px;
  padding: 0 8px;
  border-radius: 11px;
  font-family: var(--mono);
  font-size: 12px;
  font-weight: 700;
  background: var(--teal);
  color: var(--paper);
  margin-top: 1px;
}
.pill.coral { background: var(--coral); }
.pill.gold { background: var(--gold); color: var(--ink); }
.pill.mute { background: var(--muted); }

@media (prefers-reduced-motion: reduce) {
  .chrome .brand, .chrome .num, .eyebrow, h2, .code-wrap, .notes li {
    transform: none !important;
    opacity: 1 !important;
  }
}
</style>

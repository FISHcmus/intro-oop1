<script setup lang="ts">
import { ref } from 'vue'
import { onSlideEnter } from '@slidev/client'

const mountKey = ref(0)
onSlideEnter(() => { mountKey.value++ })
</script>

<template>
  <div class="s-delta" :key="mountKey">
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
        <b>04</b> &middot; Easy &middot; chấm điểm delta
      </div>
    </div>

    <div class="eyebrow"
         v-motion
         :initial="{ opacity: 0, x: -6 }"
         :enter="{ opacity: 1, x: 0, transition: { duration: 220, delay: 250 } }">
      Vì sao dùng delta, không dùng điểm tuyệt đối
    </div>

    <h2 v-motion
        :initial="{ opacity: 0, y: 18 }"
        :enter="{ opacity: 1, y: 0, transition: { duration: 260, delay: 320 } }">
      <em>delta</em> = score(after) &minus; score(before)
    </h2>

    <div class="body-grid">
      <div class="pseudocode"
           v-motion
           :initial="{ opacity: 0, y: 16 }"
           :enter="{ opacity: 1, y: 0, transition: { duration: 280, delay: 520 } }">
        <div class="pc-label">Giả mã &mdash; greedy one-ply</div>
        <pre><code><span class="cm">// Xếp hạng theo mức cải thiện thế cờ</span>
<span class="cm">// sau khi thử đặt quân của ta.</span>
<span class="kw">for</span> each empty cell (r, c):
    before = <span class="fn">localScore</span>(board, r, c)     <span class="cm">// mẫu 4 hướng</span>
    board.<span class="fn">place</span>(r, c, AI)
    after  = <span class="fn">localScore</span>(board, r, c)
    board.<span class="fn">undo</span>(r, c)
    delta  = after &minus; before

<span class="kw">pick</span> argmax(delta)</code></pre>
      </div>

      <div class="insight"
           v-motion
           :initial="{ opacity: 0, x: 12 }"
           :enter="{ opacity: 1, x: 0, transition: { duration: 280, delay: 720 } }">
        <div class="kicker">Vì sao không chỉ dùng |after|?</div>
        <p>
          Khi ta <b>chặn một đe doạ của đối thủ</b>, các cửa sổ vốn có điểm cao
          sẽ sụt về gần 0 &mdash; cả hai bên đều có quân trong đó.
          Nên chỉ dùng |after| sẽ <b>chấm thấp nước chặn</b>.
        </p>
        <p>
          <em>delta</em> ghi nhận sự thay đổi. Một nước chặn mạnh tạo ra delta
          dương lớn vì <em>before</em> rất âm (đe doạ của đối thủ).
        </p>
        <div class="examples">
          <div class="ex">
            <div class="ex-label">Đe doạ mình 3→4</div>
            <div class="ex-before">before: +1000</div>
            <div class="ex-after">after: +15000</div>
            <div class="ex-delta">Δ = +14000</div>
          </div>
          <div class="ex">
            <div class="ex-label">Chặn open-4 đối thủ</div>
            <div class="ex-before">before: &minus;22500</div>
            <div class="ex-after">after: 0</div>
            <div class="ex-delta win">Δ = +22500</div>
          </div>
        </div>
      </div>
    </div>
  </div>
</template>

<style scoped>
.s-delta {
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
  font-size: 36px;
  letter-spacing: -0.03em;
  margin-bottom: 34px;
  font-family: var(--mono);
  font-weight: 700;
  will-change: transform, opacity;
}
h2 em { color: var(--coral); font-style: normal; font-family: var(--sans); }

.body-grid {
  display: grid;
  grid-template-columns: 1.05fr 0.95fr;
  gap: 40px;
  flex: 1;
}

.pseudocode {
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
.pseudocode pre {
  background: var(--ink);
  color: var(--paper);
  padding: 18px 22px;
  border-radius: 6px;
  font-family: var(--mono);
  font-size: 14px;
  line-height: 1.65;
  margin: 0;
  white-space: pre-wrap;
  word-break: break-word;
}
.pseudocode .cm { color: rgba(250,250,247,0.45); }
.pseudocode .kw { color: var(--gold); }
.pseudocode .fn { color: var(--teal-tint); }

.insight {
  display: flex;
  flex-direction: column;
  gap: 14px;
  will-change: transform, opacity;
}
.kicker {
  font-family: var(--mono);
  font-size: 10px;
  text-transform: uppercase;
  letter-spacing: 0.12em;
  color: var(--coral);
}
.insight p {
  font-size: 14px;
  line-height: 1.5;
  margin: 0;
  color: var(--ink);
}
.insight p b { font-weight: 700; }
.insight p em { color: var(--teal); font-style: normal; font-weight: 700; }

.examples {
  display: grid;
  grid-template-columns: 1fr 1fr;
  gap: 10px;
  margin-top: 8px;
}
.ex {
  border: 1px solid var(--line);
  border-radius: 4px;
  padding: 10px 12px;
  font-family: var(--mono);
  font-size: 11px;
  line-height: 1.55;
  background: var(--paper-2);
}
.ex-label {
  font-family: var(--sans);
  font-weight: 700;
  font-size: 11px;
  color: var(--ink);
  margin-bottom: 4px;
  text-transform: uppercase;
  letter-spacing: 0.08em;
}
.ex-before, .ex-after { color: var(--muted); }
.ex-delta {
  margin-top: 4px;
  color: var(--teal);
  font-weight: 700;
  border-top: 1px solid var(--line);
  padding-top: 4px;
}
.ex-delta.win { color: var(--coral); }

@media (prefers-reduced-motion: reduce) {
  .chrome .brand, .chrome .num, .eyebrow, h2, .pseudocode, .insight {
    transform: none !important;
    opacity: 1 !important;
  }
}
</style>

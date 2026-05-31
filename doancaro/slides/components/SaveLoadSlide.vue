<script setup lang="ts">
import { ref } from 'vue'
import { onSlideEnter } from '@slidev/client'

const mountKey = ref(0)
onSlideEnter(() => { mountKey.value++ })
</script>

<template>
  <div class="s-save" :key="mountKey">
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
        <b>11</b> &middot; Lưu &middot; Tải &middot; Thiết lập
      </div>
    </div>

    <div class="eyebrow"
         v-motion
         :initial="{ opacity: 0, x: -6 }"
         :enter="{ opacity: 1, x: 0, transition: { duration: 220, delay: 250 } }">
      Slot nhị phân &middot; magic + CRC32
    </div>

    <h2 v-motion
        :initial="{ opacity: 0, y: 18 }"
        :enter="{ opacity: 1, y: 0, transition: { duration: 260, delay: 320 } }">
      Bốn slot, kiểm tra mỗi lần <em>tải</em>.
    </h2>

    <div class="body-grid">
      <div class="format"
           v-motion
           :initial="{ opacity: 0, y: 14 }"
           :enter="{ opacity: 1, y: 0, transition: { duration: 280, delay: 480 } }">
        <div class="pc-label">Bố cục file save &mdash; <code>slot{0..3}.caro</code></div>
<pre class="layout"><code><span class="cm">// SaveHeader (kích thước cố định, little-endian)</span>
<span class="ty">uint32_t</span>  magic        = <span class="lit">0x4341524F</span>   <span class="cm">// "CARO"</span>
<span class="ty">uint16_t</span>  version      = <span class="lit">4</span>            <span class="cm">// aiDepth 1/2/3 + khối Story Mode</span>
<span class="ty">uint32_t</span>  <span class="hi">checksum</span>     <span class="cm">// CRC32 phủ mọi byte sau trường này</span>
<span class="ty">int64_t</span>   timestamp
<span class="ty">float</span>     playTime
<span class="ty">int</span>       moveCount, gameMode, aiDepth, currentTurn
<span class="ty">int</span>       p1Wins, p2Wins, p1Moves, p2Moves
<span class="ty">char</span>      p1Name[<span class="lit">32</span>], p2Name[<span class="lit">32</span>]
<span class="ty">int</span>       <span class="hi">storySetId</span>, storyMatchWins/Losses   <span class="cm">// v4</span>
<span class="ty">int</span>       story{Voi,Ga,Ngua}Charges          <span class="cm">// v4</span>

<span class="cm">// Thân</span>
<span class="ty">int</span>       cells[<span class="lit">15</span>][<span class="lit">15</span>]      <span class="cm">// toàn bộ trạng thái bàn</span>
<span class="ty">int</span>       lastMoveRow, lastMoveCol</code></pre>
        <div class="slot-row">
          <div class="slot auto"><div class="sl-label">slot 0</div><div class="sl-tag">tự động</div></div>
          <div class="slot"><div class="sl-label">slot 1</div><div class="sl-tag">thủ công</div></div>
          <div class="slot"><div class="sl-label">slot 2</div><div class="sl-tag">thủ công</div></div>
          <div class="slot"><div class="sl-label">slot 3</div><div class="sl-tag">thủ công</div></div>
        </div>
      </div>

      <div class="notes-col">
        <div class="note-card"
             v-motion
             :initial="{ opacity: 0, x: 10 }"
             :enter="{ opacity: 1, x: 0, transition: { duration: 240, delay: 700 } }">
          <div class="kicker">LoadResult</div>
          <ul class="enum">
            <li><code>OK</code> &mdash; magic + version + CRC32 đều khớp</li>
            <li><code>NotFound</code> &mdash; file không tồn tại</li>
            <li><code>Corrupt</code> &mdash; sai magic hoặc CRC lệch</li>
            <li><code>VersionMismatch</code> &mdash; file của bản build cũ</li>
          </ul>
        </div>

        <div class="note-card"
             v-motion
             :initial="{ opacity: 0, x: 10 }"
             :enter="{ opacity: 1, x: 0, transition: { duration: 240, delay: 860 } }">
          <div class="kicker">settings.cfg &mdash; text, 2 ints</div>
<pre class="cfg"><code>fprintf(f, <span class="str">"%d %d\n"</span>, vsAI ? 1 : 0, aiDepth);
<span class="cm">// ví dụ  "1 3\n"  → bật đấu AI, mức Hard</span></code></pre>
          <p class="tiny">Các giá trị <code>aiDepth</code> cũ (2, 4, 6, 8) được ánh xạ về Hard (3) khi tải.</p>
        </div>
      </div>
    </div>
  </div>
</template>

<style scoped>
.s-save {
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
  font-size: 32px;
  letter-spacing: -0.025em;
  margin-bottom: 24px;
  will-change: transform, opacity;
}
h2 em { color: var(--coral); font-style: normal; }

.body-grid {
  display: grid;
  grid-template-columns: 1.2fr 0.8fr;
  gap: 28px;
  flex: 1;
}

.pc-label {
  font-family: var(--mono);
  font-size: 10px;
  text-transform: uppercase;
  letter-spacing: 0.12em;
  color: var(--muted);
  margin-bottom: 6px;
}
.pc-label code {
  font-family: var(--mono);
  background: var(--paper-2);
  padding: 1px 5px;
  border-radius: 3px;
  text-transform: none;
  letter-spacing: 0;
  color: var(--ink);
}

.format {
  will-change: transform, opacity;
  display: flex;
  flex-direction: column;
}

pre.layout {
  background: var(--ink);
  color: var(--paper);
  padding: 14px 18px;
  border-radius: 6px;
  font-family: var(--mono);
  font-size: 11.5px;
  line-height: 1.55;
  margin: 0 0 14px;
  white-space: pre-wrap;
  word-break: break-word;
}
pre.layout .cm { color: rgba(250,250,247,0.42); }
pre.layout .ty { color: var(--coral); }
pre.layout .lit { color: var(--gold); }
pre.layout .hi { color: var(--teal-tint); font-weight: 700; }

.slot-row {
  display: grid;
  grid-template-columns: repeat(4, 1fr);
  gap: 8px;
}
.slot {
  border: 1px solid var(--line);
  border-radius: 4px;
  padding: 8px 10px;
  background: var(--paper-2);
  text-align: center;
}
.slot.auto {
  border-color: var(--teal);
  background: var(--teal-tint);
}
.sl-label {
  font-family: var(--mono);
  font-size: 11px;
  font-weight: 700;
  color: var(--ink);
}
.sl-tag {
  font-family: var(--mono);
  font-size: 9px;
  letter-spacing: 0.1em;
  text-transform: uppercase;
  color: var(--muted);
  margin-top: 2px;
}

.notes-col {
  display: flex;
  flex-direction: column;
  gap: 14px;
}
.note-card {
  border: 1px solid var(--line);
  border-radius: 6px;
  padding: 12px 14px;
  background: var(--paper-2);
  will-change: transform, opacity;
}
.kicker {
  font-family: var(--mono);
  font-size: 10px;
  text-transform: uppercase;
  letter-spacing: 0.12em;
  color: var(--coral);
  margin-bottom: 8px;
}

ul.enum {
  list-style: none;
  padding: 0;
  margin: 0;
  display: flex;
  flex-direction: column;
  gap: 4px;
}
ul.enum li {
  font-size: 12px;
  line-height: 1.4;
  color: var(--ink);
}
ul.enum code {
  font-family: var(--mono);
  font-size: 11px;
  background: var(--paper);
  padding: 1px 5px;
  border-radius: 3px;
  border: 1px solid var(--line);
  font-weight: 700;
  color: var(--teal);
}

pre.cfg {
  background: var(--ink);
  color: var(--paper);
  padding: 10px 12px;
  border-radius: 4px;
  font-family: var(--mono);
  font-size: 11px;
  line-height: 1.5;
  margin: 0 0 6px;
  white-space: pre-wrap;
  word-break: break-word;
}
pre.cfg .cm { color: rgba(250,250,247,0.42); }
pre.cfg .str { color: var(--gold); }

.tiny {
  font-size: 11px;
  color: var(--muted);
  margin: 0;
  line-height: 1.45;
}

@media (prefers-reduced-motion: reduce) {
  .chrome .brand, .chrome .num, .eyebrow, h2, .format, .note-card {
    transform: none !important;
    opacity: 1 !important;
  }
}
</style>

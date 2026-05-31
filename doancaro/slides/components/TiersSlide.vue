<script setup lang="ts">
import { ref } from 'vue'
import { onSlideEnter } from '@slidev/client'
import { slideBackgrounds } from './slideBackgrounds'

const mountKey = ref(0)
const bg = slideBackgrounds.tiers
onSlideEnter(() => { mountKey.value++ })
</script>

<template>
  <div class="s-tiers scene-slide" :key="mountKey">
    <div class="scene-bg cool">
      <img :src="bg" alt="Difficulty selection background" />
    </div>

    <div class="scene-shell">
      <div class="chrome dark">
        <div class="brand"
             v-motion
             :initial="{ opacity: 0, scale: 1.12, rotate: -2 }"
             :enter="{ opacity: 1, scale: 1, rotate: 0, transition: { duration: 200, delay: 100 } }">
          <span class="dot"></span>CARO
        </div>
        <div class="num"
             v-motion
             :initial="{ opacity: 0, scale: 1.12, rotate: 2 }"
             :enter="{ opacity: 1, scale: 1, rotate: 0, transition: { duration: 200, delay: 180 } }">
          <b>06</b> &middot; Easy &middot; heuristic
        </div>
      </div>

      <div class="glass-panel tiers-panel"
           v-motion
           :initial="{ opacity: 0, y: 24 }"
           :enter="{ opacity: 1, y: 0, transition: { duration: 320, delay: 320 } }">
        <div class="eyebrow on-image">Heuristic &middot; delta score</div>
        <h2 class="scene-title">Easy <em>chấm nhanh</em> và chặn sớm.</h2>

        <table class="tiers-table">
          <thead>
            <tr>
              <th>Mẫu</th>
              <th>Ý nghĩa</th>
              <th>Ưu tiên</th>
            </tr>
          </thead>
          <tbody>
            <tr>
              <td><code>XXXXX</code></td>
              <td>thắng ngay</td>
              <td>rất cao</td>
            </tr>
            <tr>
              <td><code>.XXX.</code></td>
              <td>thế mở mạnh</td>
              <td>cao</td>
            </tr>
            <tr>
              <td><code>OOOO.</code></td>
              <td>nguy cơ thua ngay</td>
              <td>chặn trước</td>
            </tr>
            <tr>
              <td><code>XX...</code></td>
              <td>thế yếu</td>
              <td>thấp</td>
            </tr>
          </tbody>
        </table>

        <p class="tiers-note">
          Pseudocode: <code>delta = score(after move) - score(now)</code> &middot; ưu tiên chặn nguy hiểm trước khi tấn công yếu.
        </p>
      </div>
    </div>
  </div>
</template>

<style scoped>
.tiers-panel {
  margin-top: 12px;
  margin-bottom: 18px;
  margin-left: auto;
  width: min(900px, 86%);
  padding: 20px 22px 20px;
}

.tiers-panel .scene-title {
  font-size: 48px;
  margin-bottom: 12px;
}

.tiers-table {
  width: 100%;
  border-collapse: collapse;
  overflow: hidden;
  border-radius: 22px;
  background: rgba(255, 251, 242, 0.54);
  border: 1px solid rgba(11, 22, 24, 0.10);
}

.tiers-table th,
.tiers-table td {
  padding: 10px 12px;
  text-align: left;
  border-bottom: 1px solid rgba(11, 22, 24, 0.08);
  color: var(--ink);
}

.tiers-table th {
  font-family: var(--mono);
  font-size: 10px;
  text-transform: uppercase;
  letter-spacing: 0.12em;
  color: var(--muted);
}

.tiers-table tbody tr:last-child td {
  border-bottom: none;
}

.tiers-table code {
  font-family: var(--mono);
  font-size: 12px;
}

.tiers-note {
  margin: 12px 0 0;
  font-size: 13px;
  line-height: 1.35;
  color: var(--ink-2);
}

.tiers-note code,
.tiers-table code {
  font-family: var(--mono);
}

@media (prefers-reduced-motion: reduce) {
  .chrome .brand, .chrome .num, .tiers-panel {
    transform: none !important;
    opacity: 1 !important;
  }
}
</style>

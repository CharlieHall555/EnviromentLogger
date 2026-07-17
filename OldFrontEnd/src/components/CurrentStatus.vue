<script setup lang="ts">
import { computed, onMounted, onUnmounted, ref } from "vue";
import {
  fetchLatestReading,
  type SensorReading,
} from "../api/latestReading";
import {
  formatMetric,
  getPM1Class,
  getPM10Class,
  getPM25Class,
  getTemperatureClass,
} from "../utils/metricHelpers";

const latestReading = ref<SensorReading | null>(null);
const isLoading = ref(false);
const errorMessage = ref("");
let refreshTimer: ReturnType<typeof setInterval> | null = null;

function formatTimestamp(value: string | undefined): string {
  if (!value) {
    return "-";
  }

  const parsed = new Date(value);
  if (Number.isNaN(parsed.getTime())) {
    return value;
  }

  return parsed.toLocaleString();
}

const measuredAtText = computed(() =>
  formatTimestamp(latestReading.value?.measured_at)
);
const receivedAtText = computed(() =>
  formatTimestamp(latestReading.value?.received_at)
);

async function loadLatestReading() {
  isLoading.value = true;
  errorMessage.value = "";

  try {
    latestReading.value = await fetchLatestReading();
    if (!latestReading.value) {
      errorMessage.value = "No readings available yet.";
    }
  } catch {
    latestReading.value = null;
    errorMessage.value = "Failed to load current status.";
  } finally {
    isLoading.value = false;
  }
}

onMounted(() => {
  loadLatestReading();
  refreshTimer = setInterval(loadLatestReading, 5 * 60 * 1000);
});

onUnmounted(() => {
  if (refreshTimer !== null) {
    clearInterval(refreshTimer);
  }
});
</script>

<template>
  <section class="status-card">
    <header class="status-header">
      <h2>Current Status</h2>
    </header>

    <p v-if="errorMessage" class="error">{{ errorMessage }}</p>

    <div v-else-if="latestReading" class="grid">
      <article class="metric">
        <span class="label">Temperature</span>
        <strong
          class="data_value"
          :class="getTemperatureClass(latestReading.temperature)"
        >
          {{ formatMetric(latestReading.temperature, "C") }}
        </strong>
      </article>


      <article class="metric">
        <span class="label">PM10</span>
        <strong
          class="data_value"
          :class="getPM10Class(latestReading.pm10)"
        >
          {{ formatMetric(latestReading.pm10, "μg/m") }}
        </strong>
      </article>

      <article class="metric">
        <span class="label">PM1.0</span>
        <strong
          class="data_value"
          :class="getPM1Class(latestReading.pm1_0)"
        >
          {{ formatMetric(latestReading.pm1_0, "μg/m") }}
        </strong>
      </article>

      <article class="metric">
        <span class="label">PM2.5</span>
        <strong
          class="data_value"
          :class="getPM25Class(latestReading.pm2_5)"
        >
          {{ formatMetric(latestReading.pm2_5, "μg/m") }}
        </strong>
      </article>
    
      <article class="metric">
        <span class="label">Humidity</span>
        <strong>{{ formatMetric(latestReading.humidity, "%") }}</strong>
      </article>

      <article class="meta">
        <span class="label">Measured At</span>
        <strong>{{ measuredAtText }}</strong>
      </article>
    </div>
  </section>
</template>

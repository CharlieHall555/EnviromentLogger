<script setup lang="ts">
import { computed, onMounted, onUnmounted, ref } from "vue";
import {
  fetchLatestReading,
  type SensorReading,
} from "../api/latestReading";

const latestReading = ref<SensorReading | null>(null);
const isLoading = ref(false);
const errorMessage = ref("");
let refreshTimer: ReturnType<typeof setInterval> | null = null;

function toNumeric(value: number | string | null): number | null {
  if (value === null || value === "") {
    return null;
  }

  const parsed = typeof value === "number" ? value : Number(value);
  return Number.isFinite(parsed) ? parsed : null;
}

function getTemperatureClass(value: number | string | null): string {
  const numericValue = toNumeric(value);
  if (numericValue === null) {
    return "";
  }

  if (numericValue < 16) {
    return "temp-cool";
  }

  if (numericValue < 24) {
    return "temp-mild";
  }

  if (numericValue < 30) {
    return "temp-warm";
  }

  return "temp-hot";
}

function formatMetric(
  value: number | string | null,
  unit: string,
  fractionDigits = 1
): string {
  const numericValue = toNumeric(value);
  if (numericValue === null) {
    return "-";
  }

  return `${numericValue.toFixed(fractionDigits)} ${unit}`;
}

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
          class="temp-value"
          :class="getTemperatureClass(latestReading.temperature)"
        >
          {{ formatMetric(latestReading.temperature, "C") }}
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

      <article class="meta">
        <span class="label">Received At</span>
        <strong>{{ receivedAtText }}</strong>
      </article>
    </div>
  </section>
</template>

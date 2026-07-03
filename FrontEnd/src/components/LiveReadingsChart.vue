<script setup lang="ts">
import { computed, onMounted, onUnmounted, ref } from "vue";
import {
  CategoryScale,
  Chart as ChartJS,
  Legend,
  LineElement,
  LinearScale,
  PointElement,
  Title,
  Tooltip,
  type ChartData,
  type ChartOptions,
} from "chart.js";
import { Line } from "vue-chartjs";
import {
  fetchReadingsPage,
  type SensorReading,
} from "../api/latestReading";

ChartJS.register(
  Title,
  Tooltip,
  Legend,
  CategoryScale,
  LinearScale,
  PointElement,
  LineElement
);

const pageSize = 10;
const currentPage = ref(1);
const readings = ref<SensorReading[]>([]);
const isLoading = ref(false);
const errorMessage = ref("");
let refreshTimer: ReturnType<typeof setInterval> | null = null;

const chartReadings = computed(() => [...readings.value].reverse());

function toNumeric(value: number | string | null): number | null {
  if (value === null || value === "") {
    return null;
  }

  const parsed = typeof value === "number" ? value : Number(value);
  return Number.isFinite(parsed) ? parsed : null;
}

function formatLabel(dateString: string): string {
  const parsed = new Date(dateString);
  if (Number.isNaN(parsed.getTime())) {
    return dateString;
  }

  return parsed.toLocaleTimeString([], {
    hour: "2-digit",
    minute: "2-digit",
  });
}

const chartData = computed<ChartData<"line">>(() => ({
  labels: chartReadings.value.map((reading) => formatLabel(reading.measured_at)),
  datasets: [
    {
      label: "Humidity (%)",
      data: chartReadings.value.map((reading) => toNumeric(reading.humidity)),
      borderColor: "#0284c7",
      backgroundColor: "rgba(2, 132, 199, 0.2)",
      pointBackgroundColor: "#0284c7",
      tension: 0.32,
      spanGaps: true,
      yAxisID: "yHumidity",
    },
    {
      label: "Temperature (C)",
      data: chartReadings.value.map((reading) => toNumeric(reading.temperature)),
      borderColor: "#f97316",
      backgroundColor: "rgba(249, 115, 22, 0.2)",
      pointBackgroundColor: "#f97316",
      tension: 0.32,
      spanGaps: true,
      yAxisID: "yTemperature",
    },
  ],
}));

const chartOptions = computed<ChartOptions<"line">>(() => ({
  responsive: true,
  maintainAspectRatio: false,
  interaction: {
    mode: "index",
    intersect: false,
  },
  plugins: {
    legend: {
      position: "top",
    },
    tooltip: {
      callbacks: {
        label(context) {
          const value = context.parsed.y;
          if (value === null || Number.isNaN(value)) {
            return `${context.dataset.label}: no data`;
          }
          return `${context.dataset.label}: ${value.toFixed(1)}`;
        },
      },
    },
  },
  scales: {
    yHumidity: {
      type: "linear",
      position: "left",
      title: {
        display: true,
        text: "Humidity (%)",
      },
    },
    yTemperature: {
      type: "linear",
      position: "right",
      grid: {
        drawOnChartArea: false,
      },
      title: {
        display: true,
        text: "Temperature (C)",
      },
    },
  },
}));

const canGoForward = computed(() => currentPage.value > 1);

async function loadPage(page: number) {
  if (page < 1) {
    return;
  }

  isLoading.value = true;
  errorMessage.value = "";

  try {
    const response = await fetchReadingsPage(page, pageSize);

    if (response.readings.length === 0 && page > 1) {
      errorMessage.value = "No older readings available.";
      return;
    }

    currentPage.value = page;
    readings.value = response.readings;
  } catch {
    errorMessage.value = "Failed to load live readings.";
  } finally {
    isLoading.value = false;
  }
}

function goBack() {
  loadPage(currentPage.value + 1);
}

function goForward() {
  if (!canGoForward.value) {
    return;
  }

  loadPage(currentPage.value - 1);
}

onMounted(() => {
  loadPage(1);
  refreshTimer = setInterval(() => {
    loadPage(currentPage.value);
  }, 5 * 60 * 1000);
});

onUnmounted(() => {
  if (refreshTimer !== null) {
    clearInterval(refreshTimer);
  }
});
</script>

<template>
  <section class="live-readings-view">
    <div class="chart-card live-chart-card">
      <header class="top-row">
        <h1>Live Readings</h1>
        <div class="controls">
          <button
            class="nav-button"
            type="button"
            @click="goBack"
            :disabled="isLoading"
            aria-label="Back 10 readings"
          >
            ◀
          </button>
          <span class="page-indicator">Page {{ currentPage }}</span>
          <button
            class="nav-button"
            type="button"
            @click="goForward"
            :disabled="isLoading || !canGoForward"
            aria-label="Forward 10 readings"
          >
            ▶
          </button>
        </div>
      </header>

      <p v-if="errorMessage" class="error">{{ errorMessage }}</p>
      <p v-else-if="readings.length === 0" class="empty">No live readings available.</p>

      <div v-else class="chart-wrap">
        <Line :data="chartData" :options="chartOptions" />
      </div>
    </div>
  </section>
</template>

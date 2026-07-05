<script setup lang="ts">
import { computed } from "vue";
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
import type { ReadingSummary } from "../api/readingSummaries";

ChartJS.register(
  Title,
  Tooltip,
  Legend,
  CategoryScale,
  LinearScale,
  PointElement,
  LineElement
);

const props = defineProps<{
  days: ReadingSummary[];
}>();

const temperatureAxisMin = 0;
const temperatureAxisMax = 50;

function parseLocalDate(dateString: string): Date {
  const [yearPart, monthPart, dayPart] = dateString.split("-");
  const year = Number(yearPart ?? "1970");
  const month = Number(monthPart ?? "1");
  const day = Number(dayPart ?? "1");

  return new Date(year, month - 1, day);
}

function formatDayWithDate(dateString: string): string {
  const parsed = parseLocalDate(dateString);
  if (Number.isNaN(parsed.getTime())) {
    return dateString;
  }

  const weekday = parsed.toLocaleDateString([], { weekday: "short" });
  return `${weekday} (${dateString})`;
}

function toNumeric(value: number | string | null): number | null {
  if (value === null || value === "") {
    return null;
  }

  const parsed = typeof value === "number" ? value : Number(value);
  return Number.isFinite(parsed) ? parsed : null;
}

const chartData = computed<ChartData<"line">>(() => ({
  labels: props.days.map((day) => formatDayWithDate(day.date)),
  datasets: [
    {
      label: "Temperature Avg (C)",
      data: props.days.map((day) => toNumeric(day.temperature_avg)),
      borderColor: "#f97316",
      backgroundColor: "rgba(249, 115, 22, 0.2)",
      pointBackgroundColor: "#f97316",
      tension: 0.35,
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
    yTemperature: {
      type: "linear",
      position: "left",
      min: temperatureAxisMin,
      max: temperatureAxisMax,
      title: {
        display: true,
        text: "Temperature (C)",
      },
    },
  },
}));
</script>

<template>
  <section class="chart-card">
    <h2>Weekly Trend</h2>
    <p v-if="days.length === 0" class="empty">No readings available to chart.</p>
    <div v-else class="chart-wrap">
      <Line :data="chartData" :options="chartOptions" />
    </div>
  </section>
</template>

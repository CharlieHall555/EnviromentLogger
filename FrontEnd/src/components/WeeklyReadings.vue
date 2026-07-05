<script setup lang="ts">
import { computed, onMounted, onUnmounted, ref } from "vue";
import { fetchWeeklySummary, type WeeklySummary } from "../api/readingSummaries";
import WeeklyChart from "./WeeklyChart.vue";

function formatDate(date: Date): string {
  const year = date.getFullYear();
  const month = String(date.getMonth() + 1).padStart(2, "0");
  const day = String(date.getDate()).padStart(2, "0");
  return `${year}-${month}-${day}`;
}

function parseLocalDate(dateString: string): Date {
  const [yearPart, monthPart, dayPart] = dateString.split("-");
  const year = Number(yearPart ?? "1970");
  const month = Number(monthPart ?? "1");
  const day = Number(dayPart ?? "1");

  return new Date(year, month - 1, day);
}

function getMonday(dateString: string): string {
  const date = parseLocalDate(dateString);
  const day = date.getDay();
  const diff = day === 0 ? -6 : 1 - day;
  date.setDate(date.getDate() + diff);
  return formatDate(date);
}

function addDays(dateString: string, days: number): string {
  const date = parseLocalDate(dateString);
  date.setDate(date.getDate() + days);
  return formatDate(date);
}

function formatDayWithDate(dateString: string): string {
  const parsed = parseLocalDate(dateString);
  if (Number.isNaN(parsed.getTime())) {
    return dateString;
  }

  const weekday = parsed.toLocaleDateString([], { weekday: "short" });
  return `${weekday} (${dateString})`;
}

const selectedDate = ref(formatDate(new Date()));
const weeklySummary = ref<WeeklySummary | null>(null);
const isLoading = ref(false);
const errorMessage = ref("");
let refreshTimer: ReturnType<typeof setInterval> | null = null;

const currentWeekStart = computed(() => getMonday(formatDate(new Date())));
const selectedWeekStart = computed(() => getMonday(selectedDate.value));
const isFutureWeek = computed(
  () => selectedWeekStart.value > currentWeekStart.value
);
const canGoForward = computed(() => {
  const nextWeekStart = addDays(selectedWeekStart.value, 7);
  return nextWeekStart <= currentWeekStart.value;
});

const weekLabel = computed(() => {
  if (!weeklySummary.value) {
    return "";
  }

  return `${weeklySummary.value.weekStart} to ${weeklySummary.value.weekEnd}`;
});

function formatMetric(value: number | string | null): string {
  const numericValue = toNumeric(value);
  if (numericValue === null) {
    return "-";
  }

  return numericValue.toFixed(1);
}

function formatRange(
  minValue: number | string | null,
  maxValue: number | string | null
): string {
  const minText = formatMetric(minValue);
  const maxText = formatMetric(maxValue);
  return `[${minText} - ${maxText}]`;
}

function toNumeric(value: number | string | null): number | null {
  if (value === null || value === "") {
    return null;
  }

  const numericValue = typeof value === "number" ? value : Number(value);
  if (!Number.isFinite(numericValue)) {
    return null;
  }

  return numericValue;
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

function goToPreviousWeek() {
  selectedDate.value = addDays(selectedWeekStart.value, -7);
  loadWeeklyReadings();
}

function goToNextWeek() {
  if (!canGoForward.value) {
    return;
  }

  selectedDate.value = addDays(selectedWeekStart.value, 7);
  loadWeeklyReadings();
}

function onDateChange() {
  loadWeeklyReadings();
}

async function loadWeeklyReadings() {
  if (isFutureWeek.value) {
    errorMessage.value = "Future weeks are not available.";
    weeklySummary.value = null;
    return;
  }

  isLoading.value = true;
  errorMessage.value = "";

  try {
    weeklySummary.value = await fetchWeeklySummary(selectedDate.value);
  } catch {
    errorMessage.value = "Failed to load weekly readings.";
    weeklySummary.value = null;
  } finally {
    isLoading.value = false;
  }
}

onMounted(() => {
  loadWeeklyReadings();
  refreshTimer = setInterval(loadWeeklyReadings, 5 * 60 * 1000);
});

onUnmounted(() => {
  if (refreshTimer !== null) {
    clearInterval(refreshTimer);
  }
});
</script>

<template>
  <section class="weekly-view">
    <header class="top-row">
      <h1>Weekly Readings</h1>
      <div class="controls">
        <button
          class="nav-button"
          type="button"
          @click="goToPreviousWeek"
          :disabled="isLoading"
          aria-label="Previous week"
        >
          ◀
        </button>
        <label for="week-date">Date</label>
        <input
          id="week-date"
          v-model="selectedDate"
          type="date"
          @change="onDateChange"
        />
        <button
          class="nav-button"
          type="button"
          @click="goToNextWeek"
          :disabled="isLoading || !canGoForward"
          aria-label="Next week"
        >
          ▶
        </button>
      </div>
    </header>

    <p v-if="weekLabel" class="week-label">Week: {{ weekLabel }}</p>
    <p v-if="errorMessage" class="error">{{ errorMessage }}</p>

    <WeeklyChart v-if="!isLoading" :days="weeklySummary?.days ?? []" />

    <div v-if="!isLoading" class="table-wrap">
      <table>
        <thead>
          <tr>
            <th>Date</th>
            <th>Humidity</th>
            <th>Temperature</th>
          </tr>
        </thead>
        <tbody>
          <tr v-for="day in weeklySummary?.days" :key="day.date">
            <td>{{ formatDayWithDate(day.date) }}</td>
            <td>
              <div class="value-stack">
                <span class="value-item">
                  <span class="value-label">Avg</span>
                  <span class="value-main">{{ formatMetric(day.humidity_avg) }}</span>
                </span>
                <span class="value-item">
                  <span class="value-label">Range</span>
                  <span class="value-range">
                    {{ formatRange(day.humidity_min, day.humidity_max) }}
                  </span>
                </span>
              </div>
            </td>
            <td>
              <div class="value-stack">
                <span class="value-item">
                  <span class="value-label">Avg</span>
                  <span class="temp-value" :class="getTemperatureClass(day.temperature_avg)">
                    {{ formatMetric(day.temperature_avg) }}
                  </span>
                </span>
                <span class="value-item">
                  <span class="value-label">Range</span>
                  <span class="value-range temp-range">
                    {{ formatRange(day.temperature_min, day.temperature_max) }}
                  </span>
                </span>
              </div>
            </td>
          </tr>
          <tr v-if="(weeklySummary?.days.length ?? 0) === 0">
            <td colspan="3" class="empty">No readings found for this week.</td>
          </tr>
        </tbody>
      </table>
    </div>
  </section>
</template>

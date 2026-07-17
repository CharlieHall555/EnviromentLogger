<script setup lang="ts">
import { computed, ref, onMounted } from "vue";
import type { ReadingSummary , WeeklySummary , groupSummariesByWeek } from "@/api/readingSummaries";

const summaries = ref<ReadingSummary[]>([]);
const selectedWeekIndex = ref(0);

onMounted(async () => {
  summaries.value = await getReadingSummaries();
});

const weeks = computed(() => groupSummariesByWeek(summaries.value));

const selectedWeek = computed(() => {
  return weeks.value[selectedWeekIndex.value] ?? null;
});
</script>

<template>
  <section v-if="selectedWeek">
    <div class="week-header">
      <button
        :disabled="selectedWeekIndex === 0"
        @click="selectedWeekIndex--"
      >
        Previous
      </button>

      <h2>
        {{ selectedWeek.weekStart }} to {{ selectedWeek.weekEnd }}
      </h2>

      <button
        :disabled="selectedWeekIndex >= weeks.length - 1"
        @click="selectedWeekIndex++"
      >
        Next
      </button>
    </div>

    <div class="day-grid">
      <div
        v-for="day in selectedWeek.days"
        :key="day.date"
        class="day-card"
      >
        <h3>{{ day.date }}</h3>

        <p>
          Temp avg:
          {{ day.temperature_avg ?? "No data" }}
        </p>

        <p>
          Humidity avg:
          {{ day.humidity_avg ?? "No data" }}
        </p>

        <p>
          Readings:
          {{ day.total_size }}
        </p>
      </div>
    </div>
  </section>
</template>
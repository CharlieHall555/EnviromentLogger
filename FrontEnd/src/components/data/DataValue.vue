<script setup lang="ts">
import { ref, computed } from "vue";
import router from "@/router";
import {
  formatMetric,
  getTemperatureClass,
  getHumidityClass,
  getPM1Class,
  getPM25Class,
  getPM10Class
} from "@/utils/metricHelpers";
import { type ReadingType } from "@/domain/sensorReading";

const props = withDefaults(
  defineProps<{
    title?: string;
    value?: number | string | null;
    previousValue?: number | string | null;
    unit?: string;
    clickable?: boolean;
    readingType?: ReadingType;
    trendThreshold?: number;
  }>(),
  {
    title: "Unknown",
    unit: "",
    clickable: false,
    trendThreshold: 10
  }
);

const emit = defineEmits<{ (e: 'click', payload: number | string | null): void }>();

const valueClass = computed(() => {
  if (!props.readingType || props.value === null || props.value === undefined) {
    return "";
  }

  switch (props.readingType) {
    case "temperature":
      return getTemperatureClass(props.value);
    case "humidity":
      return getHumidityClass(props.value);
    case "pm1_0":
      return getPM1Class(props.value);
    case "pm2_5":
      return getPM25Class(props.value);
    case "pm10":
      return getPM10Class(props.value);
    default:
      return "";
  }
});

function onClick() {
  if (props.clickable && props.readingType) {
    router.push({
      name: "one-value-log",
      query: { readingType: props.readingType }
    });
  }
}

const trend = computed<"up" | "down" | "neutral" | null>(() => {
  if (
    props.value === null ||
    props.value === undefined ||
    props.previousValue === null ||
    props.previousValue === undefined
  ) {
    return null;
  }

  const current = Number(props.value);
  const previous = Number(props.previousValue);



  if (!Number.isFinite(current) || !Number.isFinite(previous)) {
    return null;
  }

  if (previous === 0) {
    if (current === 0) return "neutral";
    return current > 0 ? "up" : "down";
  }

  const percentageChange =
    ((current - previous) / Math.abs(previous)) * 100;

  if (Math.abs(percentageChange) <= props.trendThreshold) {
    return "neutral";
  }

  if (Math.abs(percentageChange) <= 10) {
    return "neutral";
  }

  return percentageChange > 0 ? "up" : "down";
});

const trendIcon = computed(() => {
  switch (trend.value) {
    case "up":
      return "/up_arrow.svg";
    case "down":
      return "/down_arrow.svg";
    case "neutral":
      return "/neutral_arrow.svg";
    default:
      return null;
  }
});

const trendAlt = computed(() => {
  switch (trend.value) {
    case "up":
      return "Increasing";
    case "down":
      return "Decreasing";
    case "neutral":
      return "No significant change";
    default:
      return "";
  }
});

</script>

<template>
  <div class="sensor-card clickable" @click="onClick">
    <h3>{{ props.title }}</h3>

    <div class="value-row">
      <p :class="valueClass">
        {{ formatMetric(value, unit, 2) }}
      </p>

      <img v-if="trendIcon" :src="trendIcon" :alt="trendAlt" class="trend-icon" />
    </div>
  </div>
</template>

<style scoped>
.value-row {
  display: flex;
  align-items: center;
  gap: 0.35em;
  font-size: 2rem;
}

.value-row p {
  margin: 0;
  font-size: inherit;
}

.trend-icon {
  width: 1em;
  height: 1em;
}
</style>
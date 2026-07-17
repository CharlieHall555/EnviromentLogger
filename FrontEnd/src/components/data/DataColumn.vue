<script setup lang="ts">
import { ref, computed } from "vue";
import "@/assets/main.css";
import router from "@/router";
import {
  formatMetric,
  formatShortDateTime,
  getTemperatureClass,
  getHumidityClass,
  getPM1Class,
  getPM25Class,
  getPM10Class
} from "@/utils/metricHelpers";
import { type ReadingType } from "@/domain/sensorReading";

const props = withDefaults(
  defineProps<{
    value?: number | string | null;
    unit?: string;
    readingType?: ReadingType;
  }>(),
  {
    unit: "",
  }
);

const displayValue = computed(() => {
  if (typeof props.value === 'string' && isNaN(Number(props.value))) {
    // Try to parse as date/time
    const date = new Date(props.value);
    if (!isNaN(date.getTime())) {
      return formatShortDateTime(props.value);
    }
    return props.unit ? `${props.value} ${props.unit}` : props.value;
  }
  return formatMetric(props.value, props.unit, 2);
});

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

</script>

<template>
  <p class="data-text" :class="valueClass">
    {{ displayValue }}
  </p>
</template>

<style scoped>
p {
  margin: 0;
  text-align: center;
  justify-self: center;
}
</style>

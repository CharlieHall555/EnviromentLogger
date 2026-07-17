<script setup lang="ts">

import DataColumn from "@/components/data/DataColumn.vue";
import { computed } from "vue";
import { type SensorReading } from '@/domain/sensorReading.ts';

const props = defineProps<{
  // Option 1: Pass a sensor reading object
  reading?: SensorReading;
  
  // Option 2: Pass individual values
  temperature?: number;
  humidity?: number;
  pm1_0?: number;
  pm2_5?: number;
  pm10?: number;
  measured_at?: string;
}>();

// Use reading object values if provided, otherwise use individual props
const temperatureValue = computed(() => props.reading?.temperature ?? props.temperature);
const humidityValue = computed(() => props.reading?.humidity ?? props.humidity);
const pm1_0Value = computed(() => props.reading?.pm1_0 ?? props.pm1_0);
const pm2_5Value = computed(() => props.reading?.pm2_5 ?? props.pm2_5);
const pm10Value = computed(() => props.reading?.pm10 ?? props.pm10);
const measuredAtValue = computed(() => props.reading?.measured_at ?? props.measured_at);

const columnCount = computed(() => {
  return [
    measuredAtValue.value,
    temperatureValue.value,
    humidityValue.value,
    pm1_0Value.value,
    pm2_5Value.value,
    pm10Value.value
  ].filter(value => value !== undefined).length;
});

</script>

<template>
  <div
    class="data-row"
    :style="{ gridTemplateColumns: `repeat(${columnCount}, minmax(120px, 1fr))` }"
  >
    <DataColumn v-if="measuredAtValue !== undefined" :value="measuredAtValue" unit="" />
    <DataColumn v-if="temperatureValue !== undefined" :value="temperatureValue" unit="°C" readingType="temperature" />
    <DataColumn v-if="humidityValue !== undefined" :value="humidityValue" unit="%" readingType="humidity" />
    <DataColumn v-if="pm1_0Value !== undefined" :value="pm1_0Value" unit="µg/m³" readingType="pm1_0" />
    <DataColumn v-if="pm2_5Value !== undefined" :value="pm2_5Value" unit="µg/m³" readingType="pm2_5" />
    <DataColumn v-if="pm10Value !== undefined" :value="pm10Value" unit="µg/m³" readingType="pm10" />
  </div>
</template>

<style scoped>
.data-row {
  display: grid;
  gap: 1rem;
  align-items: center;
  width: 100%;
}
</style>
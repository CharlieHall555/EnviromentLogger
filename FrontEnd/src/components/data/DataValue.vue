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
    unit?: string;
    clickable?: boolean;
    readingType?: ReadingType;
  }>(),
  {
    title: "Unknown",
    unit: "",
    clickable: false
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

</script>

<template>
    <div class="sensor-card clickable" @click="onClick">
        <h3>{{ props.title }}</h3>
        <p :class="valueClass">{{formatMetric(value , unit , 2)}}</p>
    </div>
</template>

<script setup lang="ts">

import DataValue from "@/components/data/DataValue.vue";
import { ref , onMounted, onUnmounted} from "vue";
import { type SensorReading, getBlankSensorReading } from '../domain/sensorReading.ts';
import { fetchLatestReading } from '../api/latestReading.ts';
import RealTimeSummary from "@/components/data/RealTimeSummary.vue";
import ApiStatus from "@/components/status/Api-Status.vue";

const REFRESH_TIME = 5 * 60;
const latestReading = ref<SensorReading | null>(null);
const timeToUpdate = ref<number>(REFRESH_TIME);
const loading = ref<boolean>(true);


let timerId: number;

async function refreshLatestReading(){
    latestReading.value = await fetchLatestReading() ;
}

function updateTimer(){
  timeToUpdate.value -= 1;
  if (timeToUpdate.value <= 0){
    refreshLatestReading();
    timeToUpdate.value = REFRESH_TIME;
  }
}


onMounted(async () => {
  timerId = window.setInterval(updateTimer, 1000);
  refreshLatestReading()
});

onUnmounted(() => {
  clearInterval(timerId);
});

</script>

<template>
  
  <RealTimeSummary :current-reading="latestReading"/>
  <ApiStatus/>
  
</template>
<style scoped></style>

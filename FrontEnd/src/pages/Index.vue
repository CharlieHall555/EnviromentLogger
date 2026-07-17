<script setup lang="ts">

import DataValue from "@/components/data/DataValue.vue";
import { ref , onMounted, onUnmounted} from "vue";
import { type SensorReading, getBlankSensorReading } from '../domain/sensorReading.ts';
import { fetchLatestReadings } from '../api/latestReading.ts';
import RealTimeSummary from "@/components/data/RealTimeSummary.vue";
import ApiStatus from "@/components/status/Api-Status.vue";
import { isDataServiceApiHealthy } from "@/api/status.ts";

const REFRESH_TIME = 5 * 60;
const latestReading = ref<SensorReading | null>(null);
const previousReading = ref<SensorReading | null>(null);
const apiOK = ref<boolean>(false);
const timeToUpdate = ref<number>(REFRESH_TIME);
const loading = ref<boolean>(true);

let timerId: number;

async function refreshAPIs(){
    const readings = await fetchLatestReadings() ;
    latestReading.value = readings.latest;
    previousReading.value = readings.previous;
    apiOK.value = await isDataServiceApiHealthy();
}

function updateTimer(){
  timeToUpdate.value -= 1;
  if (timeToUpdate.value <= 0){
    refreshAPIs();
    timeToUpdate.value = REFRESH_TIME;
  }
}


onMounted(async () => {
  timerId = window.setInterval(updateTimer, 1000);
  refreshAPIs()
});

onUnmounted(() => {
  clearInterval(timerId);
});

</script>

<template>
  
  <RealTimeSummary :current-reading="latestReading" :previous-reading="previousReading" />
  <ApiStatus :apihealthy="apiOK" :databasehealthy="apiOK"/>
  
</template>
<style scoped></style>

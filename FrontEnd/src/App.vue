<script setup lang="ts">
import { onMounted, onUnmounted, ref } from "vue";
import CurrentStatus from "./components/CurrentStatus.vue";
import LiveReadingsChart from "./components/LiveReadingsChart.vue";
import SystemStatus from "./components/SystemStatus.vue";
import WeeklyReadings from "./components/WeeklyReadings.vue";
import { isDataServiceApiHealthy } from "./api/status";

const apiStatus = ref("Online");
const apiLastPolled = ref("Never");
let statusRefreshTimer: ReturnType<typeof setInterval> | null = null;

async function updateStatus() {
  const isHealthy = await isDataServiceApiHealthy();
  apiStatus.value = isHealthy ? "Online" : "Offline";
  apiLastPolled.value = new Date().toISOString();
}

onMounted(() => {
  updateStatus();
  statusRefreshTimer = setInterval(updateStatus, 5 * 60 * 1000);
});

onUnmounted(() => {
  if (statusRefreshTimer !== null) {
    clearInterval(statusRefreshTimer);
  }
});
</script>

<template>
  <main>
    <header class="top-nav-wrap">
      <nav class="top-nav" aria-label="Main navigation">
        <a class="brand" href="#">Air Quality Sensor</a>

        <div class="nav-links">
          <a href="#status">Status</a>
          <a href="#live-readings">Live Readings</a>
          <a href="#weekly-readings">Weekly Readings</a>
          <a href="#system-status">System Status</a>
        </div>
      </nav>
    </header>

    <div class="page-content">
      <section id="status" class="page-section">
        <CurrentStatus />
      </section>

      <section id="live-readings" class="page-section">
        <LiveReadingsChart />
      </section>

      <section id="weekly-readings" class="page-section">
        <WeeklyReadings />
      </section>

      <section id="system-status" class="page-section system-status-wrap">
        <SystemStatus :api-status="apiStatus" :api-last-polled="apiLastPolled" />
      </section>
    </div>
  </main>
</template>
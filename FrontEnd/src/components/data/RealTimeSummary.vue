<script setup lang="ts">
import { computed } from "vue";
import DataValue from "@/components/data/DataValue.vue";
import { type SensorReading } from "@/domain/sensorReading";
import { formatShortDateTime } from "@/utils/metricHelpers";

const props = defineProps<{
  currentReading: SensorReading | null;
}>();

const measuredAtLabel = computed(() => {
  if (!props.currentReading) {
    return "--";
  }

  return formatShortDateTime(props.currentReading.measured_at);
});
</script>

<template>
  <section v-if="currentReading" class="summary-panel">
    <header class="summary-header">
      <div>
        <p class="summary-kicker">Snapshot</p>
        <h2 class="summary-title">Current Air Conditions</h2>
      </div>
      <p class="summary-time">Measrued At: {{ measuredAtLabel }}</p>
    </header>

    <div class="metric-group metric-group-primary">
      <DataValue
        title="Temperature"
        :value="currentReading.temperature"
        unit="°C"
        :clickable="true"
        readingType="temperature"
      />
      <DataValue
        title="Humidity"
        :value="currentReading.humidity"
        unit="%"
        :clickable="true"
        readingType="humidity"
      />
    </div>

    <div class="metric-group metric-group-secondary">
      <DataValue
        title="PM1.0"
        :value="currentReading.pm1_0"
        unit="µg/m³"
        :clickable="true"
        readingType="pm1_0"
      />
      <DataValue
        title="PM2.5"
        :value="currentReading.pm2_5"
        unit="µg/m³"
        :clickable="true"
        readingType="pm2_5"
      />
      <DataValue
        title="PM10"
        :value="currentReading.pm10"
        unit="µg/m³"
        :clickable="true"
        readingType="pm10"
      />
    </div>
  </section>

  <section v-else class="summary-loading" aria-live="polite">
    <p>Loading latest sensor reading...</p>
  </section>
</template>

<style scoped>
.summary-panel {
  width: min(1080px, 100%);
  margin: 0 auto;
  padding: 1.25rem;
}

.summary-header {
  display: flex;
  justify-content: space-between;
  align-items: flex-end;
  gap: 1rem;
  margin-bottom: 1rem;
}

.summary-kicker {
  margin: 0;
  color: var(--text-secondary);
  font-size: 0.75rem;
  font-weight: 600;
  letter-spacing: 0.1em;
  text-transform: uppercase;
}

.summary-title {
  margin: 0.3rem 0 0;
  color: var(--text-primary);
  font-size: clamp(1.2rem, 2.5vw, 1.7rem);
  font-weight: 650;
}

.summary-time {
  margin: 0;
  color: var(--text-secondary);
  font-size: 0.9rem;
  white-space: nowrap;
}

.metric-group {
  display: grid;
  gap: 1rem;
}

.metric-group-primary {
  grid-template-columns: repeat(2, minmax(0, 1fr));
  margin-bottom: 1rem;
}

.metric-group-secondary {
  grid-template-columns: repeat(3, minmax(0, 1fr));
}

.metric-group-primary :deep(.sensor-card) {
  min-height: 140px;
}

.metric-group-primary :deep(.sensor-card p) {
  font-size: clamp(2.1rem, 5vw, 2.9rem);
}

.summary-loading {
  width: min(1080px, 100%);
  margin: 0 auto;
  padding: 2.5rem 1.25rem;
  color: var(--text-secondary);
  text-align: center;
}

@media (max-width: 880px) {
  .metric-group-secondary {
    grid-template-columns: repeat(2, minmax(0, 1fr));
  }
}

@media (max-width: 640px) {
  .summary-panel {
    padding: 1rem;
  }

  .summary-header {
    flex-direction: column;
    align-items: flex-start;
  }

  .summary-time {
    font-size: 0.85rem;
  }

  .metric-group-primary,
  .metric-group-secondary {
    grid-template-columns: 1fr;
  }
}
</style>
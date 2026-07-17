<script setup lang="ts">
defineProps<{
  apihealthy: boolean;
  databasehealthy: boolean;
}>();
</script>

<template>
  <section class="summary-panel">
    <header class="summary-header">
      <div>
        <p class="summary-kicker">Status</p>
        <h2 class="summary-title">System Health</h2>
      </div>
    </header>

    <div class="status-grid">
      <div
        class="sensor-card api-status-card"
        :class="apihealthy ? 'status-healthy' : 'status-unhealthy'"
      >
        <h3>API Health</h3>

        <p>
          {{ apihealthy ? 'Operational' : 'Unavailable' }}
        </p>

        <span class="status-message">
          {{
            apihealthy
              ? 'The API is responding normally.'
              : 'The API is not currently responding.'
          }}
        </span>
      </div>

      <div
        class="sensor-card api-status-card"
        :class="databasehealthy ? 'status-healthy' : 'status-unhealthy'"
      >
        <h3>Database Health</h3>

        <p>
          {{ databasehealthy ? 'Operational' : 'Unavailable' }}
        </p>

        <span class="status-message">
          {{
            databasehealthy
              ? 'The database connection is working normally.'
              : 'The database is not currently responding.'
          }}
        </span>
      </div>

      <div class="sensor-card api-status-card">
        <h3>Enviroment Logger</h3>

        <p>System Log</p>

        <span class="status-message">
            Logging message from hardware.
        </span>
      </div>
    </div>
  </section>
</template>

<style scoped>

.summary-panel {
  width: min(1080px, 100%);
  margin: 2rem auto 0;
  padding: 1rem 1.25rem 1.25rem;
}

.summary-header {
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
}

.status-grid {
  display: grid;
  grid-template-columns: repeat(3, minmax(0, 1fr));
  gap: 1rem;
}

.api-status-card {
  min-height: 140px;
}

.api-status-card p {
  transition: color 0.2s ease;
}

.status-healthy p {
  color: #38d996;
}

.status-unhealthy p {
  color: var(--danger);
}

.status-message {
  color: var(--text-secondary);
  font-size: 0.9rem;
}

@media (max-width: 640px) {
  .summary-panel {
    margin-top: 1.25rem;
    padding: 1rem;
  }

  .status-grid {
    grid-template-columns: 1fr;
  }
}
</style>
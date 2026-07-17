<script setup lang="ts"></script>

<template>
  <section class="summary-panel">
    <header class="summary-header">
      <div>
        <p class="summary-kicker">Status</p>
        <h2 class="summary-title">API</h2>
      </div>
    </header>
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
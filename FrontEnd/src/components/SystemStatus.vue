<script setup>
import { computed } from 'vue'

const props = defineProps({
  apiStatus: {
    type: String,
    default: 'Online'
  },
  apiLastPolled: {
    type: String,
    default: 'Never'
  }
})

const lastUpdatedText = computed(() => {
  if (!props.apiLastPolled || props.apiLastPolled === 'Never') {
    return 'Never'
  }

  const parsedDate = new Date(props.apiLastPolled)
  if (Number.isNaN(parsedDate.getTime())) {
    return props.apiLastPolled
  }

  return parsedDate.toLocaleString()
})
</script>

<template>
  <div class="status-card">
    <div class="card-header">
      <h3>System Status</h3>
      <span class="status-dot"></span>
    </div>

    <div class="status-row">
      <div class="icon">🌐</div>
      <div class="status-info">
        <p>API</p>
        <span class="last-updated">{{ lastUpdatedText }}</span>
      </div>
      <span
        class="badge"
        :class="apiStatus.toLowerCase() === 'offline' ? 'error' : 'success'"
      >
        {{ apiStatus }}
      </span>
    </div>
  </div>
</template>

<style scoped>
.status-card {
  background: white;
  border-radius: 16px;
  padding: 20px;
  box-shadow: 0 2px 10px rgba(0, 0, 0, 0.06);
  border: 1px solid #e5e7eb;
}

.card-header {
  display: flex;
  justify-content: space-between;
  align-items: center;
  margin-bottom: 16px;
}

.card-header h3 {
  margin: 0;
  font-size: 18px;
  color: #111827;
}

.status-dot {
  width: 10px;
  height: 10px;
  background: #22c55e;
  border-radius: 50%;
}

.status-row {
  display: flex;
  align-items: center;
  gap: 12px;
  padding: 14px 0;
  border-bottom: 1px solid #f1f5f9;
}

.status-row:last-child {
  border-bottom: none;
}

.icon {
  width: 36px;
  height: 36px;
  border-radius: 10px;
  background: #eff6ff;
  display: flex;
  align-items: center;
  justify-content: center;
  font-size: 18px;
}

.status-info {
  flex: 1;
}

.status-info p {
  margin: 0;
  font-size: 14px;
  font-weight: 600;
  color: #000000;
}

.last-updated {
  font-size: 14px;
  font-weight: 400;
  color: #475569;
}

.badge {
  font-size: 12px;
  padding: 5px 9px;
  border-radius: 999px;
  font-weight: 600;
}

.success {
  background: #dcfce7;
  color: #15803d;
}

.error {
  background: #fee2e2;
  color: #b91c1c;
}
</style>
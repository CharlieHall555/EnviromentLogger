<script setup lang="ts">

import { fetchReadingsPage, fetchReadingMaxPage } from '@/api/latestReading';
import DataColumn from '@/components/data/DataColumn.vue';
import DataRow from '@/components/data/DataRow.vue';
import { type ReadingType, type SensorReading } from '@/domain/sensorReading';
import { ref, onMounted, onUnmounted, computed } from 'vue';
import { useRoute } from 'vue-router';
import { Line } from 'vue-chartjs';
import {
    Chart as ChartJS,
    CategoryScale,
    LinearScale,
    PointElement,
    LineElement,
    Title,
    Tooltip,
    Legend
} from 'chart.js';
import DataValue from '@/components/data/DataValue.vue';
import { fetchLatestReadings } from '@/api/latestReading';
import TableHeading from '@/components/data/TableHeading.vue';

ChartJS.register(
    CategoryScale,
    LinearScale,
    PointElement,
    LineElement,
    Title,
    Tooltip,
    Legend
);

const route = useRoute();
const readingType = ref<ReadingType | null>(route.query.readingType as ReadingType || null);
const pageSize = ref(10);

const currentPage = ref<number>(1);
const maxPage = ref<number>(1);
const loading = ref<boolean>(true);
const readings = ref<SensorReading[]>([]);
const latestReading = ref<SensorReading | null>(null);
const previousReading = ref<SensorReading | null>(null);
const REFRESH_INTERVAL_MS = 5 * 60 * 1000;
let refreshIntervalId: ReturnType<typeof setInterval> | null = null;

const currentValue = computed(() => {
    if (!latestReading.value || !readingType.value) return null;
    return latestReading.value[readingType.value];
});

const previousValue = computed(() => {
    if (!previousReading.value || !readingType.value) return null;
    return previousReading.value[readingType.value];
});

const currentUnit = computed(() => {
    switch (readingType.value) {
        case 'temperature':
            return '°C';
        case 'humidity':
            return '%';
        case 'pm1_0':
        case 'pm2_5':
        case 'pm10':
            return 'µg/m³';
        default:
            return '';
    }
});

const currentTitle = computed(() => {
    if (!readingType.value) return 'Current Reading';
    const formatted = readingType.value.charAt(0).toUpperCase() + readingType.value.slice(1).replace(/_/g, '.');
    return `Current ${formatted}`;
});

const chartData = computed(() => {
    if (!readingType.value || readings.value.length === 0) {
        return null;
    }

    return {
        labels: readings.value.map(r => {
            const date = new Date(r.measured_at);
            return `${date.getMonth() + 1}/${date.getDate()} ${date.getHours()}:${String(date.getMinutes()).padStart(2, '0')}`;
        }).reverse(),
        datasets: [
            {
                label: readingType.value,
                data: readings.value.map(r => r[readingType.value!]).reverse(),
                borderColor: '#38d996',
                backgroundColor: 'rgba(56, 217, 150, 0.1)',
                tension: 0
            }
        ]
    };
});


const chartOptions = computed(() => {
    const values = readings.value
        .map(reading =>
            readingType.value ? Number(reading[readingType.value]) : NaN
        )
        .filter(value => Number.isFinite(value));

    const minReading = values.length > 0 ? Math.min(...values) : 0;
    const maxReading = values.length > 0 ? Math.max(...values) : 0;

    return {
        responsive: true,
        maintainAspectRatio: false,
        plugins: {
            legend: {
                display: true
            },
            title: {
                display: true,
                text: `${
                    readingType.value
                        ? readingType.value.charAt(0).toUpperCase() +
                          readingType.value.slice(1).replace(/_/g, '.')
                        : ''
                } Over Time`
            }
        },
        scales: {
            y: {
                beginAtZero: false,
                suggestedMin: minReading - 5,
                suggestedMax: maxReading + 5
            }
        }
    };
});

onMounted(async () => {
    await refreshData();

    refreshIntervalId = setInterval(() => {
        void refreshData();
    }, REFRESH_INTERVAL_MS);
});

onUnmounted(() => {
    if (refreshIntervalId) {
        clearInterval(refreshIntervalId);
        refreshIntervalId = null;
    }
});

async function refreshData() {
    await updateMaxPage();
    await loadLatestReading();
    await loadReadings();
}

async function loadLatestReading() {
    try {
        const readings = await fetchLatestReadings();
        latestReading.value = readings.latest;
        previousReading.value = readings.previous;
    } catch (err) {
        console.error('Failed to fetch latest reading:', err);
    }
}

async function updateMaxPage() {
    const pageSizeReponse = await fetchReadingMaxPage(pageSize.value);
    maxPage.value = pageSizeReponse.n_pages;
}

function handlePageSizeChange() {
    currentPage.value = 1; // Reset to first page when changing page size
    updateMaxPage();
    loadReadings();
}

function forwardClick() {
    currentPage.value = currentPage.value + 1
    if (currentPage.value > maxPage.value) {
        currentPage.value = maxPage.value
    }
    loadReadings();
}

function backClick() {
    currentPage.value = currentPage.value - 1
    if (currentPage.value < 1) {
        currentPage.value = 1
    }
    loadReadings();
}

async function loadReadings() {
    loading.value = true;
    try {
        const readingReponse = await fetchReadingsPage(
            currentPage.value,
            pageSize.value
        );
        readings.value = readingReponse.readings;
    } catch (err) {
        console.error(err);
    } finally {
        loading.value = false;
    }
}

</script>

<template>
    <div class="page-container">
        <header class="page-header">
            <h1>Sensor Readings Log</h1>
            <p v-if="readingType" class="reading-type-label">
                {{ readingType.charAt(0).toUpperCase() + readingType.slice(1).replace(/_/g, '.') }}
            </p>
        </header>

        <DataValue 
            v-if="readingType && latestReading"
            :title="currentTitle"
            :value="currentValue"
            :previous-value="previousValue"
            :unit="currentUnit"
            :readingType="readingType"
        />
        <br>

        <div class="controls card">
            <div class="control-group">
                <label for="pageSize">Page Size:</label>
                <select id="pageSize" v-model.number="pageSize" @change="handlePageSizeChange">
                    <option :value="10">10</option>
                    <option :value="25">25</option>
                    <option :value="50">50</option>
                    <option :value="100">100</option>
                </select>
            </div>

            <div class="pagination">
                <button class="btn-primary" @click="backClick" :disabled="currentPage <= 1">
                    ← Previous
                </button>
                <span class="page-info">Page {{ currentPage }} / {{ maxPage }}</span>
                <button class="btn-primary" @click="forwardClick" :disabled="currentPage >= maxPage">
                    Next →
                </button>
            </div>
        </div>

        <div v-if="readingType">
            <div v-if="chartData" class="chart-container card">
                <Line :data="chartData" :options="chartOptions" />
            </div>

            <div class="readings-list card">
                <div v-if="loading" class="loading">Loading...</div>
                <div v-else>
                    <TableHeading :headings="['date' , readingType]" />
                    <div v-for="reading in readings" :key="reading.id" class="reading-item">
                        <DataRow v-bind="{ [readingType]: reading[readingType], measured_at: reading.measured_at }" />
                    </div>
                </div>
            </div>
        </div>

        <div v-else class="no-data card">
            <p>Please select a reading type to view data.</p>
        </div>
    </div>
</template>

<style scoped>


.page-header h1 {
    font-size: 2rem;
    color: var(--text-primary);
    margin-bottom: 0.5rem;
}

.reading-type-label {
    font-size: 1.2rem;
    color: var(--accent);
    font-weight: 600;
}

.controls {
    display: flex;
    justify-content: space-between;
    align-items: center;
    padding: 1.5rem;
    margin-bottom: 2rem;
    flex-wrap: wrap;
    gap: 1rem;
}

.control-group {
    display: flex;
    align-items: center;
    gap: 0.5rem;
}

.control-group label {
    font-weight: 600;
    color: var(--text-primary);
}

.control-group select {
    padding: 0.5rem 1rem;
    border-radius: 8px;
    border: 1px solid var(--border);
    background: var(--background);
    color: var(--text-primary);
    font-size: 1rem;
    cursor: pointer;
    transition: border-color 0.2s;
}

.control-group select:hover {
    border-color: var(--accent);
}

.control-group select:focus {
    outline: none;
    border-color: var(--accent);
}

.pagination {
    display: flex;
    align-items: center;
    gap: 1rem;
}

.page-info {
    font-weight: 600;
    color: var(--text-primary);
    min-width: 100px;
    text-align: center;
}

.chart-container {
    height: 400px;
    margin-bottom: 2rem;
    padding: 1.5rem;
}

.readings-list {
    padding: 1.5rem;
}

.reading-item {
    padding: 0.75rem;
    border-bottom: 1px solid var(--border);
    transition: background 0.2s;
    border-radius: 8px;
}

.reading-item:last-child {
    border-bottom: none;
}

.reading-item:hover {
    background: var(--surface-hover);
}

.loading {
    text-align: center;
    padding: 2rem;
    font-size: 1.1rem;
    color: var(--text-secondary);
}

.no-data {
    text-align: center;
    padding: 3rem;
}

.no-data p {
    font-size: 1.1rem;
    color: var(--text-secondary);
}
</style>
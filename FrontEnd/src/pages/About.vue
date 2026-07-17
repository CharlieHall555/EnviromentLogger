<script setup lang="ts">

</script>

<template>
    <div class="page-container">
        <header class="page-header">
            <h1>About</h1>
        </header>
        <section class="about-card">
            <p>
                This project is a full-stack air-quality monitoring system designed to collect, store, and display
                environmental data over time.
            </p>

            <p>
                An ESP32 reads temperature, humidity, pressure, and particulate matter levels using connected
                sensors, including a BME280 and PMS5003. The device sends these readings to a backend API, where
                they are stored in a PostgreSQL database for later analysis.
            </p>

            <p>
                The frontend dashboard is built with Vue, TypeScript, and Chart.js. It displays the latest sensor
                readings, historical data, status indicators, and graphs showing how conditions change over time.
                The interface also includes light and dark themes and is designed to work across different screen
                sizes.
            </p>

            <p>
                The system includes hardware logging, error reporting, sensor retry handling, Wi-Fi reconnection,
                and deep-sleep support to reduce power usage. Old readings and hardware logs can also be removed
                automatically to prevent the database from growing indefinitely.
            </p>

            <p>
                The main goal of the project is to create a reliable and accessible way to monitor indoor air
                quality while developing practical experience with embedded systems, APIs, databases, deployment,
                and modern frontend development.
            </p>
        </section>

    </div>
</template>
<style scoped>

.about-card {
    max-width: 700px;
    padding: 1.5rem;
    margin: 0 auto;
}

.about-card p {
    margin: 0 0 1rem;
    line-height: 1.6;
    color: var(--text-secondary);
    text-align: center;
}

.about-card p:last-child {
    margin-bottom: 0;
}

.page-header h1 {
    font-size: 2rem;
    color: var(--text-primary);
    margin-bottom: 1rem;
}
</style>

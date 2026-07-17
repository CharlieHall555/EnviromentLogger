<script setup lang="ts">
import { ref, onMounted } from 'vue';
import BaselineDarkModeIcon from '@iconify-vue/ic/baseline-dark-mode';
import BaselineLightModeIcon from '@iconify-vue/ic/baseline-light-mode';

const isDarkMode = ref(true);

onMounted(() => {
    const savedTheme = localStorage.getItem('theme');

    if (savedTheme === 'light') {
        isDarkMode.value = false;
        document.documentElement.classList.add('light-mode');
    }
});

function toggleTheme() {
    isDarkMode.value = !isDarkMode.value;

    if (isDarkMode.value) {
        document.documentElement.classList.remove('light-mode');
        localStorage.setItem('theme', 'dark');
    } else {
        document.documentElement.classList.add('light-mode');
        localStorage.setItem('theme', 'light');
    }
}
</script>

<template>
    <nav>
        <RouterLink to="/">Dashboard</RouterLink>

        <button
            class="theme-toggle"
            @click="toggleTheme"
            :aria-label="isDarkMode ? 'Switch to light mode' : 'Switch to dark mode'"
        >
            <BaselineLightModeIcon v-if="isDarkMode" />
            <BaselineDarkModeIcon v-else />
        </button>
    </nav>

    <main class="main-content">
        <RouterView />
    </main>
</template>

<style scoped>
nav {
    display: flex;
    justify-content: space-between;
    align-items: center;
    padding: 1rem 2rem;
    background: var(--surface);
    border-bottom: 1px solid var(--border);
}

nav a {
    color: var(--text-primary);
    text-decoration: none;
    font-weight: 600;
    transition: color 0.2s;
}

nav a:hover {
    color: var(--accent);
}

.theme-toggle {
    display: flex;
    align-items: center;
    justify-content: center;

    width: 42px;
    height: 42px;
    padding: 0;

    color: var(--text-primary);
    background: var(--surface-hover);
    border: 1px solid var(--border);
    border-radius: 8px;

    font-size: 1.5rem;
    cursor: pointer;
    transition:
        border-color 0.2s,
        color 0.2s,
        transform 0.2s;
}

.theme-toggle:hover {
    color: var(--accent);
    border-color: var(--accent);
    transform: scale(1.1);
}

</style>
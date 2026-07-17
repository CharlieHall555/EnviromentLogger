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
  } else {
    isDarkMode.value = true;
    document.documentElement.classList.remove('light-mode');
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
  <nav class="navbar">
    <div class="nav-buttons">
      <RouterLink to="/">
        Dashboard
      </RouterLink>

      <RouterLink to="/about">
        About
      </RouterLink>

      <a
        href="https://github.com/CharlieHall555/EnviromentLogger"
        target="_blank"
        rel="noopener noreferrer"
      >
        Repo
      </a>
    </div>

    <button
      class="theme-toggle"
      type="button"
      @click="toggleTheme"
      :aria-label="
        isDarkMode
          ? 'Switch to light mode'
          : 'Switch to dark mode'
      "
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
.navbar {
  position: relative;

  display: flex;
  justify-content: center;
  align-items: center;

  min-height: 74px;
  padding: 1rem 5.5rem;

  background: var(--surface);
  border-bottom: 1px solid var(--border);
}

.nav-buttons {
  display: flex;
  justify-content: center;
  align-items: center;
  gap: 0.75rem;
}

.nav-buttons a {
  padding: 0.65rem 1rem;

  color: var(--text-primary);
  background: transparent;

  border: 1px solid transparent;
  border-radius: 8px;

  font-weight: 600;
  text-decoration: none;

  transition:
    color 0.2s ease,
    background-color 0.2s ease,
    border-color 0.2s ease,
    transform 0.2s ease;
}

.nav-buttons a:hover {
  color: var(--accent);
  background: var(--surface-hover);
  border-color: var(--border);
  transform: translateY(-1px);
}

.nav-buttons a.router-link-active {
  color: var(--accent);
  background: var(--surface-hover);
  border-color: var(--border-hover);
}

.theme-toggle {
  position: absolute;
  top: 50%;
  right: 2rem;
  transform: translateY(-50%);

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
    border-color 0.2s ease,
    color 0.2s ease,
    background-color 0.2s ease,
    transform 0.2s ease;
}

.theme-toggle:hover {
  color: var(--accent);
  border-color: var(--accent);
  transform: translateY(-50%) scale(1.08);
}

.main-content {
  min-height: calc(100vh - 74px);
}

@media (max-width: 640px) {
  .navbar {
    min-height: 66px;
    padding: 0.75rem 4.5rem 0.75rem 1rem;
  }

  .nav-buttons {
    gap: 0.25rem;
  }

  .nav-buttons a {
    padding: 0.55rem 0.7rem;
    font-size: 0.9rem;
  }

  .theme-toggle {
    right: 1rem;
    width: 38px;
    height: 38px;
    font-size: 1.3rem;
  }
}
</style>
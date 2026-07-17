<script setup lang="ts">
defineProps<{
  headings: string[];
}>();

function formatHeading(heading: string): string {
  return heading
    .replace(/_/g, ".")
    .replace(/\b\w/g, character => character.toUpperCase());
}
</script>

<template>
  <div class="table-heading-row">
    <p
      v-for="heading in headings"
      :key="heading"
      class="data-heading data-header-text"
    >
      {{ formatHeading(heading) }}
    </p>
  </div>
</template>

<style scoped>

.table-heading-row {
  display: grid;
  grid-template-columns: repeat(2, minmax(120px, 1fr));
  gap: 1rem;
  width: 100%;

  justify-items: center; /* Centers each grid item */
}

.data-heading {
  margin: 0;
  text-align: center;     /* Centers the text */
}
</style>
import { createRouter, createWebHistory } from "vue-router";
import Index from "@/pages/Index.vue";
import ReadingLog from "./pages/ReadingLog.vue";
import OneValueReadingLog from "@/pages/OneValueReadingLog.vue";
import About from "./pages/About.vue";

const router = createRouter({
  history: createWebHistory(),
  routes: [
    {
      path: "/",
      name: "dashboard",
      component: Index,
    },

    {
      path: "/one-value-log",
      name: "one-value-log",
      component: OneValueReadingLog
    },

    {
      path: "/about",
      name: "about",
      component: About
    },
  ],
});

export default router;
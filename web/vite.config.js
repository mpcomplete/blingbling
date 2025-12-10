import { defineConfig } from 'vite';

export default defineConfig({
  server: {
    port: 3000,
    open: true
  },
  base: '/blingbling',
  build: {
    outDir: 'dist'
  }
});
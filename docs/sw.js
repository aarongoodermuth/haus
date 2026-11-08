// Haus service worker — build 2026-10-23.31.
// Retrigger 2026-11-06: force a fresh Pages deploy after run #33 wedged
// in deployment_queued (icons never deployed). No functional change.
// Offline-first for the game itself: the first online visit caches every
// file the game needs, and later visits work with no internet at all.
// Bump CACHE (and the ?v= tokens in index.html) on every deploy, so an
// updated worker replaces the old cache instead of serving stale files.
const CACHE = "haus-20261023-31";
const CORE = [
  "./",
  "index.html",
  "style.css?v=20261023-31",
  "game.js?v=20261023-31",
  "manifest.webmanifest",
  "icon-192.png",
  "icon-512.png",
  "icon-512-maskable.png",
  "RULES.md",
  "debug/index.html",
];

self.addEventListener("install", (event) => {
  event.waitUntil(
    caches.open(CACHE).then((cache) => cache.addAll(CORE)).then(() => self.skipWaiting())
  );
});

self.addEventListener("activate", (event) => {
  event.waitUntil(
    caches.keys()
      .then((keys) => Promise.all(keys.filter((k) => k.startsWith("haus-") && k !== CACHE).map((k) => caches.delete(k))))
      .then(() => self.clients.claim())
  );
});

self.addEventListener("fetch", (event) => {
  const req = event.request;
  if (req.method !== "GET") return;
  const url = new URL(req.url);
  if (url.origin !== self.location.origin) return; // only our own files

  // Navigations (the page itself): try the network first so an online
  // visit picks up the newest build; fall back to the cached page offline.
  if (req.mode === "navigate") {
    event.respondWith(
      fetch(req)
        .then((res) => {
          const copy = res.clone();
          caches.open(CACHE).then((cache) => cache.put(req, copy));
          return res;
        })
        .catch(() => {
          // Offline: serve the cached page for this URL — the debug page
          // if that's where the user was headed, else the main game.
          const fallback = url.pathname.includes("/debug/") ? "debug/index.html" : "index.html";
          return caches.match(fallback).then((r) => r || caches.match("./"));
        })
    );
    return;
  }

  // Everything else same-origin: cache first, then network (and cache
  // what the network returns, so a new ?v= asset is kept for next time).
  event.respondWith(
    caches.match(req, { ignoreSearch: false }).then((cached) => {
      if (cached) return cached;
      return fetch(req)
        .then((res) => {
          if (res.ok) {
            const copy = res.clone();
            caches.open(CACHE).then((cache) => cache.put(req, copy));
          }
          return res;
        })
        .catch(() => caches.match(req));
    })
  );
});

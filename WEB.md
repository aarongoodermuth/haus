# Haus — web version

The browser version of Haus lives in [`docs/`](docs/) in this repo.
It's a static site (plain HTML/CSS/JS, no build step) designed to be
played on a phone browser.

- **▶ Play it:** https://aarongoodermuth.github.io/haus/
  (GitHub Pages, deployed from branch `master`, folder `/docs` —
  pushing to `docs/` redeploys automatically.)
- **Play offline (Android):** visit the link once while online, then in
  Chrome tap ⋮ → **Install app** (or **Add to Home screen**). The Haus
  icon appears on the home screen and opens fullscreen like an app — no
  internet needed after that. It works because the game is a PWA: a
  manifest (`docs/manifest.webmanifest`) plus a service worker
  (`docs/sw.js`) cache the game files on first visit. Scores and your
  variant choice already live in localStorage, so they survive offline
  too. When a new build is pushed, the next online visit updates it.
- **Run locally:** `python3 -m http.server 8000 --directory docs`,
  then open `http://localhost:8000` (or your machine's LAN address on a phone).
  Note: service workers need http(s), not `file://` — serve it, don't
  just open `index.html`, if you want to test the offline install.
- **Rules:** [`docs/RULES.md`](docs/RULES.md) — the baseline is the C++
  build in `Haus/`; where they disagree, the C++ rules win.
- **Tests:** `node docs/test.js` runs the headless engine tests.

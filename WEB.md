# Haus — web version

The browser version of Haus lives in [`docs/`](docs/) in this repo.
It's a static site (plain HTML/CSS/JS, no build step) designed to be
played on a phone browser and hosted on GitHub Pages.

- **Play / run locally:** `python3 -m http.server 8000 --directory docs`,
  then open `http://localhost:8000` (or your machine's LAN address on a phone).
- **Hosting:** repo Settings → Pages → deploy from branch `master`,
  folder `/docs`. Note: this repo is private; GitHub Pages for private
  repos requires a GitHub Pro (or team/enterprise) plan.
- **Rules:** [`docs/RULES.md`](docs/RULES.md) — the baseline is the C++
  build in `Haus/`; where they disagree, the C++ rules win.
- **Tests:** `node docs/test.js` runs the headless engine tests.

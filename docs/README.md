# Haus

A browser-based card game based on **Hasenpfeffer** (Hausenpfeffer / Hawsey),
the Euchre-family trick-taking bidding game. Built to play on a phone browser:
no build step, no backend, no accounts — just static files.

## Play it locally

```sh
python3 -m http.server 8000
# then open http://localhost:8000 on this machine,
# or http://<this-machine's-LAN-address>:8000 on your phone (same Wi-Fi)
```

Or just open `index.html` in a browser.

## Hosting (minimal traffic)

This project lives in the existing **`aarongoodermuth/haus`** repo under
`docs/`, hosted on **GitHub Pages**: https://aarongoodermuth.github.io/haus/
Aaron made the repo public on 2026-10-23 so Pages works on the free plan;
Pages deploys from branch `master`, folder `/docs`.

To run it straight from a clone instead:

```sh
python3 -m http.server 8000 --directory docs
```

Other fine options: Cloudflare Pages, Netlify, or any web server that can
serve three files.

## Layout

| File | What it is |
| --- | --- |
| `index.html` | Page shell, mobile viewport, table layout |
| `style.css` | Felt-table styling, touch-sized cards (≥48px targets) |
| `game.js` | Game engine + UI. Engine helpers are pure and Node-testable |
| `test.js` | Headless engine tests: `node test.js` |
| `RULES.md` | Draft rules (ported from the C++ build) + open questions |

## Status

v0.1 prototype: single player vs 3 simple bots, full hand flow
(deal → bid → name trump → 6 tricks → scoring), first to 31.

## Bot play strategy (build .13)

The old bots led a middling card and only trumped when last to act. The
new `botPlay` thinks a little: lead from strength (off-suit Ace, then
bars, then the top card of your longest suit); follow by winning as
cheaply as possible; save trump unless the win is guaranteed (last to
act), denies the bidder a trick, or you're the bidder with a live bid;
when you can't win, shed the lowest non-trump card (preferring to open
a void). No partnerships — everyone scores alone — so there is no
partner logic. Covered by unit tests in `test.js`.
Rules baseline is the C++ build in this repo's `Haus/` folder — where the
two disagree, the C++ rules win (Aaron, 2026-10-23); see `RULES.md`.
Multiplayer is out of scope for a pure static site; if it's wanted later,
that's a small backend or a peer-to-peer step to design separately.

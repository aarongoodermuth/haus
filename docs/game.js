"use strict";
/*
 * Haus — browser Hasenpfeffer (prototype v0.1)
 * Baseline rules are ported from Aaron's C++ `haus` repo (see RULES.md).
 * Pure engine helpers are exported for Node tests when available.
 */

const SUITS = [
  { id: "hearts",   symbol: "♥", color: "red",   sameColor: "diamonds" },
  { id: "diamonds", symbol: "♦", color: "red",   sameColor: "hearts" },
  { id: "clubs",    symbol: "♣", color: "black", sameColor: "spades" },
  { id: "spades",   symbol: "♠", color: "black", sameColor: "clubs" },
];
const SUIT_BY_ID = Object.fromEntries(SUITS.map(s => [s.id, s]));
const VALUES = ["9", "10", "J", "Q", "K", "A"]; // index = rank 0..5
const BIDS = [
  { id: "pass",  label: "Pass", amount: 0,  required: 0 },
  { id: "3",     label: "3",    amount: 3,  required: 3 },
  { id: "4",     label: "4",    amount: 4,  required: 4 },
  { id: "5",     label: "5",    amount: 5,  required: 5 },
  { id: "6",     label: "6",    amount: 6,  required: 6 },
  { id: "haus",  label: "Haus!", amount: 12, required: 6 },
];
const WIN_SCORE = 31;
const CARDS_PER_PLAYER = 6;

function buildDeck() {
  const deck = [];
  for (const s of SUITS) for (let v = 0; v < VALUES.length; v++) deck.push({ suit: s.id, value: v });
  return deck;
}
function shuffle(deck, rng = Math.random) {
  const d = deck.slice();
  for (let i = d.length - 1; i > 0; i--) { const j = Math.floor(rng() * (i + 1)); [d[i], d[j]] = [d[j], d[i]]; }
  return d;
}
// Trump is null before bidding finishes (deal / bidding phases), and card
// rendering calls these helpers for every card — they must not throw then.
// The old isLeftBower dereferenced SUIT_BY_ID[null].sameColor on any Jack,
// which crashed render() during the deal, truncating the hand on screen
// and preventing bidding from ever starting.
function isRightBower(card, trump) { return !!trump && card.value === 2 && card.suit === trump; }
function isLeftBower(card, trump) { return !!trump && card.value === 2 && card.suit === SUIT_BY_ID[trump].sameColor; }
function isTrump(card, trump) { return !!trump && (card.suit === trump || isLeftBower(card, trump)); }
function effectiveSuit(card, trump) { return isTrump(card, trump) ? trump : card.suit; }
function cardStrength(card, trump, ledSuit) {
  // Comparable within a trick: trump beats led suit beats off-suit.
  if (isTrump(card, trump)) {
    if (isRightBower(card, trump)) return 100 + 7;
    if (isLeftBower(card, trump)) return 100 + 6;
    return 100 + card.value; // trump A(5) > K(4) > Q(3) > 10(1) > 9(0); J is bar above
  }
  if (card.suit === ledSuit) return 50 + card.value;
  return card.value;
}
function legalCards(hand, trump, ledCard) {
  // A bar is trump for EVERY purpose, including following suit — this
  // deliberately departs from C++ Rules::FLegalPlay, which judged by printed
  // suit (Aaron's correction, 2026-10-24). So with clubs trump and spades
  // led, the J♠ is a club, not a spade: a hand of J♠ + no other spades is
  // void in spades and may play anything. Conversely, when trump is led,
  // the left bar must follow it like any other trump card.
  if (!ledCard) return hand.slice();
  const ledSuit = effectiveSuit(ledCard, trump);
  const follow = hand.filter(c => effectiveSuit(c, trump) === ledSuit);
  return follow.length ? follow : hand.slice();
}
function trickWinner(plays, trump) { // plays: [{player, card}] in play order
  const led = effectiveSuit(plays[0].card, trump);
  let best = plays[0];
  for (const p of plays.slice(1)) if (cardStrength(p.card, trump, led) > cardStrength(best.card, trump, led)) best = p;
  return best.player;
}
function scoreHand(players, bidderIdx, bid) {
  // Ported from C++ Player::DoScoring: scoring is per-player in v0.1 (see RULES.md Q1).
  // Bidder: made it -> max(tricks, bid amount); missed -> -bid amount.
  // Everyone else: +1 per trick they personally won.
  players.forEach((p, i) => {
    if (i === bidderIdx) {
      if (p.tricks >= bid.required) p.score += Math.max(p.tricks, bid.amount);
      else p.score -= bid.amount;
    } else {
      p.score += p.tricks;
    }
  });
}
function estimateBid(hand) { // expected tricks at each candidate trump suit
  let best = { suit: null, est: 0 };
  for (const s of SUITS) {
    let est = 0;
    for (const c of hand) {
      if (isRightBower(c, s.id)) est += 1;
      else if (isLeftBower(c, s.id)) est += 0.9;
      else if (c.suit === s.id && c.value === 5) est += 0.9;      // trump ace
      else if (c.suit === s.id && c.value >= 3) est += 0.45;     // trump K/Q
      else if (c.suit === s.id) est += 0.25;                    // small trump
      else if (c.value === 5) est += 0.75;                      // off-suit ace
    }
    // Long trump wins late tricks once the suit is drawn out, and a void
    // plus trump means trumping into that suit — both worth a little extra.
    const trumpCount = hand.filter(c => isTrump(c, s.id)).length;
    if (trumpCount >= 4) est += 0.15 * (trumpCount - 3);
    if (trumpCount >= 2) {
      const held = new Set(hand.map(c => effectiveSuit(c, s.id)));
      for (const t of SUITS) if (t.id !== s.id && !held.has(t.id)) est += 0.2;
    }
    if (est > best.est) best = { suit: s.id, est };
  }
  return best;
}
function isHausLock(hand, suit) {
  // Near-certain all 6 tricks: both bars, and every other card a winner
  // once trump is drawn out — all 6 trump, 5 trump + an off-suit ace, or
  // 4 trump + 2 off-suit aces. (A 5-trump hand with a weak 6th card is NOT
  // a lock: after trump is drawn, a higher card of that suit still beats it.)
  const trump = hand.filter(c => isTrump(c, suit));
  const offAces = hand.filter(c => !isTrump(c, suit) && c.value === 5).length;
  return hand.some(c => isRightBower(c, suit)) && hand.some(c => isLeftBower(c, suit)) &&
    (trump.length >= 6 || (trump.length === 5 && offAces >= 1) || (trump.length === 4 && offAces === 2));
}
function botBid(hand, currentBid) {
  const { est, suit } = estimateBid(hand);
  // Bidding is a double-edged sword: missing costs the bid, but winning
  // the bid names trump — and est is already computed at your best suit,
  // so the trump-control value is baked into the number. On top of that,
  // passing lets someone else name trump, which can only hurt you, so the
  // bid decision gets a small control premium: bid 3 from est 2.75 up.
  const haus = BIDS.find(b => b.id === "haus");
  if (isHausLock(hand, suit) && haus.amount > currentBid.amount) return haus;
  const want = Math.floor(est + 0.25);
  const bid = BIDS.find(b => b.id === String(want));
  if (bid && bid.amount > currentBid.amount) return bid;
  return BIDS[0];
}
function botChooseTrump(hand) { return estimateBid(hand).suit || "spades"; }
function leadCard(hand, trump, sweep = false) {
  // Lead from strength: a trick in hand is worth the same whenever you take
  // it, so open with a card that is hard to beat, not a middling one.
  const offAces = hand.filter(c => c.value === 5 && !isTrump(c, trump));
  // Sweeping for Haus: an off-suit Ace led while opponents still hold trump
  // can be trumped, so hold the aces until your trump leads have drawn
  // their trump out. Once you're out of trump yourself, aces lead again.
  const hasTrump = hand.some(c => isTrump(c, trump));
  if (offAces.length && !(sweep && hasTrump)) return offAces[0]; // off-suit Ace: wins unless trumped
  const right = hand.find(c => isRightBower(c, trump));
  if (right) return right; // nothing beats the right bar
  const left = hand.find(c => isLeftBower(c, trump));
  if (left) return left; // only the right bar beats it
  // Otherwise lead the top card of your longest suit (ties: higher top card,
  // then a non-trump suit) — long suits are where future tricks live.
  const bySuit = {};
  for (const c of hand) (bySuit[effectiveSuit(c, trump)] ||= []).push(c);
  const suits = Object.keys(bySuit).sort((a, b) =>
    bySuit[b].length - bySuit[a].length ||
    Math.max(...bySuit[b].map(c => c.value)) - Math.max(...bySuit[a].map(c => c.value)) ||
    (a === trump ? 1 : 0) - (b === trump ? 1 : 0));
  const suitCards = bySuit[suits[0]];
  return suitCards.reduce((best, c) => c.value > best.value ? c : best, suitCards[0]);
}
function discardCard(hand, trump, legal) {
  // Shedding (void in the led suit, no win available): lose as little as
  // possible. Never throw away trump while a non-trump card exists; among
  // equals, shed from a singleton suit to open a void for later trumping.
  const suitCount = c => hand.filter(x => x.suit === c.suit).length;
  return legal.slice().sort((a, b) =>
    (isTrump(a, trump) - isTrump(b, trump)) ||
    a.value - b.value ||
    suitCount(a) - suitCount(b))[0];
}
function botPlay(hand, trump, plays, ctx = {}) {
  // ctx (from the UI; optional in tests): { self, bidder, bidderLive, bidId }.
  // bidderLive = the bidder can still make the bid, so winning matters more
  // to them than saving trump. No partnerships here — every player scores
  // alone — so there is no "help your partner" logic, only win/deny/preserve.
  const ledCard = plays.length ? plays[0].card : null;
  // Sweeping = the bidder is going for Haus and the bid is still alive:
  // lead trump first so the off-suit aces can't be trumped (see leadCard).
  const sweeping = ctx.self === ctx.bidder && ctx.bidderLive && ctx.bidId === "haus";
  if (!ledCard) return leadCard(hand, trump, sweeping);
  const legal = legalCards(hand, trump, ledCard);
  const led = effectiveSuit(ledCard, trump);
  const sorted = legal.slice().sort((a, b) => cardStrength(a, trump, led) - cardStrength(b, trump, led));
  const currentBest = Math.max(...plays.map(p => cardStrength(p.card, trump, led)));
  const currentWinner = plays.reduce((best, p) =>
    cardStrength(p.card, trump, led) > cardStrength(best.card, trump, led) ? p : best, plays[0]);
  const winning = sorted.filter(c => cardStrength(c, trump, led) > currentBest);
  if (winning.length) {
    const cheapest = winning[0];
    if (!isTrump(cheapest, trump)) return cheapest; // win without spending trump
    // Winning costs trump. If every legal card is trump, the trump is spent
    // either way, so take the trick. Otherwise it's worth it only when the
    // win is guaranteed (last to act), when it denies the bidder a trick,
    // or when you are the bidder and the bid is still alive — else save
    // the trump and shed a cheap card instead.
    const canShed = legal.some(c => !isTrump(c, trump));
    const lastToAct = plays.length === 3;
    const deniesBidder = ctx.bidder != null && currentWinner.player === ctx.bidder && ctx.self !== ctx.bidder;
    if (!canShed || lastToAct || deniesBidder || ctx.bidderLive) return cheapest;
    return discardCard(hand, trump, legal);
  }
  // Can't win this trick: lose as cheaply as possible (discardCard picks the
  // lowest legal card, which is the lowest of the led suit when following).
  return discardCard(hand, trump, legal);
}

/* ---------------- Browser UI (skipped in Node tests) ---------------- */
if (typeof document !== "undefined") {
  const NAMES = ["You", "Jarvis", "Karen", "Bender"]; // bots nod to the C++ build
  const state = { players: [], dealer: 0, phase: "idle", trump: null, bidder: -1, bid: BIDS[0], plays: [], turn: 0, leader: 0, trickWinner: -1 };
  // Debug options: the debug site lives in the /debug/ subdirectory and
  // shares this exact file, so debug behavior is keyed off the page path
  // and the main site is untouched. Each option is its own flag, so more
  // can be added without touching the game logic.
  const DEBUG = {
    showHands: typeof location !== "undefined" && location.pathname.includes("/debug/"),
  };
  const $ = id => document.getElementById(id);
  const cardLabel = c => VALUES[c.value] + SUIT_BY_ID[c.suit].symbol;
  const cardEl = (c, opts = {}) => {
    const el = document.createElement(opts.button ? "button" : "div");
    el.className = "card" + (SUIT_BY_ID[c.suit].color === "red" ? " red" : "") + (opts.small ? " small" : "") + (state.trump && isTrump(c, state.trump) ? " trump" : "") + (opts.winner ? " winner" : "");
    el.innerHTML = `${cardLabel(c)}<span class="sub">${isRightBower(c, state.trump) ? "right bar" : isLeftBower(c, state.trump) ? "left bar" : ""}</span>`;
    if (opts.button) { el.type = "button"; el.disabled = !!opts.disabled; el.addEventListener("click", () => playHumanCard(c)); }
    return el;
  };
  function msg(t) { $("message").textContent = t || ""; }
  function render() {
    $("scoreboard").innerHTML = state.players.map((p, i) =>
      `<span class="chip">${i === state.dealer ? "🂠 " : ""}${p.name}: <strong>${p.score}</strong> · ${p.tricks} ${p.tricks === 1 ? "trick" : "tricks"}</span>`).join("");
    $("status").textContent = state.trump
      ? `Trump: ${SUIT_BY_ID[state.trump].symbol} · Bid: ${state.bid.label} by ${state.players[state.bidder]?.name || "—"}`
      : "Bidding / dealing";
    // Bidding stage only: who dealt, who is currently under for the default
    // 3 (C++ DoBet: the dealer holds a standing 3 until someone bids higher),
    // and every bid made so far, so your turn shows what came before it.
    const bidInfo = $("bid-info");
    if (state.phase === "bid" && state.players.length) {
      const dealerName = state.players[state.dealer].name;
      const underNote = state.bids.every(b => b.label === "Pass")
        ? ` · ${dealerName} is under for 3 unless someone bids higher`
        : "";
      bidInfo.innerHTML = `<div>Dealer: ${dealerName}${underNote}</div>` +
        (state.bids.length
          ? `<div>Bids so far: ${state.bids.map(b => `${state.players[b.player].name} — ${b.label}`).join(" · ")}</div>`
          : "");
    } else bidInfo.innerHTML = "";
    state.players.forEach((p, i) => {
      $("name-" + i).textContent = (state.trickWinner === i && state.plays.length === 4 ? "🏆 " : "") + p.name;
      const slot = $("played-" + i); slot.innerHTML = "";
      const play = state.plays.find(x => x.player === i);
      if (play) slot.appendChild(cardEl(play.card, { small: true, winner: state.trickWinner === i && state.plays.length === 4 }));
      // Debug: opponents' whole hands face up under their seats. Hands are
      // already sorted at deal, so they render as dealt.
      if (DEBUG.showHands && i > 0) {
        const handSlot = $("hand-" + i);
        if (handSlot) {
          handSlot.innerHTML = "";
          p.hand.forEach(c => handSlot.appendChild(cardEl(c, { small: true })));
        }
      }
    });
    // The center stays quiet: the trick itself is shown by the cards in the
    // seats, the winner by the trophy by their name + the gold card, and
    // the Deal button speaks for itself. The only thing the center still
    // says is who leads, which appears nowhere else.
    const center = $("center-info");
    center.textContent = (!state.plays.length && state.trump) ? `${state.players[state.leader].name} to lead` : "";
    const handEl = $("hand");
    if (handDrag && handDrag.active) return; // don't rebuild the hand mid-drag (bot timers re-render)
    handEl.innerHTML = "";
    const me = state.players[0]; if (!me) return;
    const ledCard = state.plays.length ? state.plays[0].card : null;
    const playable = state.phase === "play" && state.turn === 0;
    const legal = playable ? legalCards(me.hand, state.trump, ledCard) : [];
    me.hand.forEach(c => {
      const el = cardEl(c, {});
      el._card = c;
      if (playable && !legal.includes(c)) el.className += " disabled";
      attachHandCard(el, c, playable && legal.includes(c));
      handEl.appendChild(el);
    });
  }
  // Drag-to-reorder for the player's hand, built on Pointer Events so mouse
  // and touch share one path. A press that barely moves is a tap (plays the
  // card when it's your turn); moving past the threshold starts a drag, and
  // the other cards shift live. The new order is committed to the hand array
  // on release, so it persists across renders.
  let handDrag = null;
  function attachHandCard(el, card, canPlay) {
    el.addEventListener("pointerdown", e => {
      handDrag = { el, startX: e.clientX, startY: e.clientY, active: false, grabDX: 0, grabDY: 0, placeholder: null };
      if (el.setPointerCapture) { try { el.setPointerCapture(e.pointerId); } catch (_) {} }
    });
    el.addEventListener("pointermove", e => {
      if (!handDrag || handDrag.el !== el) return;
      const dx = e.clientX - handDrag.startX, dy = e.clientY - handDrag.startY;
      if (!handDrag.active) {
        if (Math.hypot(dx, dy) <= 10) return;
        // Lift off: the card leaves the layout and floats, following the pointer
        // from the exact spot it was grabbed. A placeholder holds its slot and
        // shows where the drop will land. Only the OTHER cards' positions are
        // used to place that slot, so it can't jitter the way v1 did.
        const r = el.getBoundingClientRect();
        handDrag.active = true;
        handDrag.grabDX = handDrag.startX - r.left; handDrag.grabDY = handDrag.startY - r.top; // grab point from the original press, so tracking is exact from the first move
        const ph = document.createElement("div");
        ph.className = "card placeholder";
        ph.style.width = r.width + "px"; ph.style.height = r.height + "px";
        handDrag.placeholder = ph;
        $("hand").insertBefore(ph, el);
        el.className += " dragging";
        el.style.width = r.width + "px"; el.style.height = r.height + "px";
        el.style.left = r.left + "px"; el.style.top = r.top + "px";
      }
      if (e.preventDefault) e.preventDefault();
      el.style.left = (e.clientX - handDrag.grabDX) + "px";
      el.style.top = (e.clientY - handDrag.grabDY) + "px";
      // Work out the insertion index from the pointer against the stationary
      // slots, scanning in DOM order (which is row-by-row on screen): the
      // drop slot is before the first card that sits below the pointer, or
      // shares its row and sits to its right.
      const handEl = $("hand");
      const slots = Array.from(handEl.children).filter(x => x !== el && x !== handDrag.placeholder);
      let idx = slots.length;
      for (let i = 0; i < slots.length; i++) {
        const r = slots[i].getBoundingClientRect();
        if (e.clientY < r.top) { idx = i; break; }
        if (e.clientY <= r.bottom && e.clientX < r.left + r.width / 2) { idx = i; break; }
        idx = i + 1;
      }
      const ref = idx < slots.length ? slots[idx] : null;
      if (ref) handEl.insertBefore(handDrag.placeholder, ref); else handEl.appendChild(handDrag.placeholder);
    }, { passive: false });
    const settleDrag = () => {
      const handEl = $("hand");
      handEl.insertBefore(el, handDrag.placeholder);
      handDrag.placeholder.remove();
      el.className = el.className.replace(" dragging", "");
      el.style.width = ""; el.style.height = ""; el.style.left = ""; el.style.top = "";
      const order = Array.from(handEl.children).map(x => x._card);
      if (order.length === state.players[0].hand.length) state.players[0].hand = order;
      handDrag = null;
      render();
    };
    el.addEventListener("pointerup", () => {
      if (!handDrag || handDrag.el !== el) return;
      if (handDrag.active) return settleDrag();
      handDrag = null;
      if (canPlay) playHumanCard(card); // it was a tap, not a drag
    });
    el.addEventListener("pointercancel", () => {
      if (!handDrag || handDrag.el !== el) return;
      if (handDrag.active) settleDrag(); else handDrag = null;
    });
  }
  // Long-press on a card must not pop a context menu mid-drag.
  $("hand").addEventListener("contextmenu", e => { if (e.preventDefault) e.preventDefault(); });
  function newGame() {
    state.players = NAMES.map(name => ({ name, score: 0, tricks: 0, hand: [] }));
    state.dealer = Math.floor(Math.random() * 4);
    startHand();
  }
  function hideMainBtn() { $("main-btn").style.display = "none"; }
  function showMainBtn() { $("main-btn").style.display = "block"; }
  function startHand() {
    const deck = shuffle(buildDeck());
    state.players.forEach(p => { p.tricks = 0; p.hand = []; });
    for (let r = 0; r < CARDS_PER_PLAYER; r++) for (let i = 0; i < 4; i++) state.players[(state.dealer + 1 + i) % 4].hand.push(deck.pop());
    // Sort once, here at the deal. From then on the array order IS the display
    // order, so the player can drag cards into their own order and it sticks.
    state.players.forEach(p => p.hand.sort((a, b) => a.suit.localeCompare(b.suit) || b.value - a.value));
    state.trump = null; state.plays = []; state.trickWinner = -1; state.phase = "bid";
    // Aaron's rule, overriding C++ DoBet: the dealer bids LAST, not first.
    // Bidding starts left of the dealer; there is no standing bid, so the
    // first bid only has to beat pass. The dealer is under for 3 in the
    // sense that if nobody bids, the dealer cannot pass and must bid 3+.
    state.bid = BIDS[0]; state.bidder = -1; state.bids = [];
    state.bidTurn = (state.dealer + 1) % 4; state.bidsSeen = 0;
    msg(`Hand dealt. Dealer: ${state.players[state.dealer].name} — bidding starts with ${state.players[state.bidTurn].name}, dealer bids last.`);
    hideMainBtn(); // the Deal button is done its job — it stays out of the way until the hand ends
    render(); nextBid();
  }
  function nextBid() {
    if (state.bidsSeen >= 4) return finishBidding();
    const i = state.bidTurn;
    if (i === 0) {
      const panel = $("panel"); panel.innerHTML = "";
      BIDS.forEach(b => {
        // Same-or-lower bids are never offered: only bids beating the
        // current high bid appear.
        if (b.id !== "pass" && b.amount <= state.bid.amount) return;
        // The dealer bids last: if nobody has bid yet, the dealer cannot
        // pass — the dealer must bid at least 3 (the "under for 3" case).
        if (b.id === "pass" && state.dealer === 0 && state.bidder === -1) return;
        const btn = document.createElement("button"); btn.type = "button";
        btn.textContent = b.id === "pass" ? "Pass" : "Bid " + b.label;
        if (b.id === "pass") btn.className = "secondary";
        btn.addEventListener("click", () => { panel.innerHTML = ""; acceptBid(0, b); });
        panel.appendChild(btn);
      });
      // Same condition that hides the Pass button: a dealer facing an
      // empty round can't pass, so the prompt must not offer it.
      msg(state.dealer === 0 && state.bidder === -1
        ? "Your bid — you can't pass, nobody has bid yet."
        : "Your bid — or pass.");
    } else {
      setTimeout(() => {
        let b = botBid(state.players[i].hand, state.bid);
        // The dealer-last rule applies to bots too: a dealer facing an
        // empty bidding round cannot pass, so a passing bot dealer bids 3.
        if (i === state.dealer && state.bidder === -1 && b.id === "pass") b = BIDS.find(x => x.id === "3");
        acceptBid(i, b);
      }, 450);
    }
    render();
  }
  function acceptBid(i, bid) {
    state.bids.push({ player: i, label: bid.id === "pass" ? "Pass" : bid.label });
    if (bid.amount > state.bid.amount) { state.bid = bid; state.bidder = i; msg(`${state.players[i].name} bids ${bid.label}.`); }
    else msg(`${state.players[i].name} passes.`);
    state.bidsSeen++; state.bidTurn = (state.bidTurn + 1) % 4; nextBid();
  }
  function finishBidding() {
    // Safety net (unreachable now that the dealer can't pass an empty
    // round): if somehow nobody bid, the dealer takes 3.
    if (state.bidder === -1) { state.bidder = state.dealer; state.bid = BIDS.find(b => b.id === "3"); }
    // Under for 3: the dealer bid the minimum 3 because nobody else bid.
    state.bidForced = state.bidder === state.dealer && state.bid.amount === 3 &&
      state.bids.filter(b => b.player !== state.dealer).every(b => b.label === "Pass");
    if (state.bidForced) {
      msg(`Everyone passed — dealer ${state.players[state.dealer].name} is under for 3.`);
    }
    if (state.bidder === 0) {
      const panel = $("panel"); panel.innerHTML = "";
      SUITS.forEach(s => {
        const btn = document.createElement("button"); btn.type = "button"; btn.textContent = s.symbol + " " + s.id;
        btn.addEventListener("click", () => { panel.innerHTML = ""; setTrump(s.id); });
        panel.appendChild(btn);
      });
      msg(state.bidForced
        ? `You're under for 3 — pick trump:`
        : `You won the bid at ${state.bid.label}. Pick trump:`);
    } else {
      setTimeout(() => setTrump(botChooseTrump(state.players[state.bidder].hand)), 500);
    }
    render();
  }
  function setTrump(suit) {
    state.trump = suit; state.phase = "play"; state.leader = state.bidder; state.turn = state.bidder; state.plays = [];
    msg(`Trump is ${suit}. ${state.players[state.bidder].name} leads.`);
    render(); nextPlay();
  }
  function nextPlay() {
    render();
    if (state.turn === 0) { msg("Your turn — tap a card."); return; }
    setTimeout(() => {
      const p = state.players[state.turn];
      const card = botPlay(p.hand, state.trump, state.plays, {
        self: state.turn,
        bidder: state.bidder,
        // Bidder's bid is still alive if the tricks they hold plus the tricks
        // left (one per card still in hand) can still reach the requirement.
        bidderLive: state.turn === state.bidder && p.tricks + p.hand.length >= state.bid.required,
        bidId: state.bid.id,
      });
      doPlay(state.turn, card);
    }, 550);
  }
  function pauseAfterTrick(winner) {
    // Leave the completed trick on the table until the player is ready —
    // no auto-advance. The button moves to the next trick, or, after the
    // last trick, to the hand summary (which itself waits on "Next Hand").
    state.phase = "trickEnd";
    state.trickWinner = winner;
    const lastTrick = state.players[0].hand.length === 0;
    msg(`🏆 ${state.players[winner].name} won the trick. ${lastTrick ? "That was the last one." : ""}`);
    const panel = $("panel"); panel.innerHTML = "";
    const btn = document.createElement("button"); btn.type = "button";
    btn.textContent = lastTrick ? "Finish Hand ▸" : "Next Trick ▸";
    btn.addEventListener("click", () => {
      panel.innerHTML = "";
      if (lastTrick) return endHand(); // keep the final trick (and its winner highlight) under the summary
      state.plays = []; state.trickWinner = -1;
      state.phase = "play"; state.turn = winner; render(); nextPlay();
    });
    panel.appendChild(btn);
    render();
  }
  function playHumanCard(card) {
    if (state.phase !== "play" || state.turn !== 0) return;
    doPlay(0, card);
  }
  function doPlay(i, card) {
    const p = state.players[i];
    p.hand = p.hand.filter(c => c !== card);
    state.plays.push({ player: i, card });
    if (state.plays.length === 4) {
      const winner = trickWinner(state.plays, state.trump);
      state.players[winner].tricks++;
      state.leader = winner;
      pauseAfterTrick(winner);
    } else {
      state.turn = (i + 1) % 4; render(); nextPlay();
    }
    render();
  }
  function endHand() {
    const bid = state.bid, bidder = state.bidder;
    scoreHand(state.players, bidder, bid);
    const made = state.players[bidder].tricks >= bid.required;
    const top = state.players.slice().sort((a, b) => b.score - a.score)[0]; // C++ Game: winner is the high scorer
    const winner = top.score >= WIN_SCORE ? top : null;
    msg(`Hand over: ${state.players[bidder].name} bid ${bid.label}, took ${state.players[bidder].tricks} — ${made ? "made it" : "set!"}` +
        (winner ? ` 🏆 ${winner.name} wins the game with ${winner.score}!` : " Tap Next Hand."));
    showMainBtn();
    $("main-btn").textContent = winner ? "New Game" : "Next Hand";
    $("main-btn").onclick = winner ? newGame : () => { state.dealer = (state.dealer + 1) % 4; startHand(); };
    state.phase = "idle"; render();
  }
  $("main-btn").addEventListener("click", newGame, { once: true });
  state.players = NAMES.map(name => ({ name, score: 0, tricks: 0, hand: [] }));
  render();
}

if (typeof module !== "undefined") module.exports = { SUITS, VALUES, BIDS, WIN_SCORE, buildDeck, shuffle, isTrump, isLeftBower, isRightBower, effectiveSuit, legalCards, trickWinner, cardStrength, scoreHand, estimateBid, botBid, botPlay };

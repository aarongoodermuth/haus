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
    return 100 + card.value; // trump A(5) > K(4) > Q(3) > 10(1) > 9(0); J is bower above
  }
  if (card.suit === ledSuit) return 50 + card.value;
  return card.value;
}
function legalCards(hand, trump, ledCard) {
  // C++ Rules::FLegalPlay, faithfully: following suit is judged by the card's
  // PRINTED suit. The left bower still counts as trump for winning a trick,
  // but it does not satisfy (or require) following trump, and it must be
  // played when its printed suit is led and it's your only card of that suit.
  if (!ledCard) return hand.slice();
  const follow = hand.filter(c => c.suit === ledCard.suit);
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
function estimateBid(hand) { // simple bot heuristic
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
    if (est > best.est) best = { suit: s.id, est };
  }
  return best;
}
function botBid(hand, currentBid) {
  const { est } = estimateBid(hand);
  if (est >= 5.6) return BIDS.find(b => b.id === "haus");
  const want = Math.floor(est);
  const bid = BIDS.find(b => b.id === String(want));
  if (bid && bid.amount > currentBid.amount && want >= 4) return bid;
  return BIDS[0];
}
function botChooseTrump(hand) { return estimateBid(hand).suit || "spades"; }
function botPlay(hand, trump, plays) {
  const ledCard = plays.length ? plays[0].card : null;
  const legal = legalCards(hand, trump, ledCard);
  const led = ledCard ? effectiveSuit(ledCard, trump) : null;
  const sorted = legal.slice().sort((a, b) => cardStrength(a, trump, led) - cardStrength(b, trump, led));
  if (!ledCard) return sorted[Math.floor(sorted.length / 2)]; // lead a middling card
  const currentBest = Math.max(...plays.map(p => cardStrength(p.card, trump, led)));
  const winning = sorted.filter(c => cardStrength(c, trump, led) > currentBest);
  // Win as cheaply as possible if a win is cheap; otherwise shed the lowest card.
  if (winning.length && cardStrength(winning[0], trump, led) < 100) return winning[0];
  if (winning.length && plays.length === 3) return winning[0]; // last to act: take it
  return sorted[0];
}

/* ---------------- Browser UI (skipped in Node tests) ---------------- */
if (typeof document !== "undefined") {
  const NAMES = ["You", "Jarvis", "Karen", "Bender"]; // bots nod to the C++ build
  const state = { players: [], dealer: 0, phase: "idle", trump: null, bidder: -1, bid: BIDS[0], plays: [], turn: 0, leader: 0, trickWinner: -1 };
  const $ = id => document.getElementById(id);
  const cardLabel = c => VALUES[c.value] + SUIT_BY_ID[c.suit].symbol;
  const cardEl = (c, opts = {}) => {
    const el = document.createElement(opts.button ? "button" : "div");
    el.className = "card" + (SUIT_BY_ID[c.suit].color === "red" ? " red" : "") + (opts.small ? " small" : "") + (state.trump && isTrump(c, state.trump) ? " trump" : "") + (opts.winner ? " winner" : "");
    el.innerHTML = `${cardLabel(c)}<span class="sub">${isRightBower(c, state.trump) ? "right bower" : isLeftBower(c, state.trump) ? "left bower" : ""}</span>`;
    if (opts.button) { el.type = "button"; el.disabled = !!opts.disabled; el.addEventListener("click", () => playHumanCard(c)); }
    return el;
  };
  function msg(t) { $("message").textContent = t || ""; }
  function render() {
    $("scoreboard").innerHTML = state.players.map((p, i) =>
      `<span class="chip">${i === state.dealer ? "🂠 " : ""}${p.name}: <strong>${p.score}</strong> · ${p.tricks} trk</span>`).join("");
    $("status").textContent = state.trump
      ? `Trump: ${SUIT_BY_ID[state.trump].symbol} · Bid: ${state.bid.label} by ${state.players[state.bidder]?.name || "—"}`
      : "Bidding / dealing";
    state.players.forEach((p, i) => {
      $("name-" + i).textContent = (state.trickWinner === i && state.plays.length === 4 ? "🏆 " : "") + p.name;
      $("count-" + i).textContent = p.hand.length + " cards";
      const slot = $("played-" + i); slot.innerHTML = "";
      const play = state.plays.find(x => x.player === i);
      if (play) slot.appendChild(cardEl(play.card, { small: true, winner: state.trickWinner === i && state.plays.length === 4 }));
    });
    const center = $("center-info");
    center.textContent = state.plays.length
      ? "Trick: " + state.plays.map(x => `${state.players[x.player].name} ${cardLabel(x.card)}`).join(" · ") +
        (state.trickWinner >= 0 && state.plays.length === 4 ? ` — 🏆 ${state.players[state.trickWinner].name} won the trick` : "")
      : (state.trump ? `${state.players[state.leader].name} to lead` : "Tap “Deal” to start");
    const handEl = $("hand"); handEl.innerHTML = "";
    const me = state.players[0]; if (!me) return;
    const ledCard = state.plays.length ? state.plays[0].card : null;
    const legal = state.phase === "play" && state.turn === 0 ? legalCards(me.hand, state.trump, ledCard) : [];
    me.hand.slice().sort((a, b) => a.suit.localeCompare(b.suit) || b.value - a.value)
      .forEach(c => handEl.appendChild(cardEl(c, { button: state.phase === "play" && state.turn === 0, disabled: !legal.includes(c) })));
  }
  function newGame() {
    state.players = NAMES.map(name => ({ name, score: 0, tricks: 0, hand: [] }));
    state.dealer = Math.floor(Math.random() * 4);
    startHand();
  }
  function startHand() {
    const deck = shuffle(buildDeck());
    state.players.forEach(p => { p.tricks = 0; p.hand = []; });
    for (let r = 0; r < CARDS_PER_PLAYER; r++) for (let i = 0; i < 4; i++) state.players[(state.dealer + 1 + i) % 4].hand.push(deck.pop());
    state.trump = null; state.plays = []; state.trickWinner = -1; state.bid = BIDS[0]; state.bidder = -1; state.phase = "bid";
    state.bidTurn = state.dealer; state.bidsSeen = 0; // C++ Round::DoBet starts bidding at the dealer
    msg(`Hand dealt. Dealer: ${state.players[state.dealer].name} — bidding starts with the dealer.`);
    render(); nextBid();
  }
  function nextBid() {
    if (state.bidsSeen >= 4) return finishBidding();
    const i = state.bidTurn;
    if (i === 0) {
      const panel = $("panel"); panel.innerHTML = "";
      BIDS.forEach(b => {
        if (b.id !== "pass" && b.amount <= state.bid.amount) return;
        const btn = document.createElement("button"); btn.type = "button"; btn.textContent = b.id === "pass" ? "Pass" : "Bid " + b.label;
        if (b.id === "pass") btn.className = "secondary";
        btn.addEventListener("click", () => { panel.innerHTML = ""; acceptBid(0, b); });
        panel.appendChild(btn);
      });
      msg("Your bid — or pass.");
    } else {
      setTimeout(() => acceptBid(i, botBid(state.players[i].hand, state.bid)), 450);
    }
    render();
  }
  function acceptBid(i, bid) {
    if (bid.amount > state.bid.amount) { state.bid = bid; state.bidder = i; msg(`${state.players[i].name} bids ${bid.label}.`); }
    else msg(`${state.players[i].name} passes.`);
    state.bidsSeen++; state.bidTurn = (state.bidTurn + 1) % 4; nextBid();
  }
  function finishBidding() {
    if (state.bidder === -1) { // all passed: dealer is forced at 3 ("screw the dealer")
      state.bidder = state.dealer; state.bid = BIDS.find(b => b.id === "3");
      msg(`Everyone passed — dealer ${state.players[state.dealer].name} is stuck with a bid of 3.`);
    }
    if (state.bidder === 0) {
      const panel = $("panel"); panel.innerHTML = "";
      SUITS.forEach(s => {
        const btn = document.createElement("button"); btn.type = "button"; btn.textContent = s.symbol + " " + s.id;
        btn.addEventListener("click", () => { panel.innerHTML = ""; setTrump(s.id); });
        panel.appendChild(btn);
      });
      msg(`You won the bid at ${state.bid.label}. Pick trump:`);
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
      const card = botPlay(p.hand, state.trump, state.plays);
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
    $("main-btn").textContent = winner ? "New Game" : "Next Hand";
    $("main-btn").onclick = winner ? newGame : () => { state.dealer = (state.dealer + 1) % 4; startHand(); };
    state.phase = "idle"; render();
  }
  $("main-btn").addEventListener("click", newGame, { once: true });
  state.players = NAMES.map(name => ({ name, score: 0, tricks: 0, hand: [] }));
  render();
}

if (typeof module !== "undefined") module.exports = { SUITS, VALUES, BIDS, WIN_SCORE, buildDeck, shuffle, isTrump, isLeftBower, isRightBower, effectiveSuit, legalCards, trickWinner, cardStrength, scoreHand, estimateBid, botBid, botPlay };

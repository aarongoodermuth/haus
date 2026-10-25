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
// Teams variant: partners sit across from each other — seats 0 & 2 vs 1 & 3.
const teamOf = i => i % 2;
function scoreTeams(players, bidderIdx, bid) {
  // The bid is made on the bidding team's COMBINED tricks. Both teammates
  // carry the team total as their score, so the existing "first to 31"
  // check works unchanged. Bidding team: made -> max(team tricks, bid
  // amount); missed -> -bid amount. Other team: +1 per trick the team won.
  // Haus (amount 12, required 6) works the same way: the team takes all 6.
  const teamTricks = [0, 1].map(t => players.filter((_, i) => teamOf(i) === t).reduce((a, p) => a + p.tricks, 0));
  const bt = teamOf(bidderIdx);
  players.forEach((p, i) => {
    if (teamOf(i) === bt) {
      if (teamTricks[bt] >= bid.required) p.score += Math.max(teamTricks[bt], bid.amount);
      else p.score -= bid.amount;
    } else {
      p.score += teamTricks[1 - bt];
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
// ---- Haus exchange (teams only): the "cold hand" machinery ----
// A hand is COLD for a trump suit when, played alone (partner sitting out),
// no opponent distribution can beat it with correct play order:
//  - Every suit group in the hand must be a top-prefix of that suit's
//    ranking: trump ranks right bar > left bar > A > K > Q > 10 > 9, and an
//    off-suit ranks A > K > Q > J > 10 > 9 (the same-color suit's Jack is
//    the left bar, so that suit's ranking is just A > K > Q > 10 > 9).
//    A prefix means the bidder holds every card above each card they hold,
//    so each lead beats anything an opponent can answer with.
//  - If the hand has any off-suit cards, the bidder must also hold at
//    least 4 trump: leading the trump prefix draws the opponents' trump
//    out (worst case, one opponent holds all 7-k remaining trump, and k
//    leads exhaust them only when k >= 4), so the off-suit prefixes can
//    then be led safely. A pure-trump hand needs no such condition —
//    every lead is a trump lead the opponents cannot beat.
function suitGroups(hand, trump) {
  const groups = {};
  hand.forEach(c => {
    const s = effectiveSuit(c, trump);
    (groups[s] ||= []).push(c);
  });
  return groups;
}
// Trump ranking: right bar > left bar > A > K > Q > 10 > 9. Values are
// A=5, K=4, Q=3, 10=1, 9=0, so shift the face cards down one to sit below
// the left bar's 5 (a Jack is a bar, handled above).
function trumpRank(c, trump) {
  if (isRightBower(c, trump)) return 6;
  if (isLeftBower(c, trump)) return 5;
  return c.value >= 3 ? c.value - 1 : c.value;
}
function isPrefixGroup(cards, trump, suit) {
  const top = suit === trump ? 6 : 5;
  const ranks = cards.map(c => suit === trump ? trumpRank(c, trump) : c.value).sort((a, b) => b - a);
  return ranks.every((r, i) => r === top - i);
}
function allGroupsPrefix(hand, trump) {
  const groups = suitGroups(hand, trump);
  return Object.keys(groups).every(s => isPrefixGroup(groups[s], trump, s));
}
function isColdHand(hand, trump) {
  if (!allGroupsPrefix(hand, trump)) return false;
  const trumpCount = hand.filter(c => isTrump(c, trump)).length;
  const hasOffSuit = hand.some(c => !isTrump(c, trump));
  return !hasOffSuit || trumpCount >= 4;
}
// One card away from cold: removing exactly one card leaves a hand whose
// groups are all prefixes and whose trump count is at least 3, so a trump
// gift from the partner (bringing trump to 4) can complete the cold hand.
// Returns that card (the weak card to pass back), or null.
function exchangeSkeleton(hand, trump) {
  let best = null, bestStrength = Infinity;
  hand.forEach(c => {
    const rest = hand.filter(x => x !== c);
    if (!allGroupsPrefix(rest, trump)) return;
    if (rest.filter(x => isTrump(x, trump)).length < 3) return;
    const strength = isTrump(c, trump) ? 100 + c.value : c.value;
    if (strength < bestStrength) { bestStrength = strength; best = c; }
  });
  return best;
}
// The bidder asks for the exchange exactly when one card away from cold:
// already cold -> no need to bench the partner; further away -> the gift
// can't complete the hand, so the partner's help is worth more.
function shouldAskExchange(hand) {
  const trump = estimateBid(hand).suit;
  return !isColdHand(hand, trump) && exchangeSkeleton(hand, trump) !== null;
}
// The return card, committed blind (before seeing the gift): the weak card
// the skeleton identified — the one being upgraded. Falls back to the
// weakest card if no skeleton exists.
function exchangeReturnCard(hand, trump) {
  return exchangeSkeleton(hand, trump) ||
    hand.slice().sort((a, b) => (isTrump(a, trump) ? 100 + a.value : a.value) - (isTrump(b, trump) ? 100 + b.value : b.value))[0];
}
// The partner's gift. Trump is named AFTER the exchange, so the partner
// cannot condition on it: rank by universal value, as Aaron put it — an
// ace first, then a king, then a bar (any Jack is a bar under some trump),
// then the rest by value. The partner sits out afterwards, so the gift
// costs them nothing.
function exchangeGiftCard(hand) {
  const score = c => (c.value === 5 ? 300 : c.value === 4 ? 200 : c.value === 2 ? 100 : 0) + c.value;
  return hand.slice().sort((a, b) => score(b) - score(a))[0];
}
// The swap itself: bidder gives ret, partner gives gift; both hands stay
// at 6 cards, sorted the way the deal sorts them.
function exchangeCards(bidderHand, partnerHand, ret, gift) {
  const sortHand = h => h.slice().sort((a, b) => a.suit.localeCompare(b.suit) || b.value - a.value);
  return [
    sortHand(bidderHand.filter(c => c !== ret).concat([gift])),
    sortHand(partnerHand.filter(c => c !== gift).concat([ret])),
  ];
}

// ---- Three Player variant: 21-card deck + kitty ----
// Remove every 9 except the 9 of hearts: 21 cards, 6 each + 3 in the kitty.
function buildDeckThree() {
  return buildDeck().filter(c => !(c.value === 0 && c.suit !== "hearts"));
}
// The kitty discard: after taking the kitty the bidder holds 9 cards and
// must shed 3. Shed the weakest: trump cards rank far above off-suit cards
// (a bar is never shed while a low card exists), and among equals shed
// from a singleton suit first to open a void for later trumping.
function kittyDiscards(hand, trump) {
  const suitCount = c => hand.filter(x => x.suit === c.suit).length;
  const score = c => isTrump(c, trump) ? 100 + trumpRank(c, trump) : c.value;
  return hand.slice().sort((a, b) => score(a) - score(b) || suitCount(a) - suitCount(b)).slice(0, 3);
}

function botBid(hand, currentBid, opts = {}) {
  const { est, suit } = estimateBid(hand);
  // Bidding is a double-edged sword: missing costs the bid, but winning
  // the bid names trump — and est is already computed at your best suit,
  // so the trump-control value is baked into the number. On top of that,
  // passing lets someone else name trump, which can only hurt you, so the
  // bid decision gets a small control premium: bid 3 from est 2.75 up.
  const haus = BIDS.find(b => b.id === "haus");
  // A lock hand bids Haus even over a previous Haus: the latest Haus wins.
  if (isHausLock(hand, suit) && (haus.amount > currentBid.amount || currentBid.id === "haus")) return haus;
  // Teams: the bid is made on combined tricks, and your partner averages
  // 1.5 tricks (6 tricks / 4 players) on a random hand — add that to your
  // own estimate before applying the naming-trump premium.
  // Kitty (three player): the bidder swaps 3 cards with the kitty and keeps
  // the best 6 of 9, which is worth roughly half a trick on average.
  const eff = est + (opts.teams ? 1.5 : 0) + (opts.kitty ? 0.5 : 0);
  const want = Math.floor(eff + 0.25);
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
  // Sweeping with trump still in hand: lead the highest remaining trump, so
  // the opponents' trump gets drawn out before the off-suit aces are led.
  // (Without this, the longest-suit fallback could lead an off-suit ace
  // first on a mixed hand, which is exactly what sweeping avoids.)
  if (sweep && hasTrump) {
    const tr = hand.filter(c => isTrump(c, trump));
    return tr.reduce((best, c) => c.value > best.value ? c : best, tr[0]);
  }
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
  // ctx (from the UI; optional in tests): { self, bidder, bidderLive, bidId,
  // partner, teams }. bidderLive = the bidder (or, in teams, the bidder's
  // team) can still make the bid, so winning matters more than saving
  // trump. In solo play everyone scores alone; in teams, partner logic
  // applies (see the duck rule below).
  const ledCard = plays.length ? plays[0].card : null;
  // Sweeping = the Haus bidder (or, in teams, their partner too) is going
  // for Haus and the bid is still alive: lead trump first so the off-suit
  // aces can't be trumped (see leadCard).
  const sweeping = ctx.bidderLive && ctx.bidId === "haus" &&
    (ctx.teams ? teamOf(ctx.self) === teamOf(ctx.bidder) : ctx.self === ctx.bidder);
  if (!ledCard) return leadCard(hand, trump, sweeping);
  const legal = legalCards(hand, trump, ledCard);
  const led = effectiveSuit(ledCard, trump);
  const sorted = legal.slice().sort((a, b) => cardStrength(a, trump, led) - cardStrength(b, trump, led));
  const currentBest = Math.max(...plays.map(p => cardStrength(p.card, trump, led)));
  const currentWinner = plays.reduce((best, p) =>
    cardStrength(p.card, trump, led) > cardStrength(best.card, trump, led) ? p : best, plays[0]);
  const winning = sorted.filter(c => cardStrength(c, trump, led) > currentBest);
  // Teams: if your partner is already winning this trick, don't waste a
  // card on it — the trick is your team's either way. Play the cheapest
  // card that doesn't overtake them (second-hand-low convention); if every
  // legal card wins, the overtake is unavoidable, so fall through and take
  // it as cheaply as possible.
  if (ctx.teams && ctx.partner != null && currentWinner.player === ctx.partner) {
    const safe = legal.filter(c => cardStrength(c, trump, led) <= currentBest);
    if (safe.length) return discardCard(hand, trump, safe);
  }
  if (winning.length) {
    const cheapest = winning[0];
    if (!isTrump(cheapest, trump)) return cheapest; // win without spending trump
    // Winning costs trump. If every legal card is trump, the trump is spent
    // either way, so take the trick. Otherwise it's worth it only when the
    // win is guaranteed (last to act), when it denies the bidder a trick,
    // or when you are the bidder and the bid is still alive — else save
    // the trump and shed a cheap card instead.
    const canShed = legal.some(c => !isTrump(c, trump));
    // Last to act depends on how many seats are playing the trick: 4
  // normally, 3 when a partner sits out after a Haus exchange.
  const lastToAct = plays.length === (ctx.playersInTrick || 4) - 1;
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
  const state = { players: [], dealer: 0, phase: "idle", trump: null, bidder: -1, bid: BIDS[0], plays: [], turn: 0, leader: 0, trickWinner: -1, sitOut: -1, exchange: null, kitty: [], discardSel: [] };
  // Variants: "solo" (four player, every player for themselves), "teams"
  // (four player teams, 2v2, partners across the table), or "three"
  // (three player, 21-card deck with a 3-card kitty). Teams changes the
  // standing bid (4, not 3), the scoring (combined team tricks), and the
  // bot logic (partner-aware); three changes the deck, the player count,
  // and adds the kitty.
  let variant = "solo";
  const playerCount = () => variant === "three" ? 3 : 4;
  const roster = () => NAMES.slice(0, playerCount());
  // A trick has as many cards as there are players, except when a partner
  // sits out after a Haus exchange (the sitter's seat is skipped entirely).
  const trickSize = () => state.sitOut >= 0 ? 3 : playerCount();
  // Standing bid: 3 in solo, 4 in teams and three (no 3 bid exists there).
  const standingBid = () => BIDS.find(b => b.id === (variant === "solo" ? "3" : "4"));
  const teamNames = [`${NAMES[0]} & ${NAMES[2]}`, `${NAMES[1]} & ${NAMES[3]}`];
  // The Deal button has one persistent handler that dispatches on the game
  // state (fresh game vs next hand) — a single binding, so switching
  // variants mid-hand can never leave it dead or double-firing.
  let mainBtnMode = "new";
  document.getElementById("main-btn").addEventListener("click", () => {
    if (state.phase !== "idle") return;
    if (mainBtnMode === "next") { state.dealer = (state.dealer + 1) % playerCount(); startHand(); }
    else newGame();
  });
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
    if (variant === "teams" && state.players.length) {
      // Team chips: both teammates carry the team total, so either member's
      // score is the team score; tricks are the team's combined tricks.
      $("scoreboard").innerHTML = [0, 1].map(t => {
        const members = state.players.filter((_, i) => teamOf(i) === t);
        const tricks = members.reduce((a, p) => a + p.tricks, 0);
        return `<span class="chip">${teamOf(state.dealer) === t ? "🂠 " : ""}${teamNames[t]}: <strong>${members[0].score}</strong> · ${tricks} ${tricks === 1 ? "trick" : "tricks"}</span>`;
      }).join("");
    } else {
      $("scoreboard").innerHTML = state.players.map((p, i) =>
        `<span class="chip">${i === state.dealer ? "🂠 " : ""}${p.name}: <strong>${p.score}</strong> · ${p.tricks} ${p.tricks === 1 ? "trick" : "tricks"}</span>`).join("");
    }
    $("status").textContent = state.trump
      ? `Trump: ${SUIT_BY_ID[state.trump].symbol} · Bid: ${state.bid.label} by ${state.players[state.bidder]?.name || "—"}`
      : "Bidding / dealing";
    // Bidding stage only: who dealt, who is currently under for the standing
    // bid (the dealer holds it until someone bids higher), and every bid
    // made so far, so your turn shows what came before it.
    const bidInfo = $("bid-info");
    if (state.phase === "bid" && state.players.length) {
      const dealerName = state.players[state.dealer].name;
      const underNote = state.bids.every(b => b.label === "Pass")
        ? ` · ${dealerName} is under for ${standingBid().label} unless someone bids higher`
        : "";
      bidInfo.innerHTML = `<div>Dealer: ${dealerName}${underNote}</div>` +
        (state.bids.length
          ? `<div>Bids so far: ${state.bids.map(b => `${state.players[b.player].name} — ${b.label}`).join(" · ")}</div>`
          : "");
    } else bidInfo.innerHTML = "";
    const seatBase = ["seat seat-south", "seat seat-west", "seat seat-north", "seat seat-east"];
    state.players.forEach((p, i) => {
      $("name-" + i).textContent = (state.trickWinner === i && state.plays.length === trickSize() ? "🏆 " : "") + p.name;
      const seat = $("seat-" + i);
      if (seat) seat.className = seatBase[i] + (variant === "teams" ? (teamOf(i) === 0 ? " team-a" : " team-b") : "") + (i === state.sitOut ? " sitting" : "");
      const slot = $("played-" + i); slot.innerHTML = "";
      const play = state.plays.find(x => x.player === i);
      if (play) slot.appendChild(cardEl(play.card, { small: true, winner: state.trickWinner === i && state.plays.length === trickSize() }));
      // Debug: opponents' whole hands face up under their seats. Hands are
      // already sorted at deal, so they render as dealt. In teams, the
      // "opponents" are the other team only — your partner's hand stays
      // hidden, as the variant requires. Every bot slot is cleared first so
      // switching variants can't leave a stale hand on screen.
      if (DEBUG.showHands && i > 0) {
        const handSlot = $("hand-" + i);
        if (handSlot) {
          handSlot.innerHTML = "";
          if (!(variant === "teams" && i === 2)) {
            p.hand.forEach(c => handSlot.appendChild(cardEl(c, { small: true })));
          }
        }
      }
    });
    // The center stays quiet: the trick itself is shown by the cards in the
    // seats, the winner by the trophy by their name + the gold card, and
    // the Deal button speaks for itself. The only thing the center still
    // says is who leads, which appears nowhere else.
    const center = $("center-info");
    center.textContent = (!state.plays.length && state.trump) ? `${state.players[state.leader].name} to lead` : "";
    // Kitty (three player): face-down backs until the bidder takes it;
    // face up on the debug site, where everything is visible.
    const kittyEl = $("kitty");
    if (kittyEl) {
      kittyEl.innerHTML = "";
      if (state.kitty && state.kitty.length) {
        if (DEBUG.showHands) state.kitty.forEach(c => kittyEl.appendChild(cardEl(c, { small: true })));
        else kittyEl.textContent = "🂠 🂠 🂠";
      }
    }
    // Three player uses only the first 3 seats; the east seat sits empty.
    const east = $("seat-3");
    if (east) east.style.display = variant === "three" ? "none" : "";
    const handEl = $("hand");
    if (handDrag && handDrag.active) return; // don't rebuild the hand mid-drag (bot timers re-render)
    handEl.innerHTML = "";
    handEl.className = state.sitOut === 0 ? "sitting" : "";
    const me = state.players[0]; if (!me) return;
    const ledCard = state.plays.length ? state.plays[0].card : null;
    // During an exchange the hand is tappable too: every card is a legal
    // choice (a return card to pass back, or a gift for the bidder). Same
    // during the kitty discard: every card is a legal discard pick, and
    // picked cards get a raised, highlighted look.
    const inExchange = state.phase === "exchange-return" || state.phase === "exchange-gift";
    const inDiscard = state.phase === "discard";
    const playable = (state.phase === "play" && state.turn === 0) || inExchange || inDiscard;
    const legal = state.phase === "play" && state.turn === 0 ? legalCards(me.hand, state.trump, ledCard) : ((inExchange || inDiscard) ? me.hand : []);
    me.hand.forEach(c => {
      const el = cardEl(c, {});
      el._card = c;
      if (playable && !legal.includes(c)) el.className += " disabled";
      if (inDiscard && state.discardSel.includes(c)) el.className += " selected";
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
    state.players = roster().map(name => ({ name, score: 0, tricks: 0, hand: [] }));
    state.dealer = Math.floor(Math.random() * playerCount());
    startHand();
  }
  function hideMainBtn() { $("main-btn").style.display = "none"; }
  function showMainBtn() { $("main-btn").style.display = "block"; }
  function startHand() {
    // Three player uses the 21-card deck (9s removed except hearts); the
    // 3 cards left after dealing 6 each become the kitty in the middle.
    const deck = shuffle(variant === "three" ? buildDeckThree() : buildDeck());
    state.players.forEach(p => { p.tricks = 0; p.hand = []; });
    for (let r = 0; r < CARDS_PER_PLAYER; r++) for (let i = 0; i < playerCount(); i++) state.players[(state.dealer + 1 + i) % playerCount()].hand.push(deck.pop());
    state.kitty = variant === "three" ? [deck.pop(), deck.pop(), deck.pop()] : [];
    // Sort once, here at the deal. From then on the array order IS the display
    // order, so the player can drag cards into their own order and it sticks.
    state.players.forEach(p => p.hand.sort((a, b) => a.suit.localeCompare(b.suit) || b.value - a.value));
    state.trump = null; state.plays = []; state.trickWinner = -1; state.phase = "bid";
    state.sitOut = -1; state.exchange = null; state.discardSel = [];
    // Aaron's rules, overriding C++ DoBet on order but keeping its standing
    // bid: the dealer bids LAST, not first, and the dealer starts the round
    // under for the standing bid held by the dealer — 3 in solo, 4 in
    // teams (no 3 bid exists in teams). So a non-dealer's first bid must
    // beat the standing bid. Bidding starts left of the dealer; if nobody
    // outbids the standing bid, the dealer takes it (or raises) — the
    // dealer cannot pass out of the hand.
    state.bid = standingBid(); state.bidder = state.dealer; state.bids = [];
    state.bidTurn = (state.dealer + 1) % playerCount(); state.bidsSeen = 0;
    msg(`Hand dealt. Dealer: ${state.players[state.dealer].name} — bidding starts with ${state.players[state.bidTurn].name}, dealer bids last.`);
    hideMainBtn(); // the Deal button is done its job — it stays out of the way until the hand ends
    render(); nextBid();
  }
  function nextBid() {
    if (state.bidsSeen >= playerCount()) return finishBidding();
    const i = state.bidTurn;
    if (i === 0) {
      const panel = $("panel"); panel.innerHTML = "";
      // The dealer acts last, so at the dealer's turn the standing bid is
      // still un-outbid exactly when nobody has bid higher: that is the
      // dealer-under case — no Pass button (the dealer can't pass out of
      // the hand), and taking the standing bid is offered as "Under for
      // 3/4", not as a raise. For a non-dealer the standing bid simply
      // filters out everything at or below it.
      const standing = standingBid();
      const dealerUnder = state.dealer === 0 && state.bidder === state.dealer && state.bid.amount === standing.amount;
      BIDS.forEach(b => {
        // Same-or-lower bids are never offered: only bids beating the
        // current high bid appear — except the dealer taking their own
        // standing bid, which is accepting the under, not raising, and
        // except Haus over Haus: the latest Haus bid wins the bid, so a
        // Haus is always offered even when the current bid is Haus.
        if (b.id !== "pass" && b.amount <= state.bid.amount &&
          !(dealerUnder && b.amount === standing.amount) && !(b.id === "haus" && state.bid.id === "haus")) return;
        if (b.id === "pass" && dealerUnder) return;
        const btn = document.createElement("button"); btn.type = "button";
        btn.textContent = b.id === "pass" ? "Pass" : (dealerUnder && b.amount === standing.amount ? "Under for " + standing.label : "Bid " + b.label);
        if (b.id === "pass") btn.className = "secondary";
        btn.addEventListener("click", () => { panel.innerHTML = ""; acceptBid(0, b); });
        panel.appendChild(btn);
      });
      // Same condition that hides the Pass button: the prompt must match
      // the buttons — no "or pass" when passing isn't an option.
      msg(dealerUnder
        ? `You're under for ${standing.label} — take it or bid higher.`
        : "Your bid — or pass.");
    } else {
      setTimeout(() => {
        let b = botBid(state.players[i].hand, state.bid, { teams: variant === "teams", kitty: variant === "three" });
        // The dealer-last rule applies to bots too: a dealer facing the
        // un-outbid standing bid cannot pass, so a passing bot dealer takes it.
        if (i === state.dealer && state.bidder === state.dealer && state.bid.amount === standingBid().amount && b.id === "pass") b = standingBid();
        acceptBid(i, b);
      }, 450);
    }
    render();
  }
  function acceptBid(i, bid) {
    state.bids.push({ player: i, label: bid.id === "pass" ? "Pass" : bid.label });
    // Haus over Haus wins: the latest Haus bid takes the bid even though
    // the amounts tie — the only tie that counts as a raise.
    if (bid.amount > state.bid.amount || (bid.id === "haus" && state.bid.id === "haus")) { state.bid = bid; state.bidder = i; msg(`${state.players[i].name} bids ${bid.label}.`); }
    else msg(`${state.players[i].name} passes.`);
    state.bidsSeen++; state.bidTurn = (state.bidTurn + 1) % playerCount(); nextBid();
  }
  function finishBidding() {
    // Under for the standing bid: the dealer holds it because nobody outbid it.
    // (The bidder can never be "nobody" now — the standing bid guarantees
    // the dealer is the bidder at minimum.)
    state.bidForced = state.bidder === state.dealer && state.bid.amount === standingBid().amount &&
      state.bids.filter(b => b.player !== state.dealer).every(b => b.label === "Pass");
    if (state.bidForced) {
      msg(`Everyone passed — dealer ${state.players[state.dealer].name} is under for ${standingBid().label}.`);
    }
    // Teams Haus exchange: before trump is named, the Haus bidder may ask
    // their partner for a card (partner then sits out). Otherwise straight
    // to the trump pick.
    if (variant === "teams" && state.bid.id === "haus") startExchange();
    else showTrumpPick();
    render();
  }
  function showTrumpPick(customMsg) {
    if (state.bidder === 0) {
      const panel = $("panel"); panel.innerHTML = "";
      SUITS.forEach(s => {
        const btn = document.createElement("button"); btn.type = "button"; btn.textContent = s.symbol + " " + s.id;
        btn.addEventListener("click", () => { panel.innerHTML = ""; setTrump(s.id); });
        panel.appendChild(btn);
      });
      msg(customMsg || (state.bidForced
        ? `You're under for ${standingBid().label} — pick trump:`
        : `You won the bid at ${state.bid.label}. Pick trump:`));
    } else {
      setTimeout(() => setTrump(botChooseTrump(state.players[state.bidder].hand)), 500);
    }
  }
  // ---- Haus exchange (teams only) ----
  // Order of operations, per the rule: the bidder asks BEFORE naming trump;
  // the partner passes a card; the bidder passes one back BEFORE looking
  // at the gift; then the partner sits out and the bidder names trump on
  // the final hand and plays alone for all 6 tricks.
  function startExchange() {
    const bidder = state.bidder, partner = (bidder + 2) % 4;
    state.exchange = { bidder, partner, gift: null, ret: null };
    if (bidder === 0) {
      // Human bidder: the exchange is optional. Sitting the partner out is
      // a real cost, so the choice is explicit.
      const panel = $("panel"); panel.innerHTML = "";
      const ask = document.createElement("button"); ask.type = "button"; ask.textContent = "Ask partner for a card";
      ask.addEventListener("click", () => { panel.innerHTML = ""; beginReturnPick(); });
      const no = document.createElement("button"); no.type = "button"; no.textContent = "No, pick trump"; no.className = "secondary";
      no.addEventListener("click", () => { panel.innerHTML = ""; state.exchange = null; showTrumpPick(); });
      panel.appendChild(ask); panel.appendChild(no);
      msg(`You won the bid at Haus! You may ask ${NAMES[partner].name} for a card — but ${NAMES[partner].name} will sit out the hand if you do.`);
    } else {
      // Bot bidder: asks only when one card away from cold (see engine).
      const hand = state.players[bidder].hand;
      if (!shouldAskExchange(hand)) { state.exchange = null; showTrumpPick(); return; }
      // The return card is committed NOW, from the bidder's hand alone —
      // the bidder must not see the gift before choosing what to pass back.
      state.exchange.ret = exchangeReturnCard(hand, estimateBid(hand).suit);
      if (partner === 0) {
        state.phase = "exchange-gift";
        msg(`${NAMES[bidder].name} bid Haus and asks you for a card — choose one to pass. ${NAMES[bidder].name} will pass one back unseen, then you sit out.`);
        render();
      } else {
        msg(`${NAMES[bidder].name} bid Haus and asks ${NAMES[partner].name} for a card…`);
        setTimeout(() => completeExchange(exchangeGiftCard(state.players[partner].hand)), 700);
      }
    }
  }
  function beginReturnPick() {
    // The bot partner's gift is chosen now but NOT revealed: the human must
    // commit a return card first, sight unseen.
    const partner = state.exchange.partner;
    state.exchange.gift = exchangeGiftCard(state.players[partner].hand);
    state.phase = "exchange-return";
    msg(`Choose a card to pass back — you won't see ${NAMES[partner].name}'s card until you've chosen.`);
    render();
  }
  function completeExchange(gift) {
    const { bidder, partner, ret } = state.exchange;
    // From here on the exchange is over: further hand taps must be inert
    // until the trump pick (human bidder) or the bot's trump choice lands.
    state.phase = "exchange-done";
    const bp = state.players[bidder], pp = state.players[partner];
    [bp.hand, pp.hand] = exchangeCards(bp.hand, pp.hand, ret, gift);
    state.sitOut = partner;
    state.exchange = null;
    if (bidder === 0) {
      render();
      showTrumpPick(`${NAMES[partner].name} passes you the ${cardLabel(gift)}. ${NAMES[partner].name} sits out — you play alone for Haus. Pick trump:`);
    } else {
      msg(`${NAMES[bidder].name} takes the ${cardLabel(gift)} and passes ${NAMES[partner].name} the ${cardLabel(ret)} unseen. ${NAMES[partner].name} sits out — ${NAMES[bidder].name} plays alone for Haus.`);
      render();
      setTimeout(() => setTrump(botChooseTrump(bp.hand)), 600);
    }
  }
  function setTrump(suit) {
    state.trump = suit; state.leader = state.bidder; state.plays = [];
    msg(`Trump is ${suit}. ${state.players[state.bidder].name} leads.`);
    // Three player: the bidder takes the kitty and discards 3 before play
    // begins; every other variant goes straight to the first trick.
    if (variant === "three") beginKitty();
    else { state.phase = "play"; state.turn = state.bidder; render(); nextPlay(); }
  }
  function startPlay() {
    state.phase = "play"; state.turn = state.bidder; render(); nextPlay();
  }
  // ---- Kitty (three player) ----
  // The bidder adds the 3 kitty cards (9 in hand), then sheds 3, keeping 6.
  function beginKitty() {
    const bidder = state.bidder, bp = state.players[bidder];
    bp.hand = bp.hand.concat(state.kitty);
    bp.hand.sort((a, b) => a.suit.localeCompare(b.suit) || b.value - a.value);
    state.kitty = [];
    if (bidder === 0) {
      state.phase = "discard"; state.discardSel = [];
      const panel = $("panel"); panel.innerHTML = "";
      const btn = document.createElement("button"); btn.type = "button"; btn.textContent = "Discard 3 ▸";
      btn.addEventListener("click", () => {
        if (state.discardSel.length !== 3) { msg("Tap 3 cards to discard first."); return; }
        panel.innerHTML = "";
        bp.hand = bp.hand.filter(c => !state.discardSel.includes(c));
        state.discardSel = [];
        msg("Kitty taken, 3 cards discarded — play begins.");
        startPlay();
      });
      panel.appendChild(btn);
      msg("You take the kitty — tap 3 cards to discard, then discard them.");
      render();
    } else {
      const discards = kittyDiscards(bp.hand, state.trump);
      bp.hand = bp.hand.filter(c => !discards.includes(c));
      msg(`${bp.name} takes the kitty and discards 3 cards.`);
      render();
      setTimeout(startPlay, 700);
    }
  }
  function nextPlay() {
    render();
    if (state.turn === 0) { msg("Your turn — tap a card."); return; }
    setTimeout(() => {
      const p = state.players[state.turn];
      const teams = variant === "teams";
      const bt = teamOf(state.bidder);
      // Bidder's bid is still alive if the tricks held plus the tricks left
      // (one per card still in hand) can still reach the requirement — in
      // teams, counted across the bidding team, not just the bidder. A
      // sitting partner's cards are dead, so they count for nothing here.
      const teamAlive = state.players.filter((_, i) => teamOf(i) === bt && i !== state.sitOut)
        .reduce((a, q) => a + q.tricks + q.hand.length, 0);
      const bidderLive = teams
        ? teamOf(state.turn) === bt && teamAlive >= state.bid.required
        : state.turn === state.bidder && p.tricks + p.hand.length >= state.bid.required;
      const card = botPlay(p.hand, state.trump, state.plays, {
        self: state.turn,
        bidder: state.bidder,
        bidderLive,
        bidId: state.bid.id,
        partner: teams ? (state.turn + 2) % 4 : null,
        teams,
        playersInTrick: trickSize(),
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
    // Last trick = every ACTIVE player's hand is empty. (The sitting
    // partner's hand never empties, so it must be excluded — otherwise a
    // sit-out hand would never end.)
    const lastTrick = state.players.every((p, i) => i === state.sitOut || p.hand.length === 0);
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
    // Kitty discard taps: tap to pick (or unpick) a card for the discard.
    if (state.phase === "discard") {
      const sel = state.discardSel;
      const at = sel.indexOf(card);
      if (at >= 0) sel.splice(at, 1);
      else if (sel.length < 3) sel.push(card);
      else msg("Already picked 3 — tap one to unpick it.");
      render();
      return;
    }
    // Exchange taps: the human either commits a return card (blind — the
    // partner's gift is revealed only after this choice) or picks a gift
    // card for a bot bidder who asked them.
    if (state.phase === "exchange-return" && state.exchange) {
      state.exchange.ret = card;
      completeExchange(state.exchange.gift);
      return;
    }
    if (state.phase === "exchange-gift" && state.exchange) {
      completeExchange(card);
      return;
    }
    if (state.phase !== "play" || state.turn !== 0) return;
    doPlay(0, card);
  }
  function doPlay(i, card) {
    const p = state.players[i];
    p.hand = p.hand.filter(c => c !== card);
    state.plays.push({ player: i, card });
    if (state.plays.length === trickSize()) {
      const winner = trickWinner(state.plays, state.trump);
      state.players[winner].tricks++;
      state.leader = winner;
      pauseAfterTrick(winner);
    } else {
      // Advance, skipping the sitting partner's seat entirely.
      let nt = (i + 1) % playerCount();
      if (nt === state.sitOut) nt = (nt + 1) % playerCount();
      state.turn = nt; render(); nextPlay();
    }
    render();
  }
  function endHand() {
    const bid = state.bid, bidder = state.bidder;
    const teams = variant === "teams";
    if (teams) scoreTeams(state.players, bidder, bid); else scoreHand(state.players, bidder, bid);
    const teamTricks = t => state.players.filter((_, i) => teamOf(i) === t).reduce((a, p) => a + p.tricks, 0);
    const made = teams ? teamTricks(teamOf(bidder)) >= bid.required : state.players[bidder].tricks >= bid.required;
    const top = state.players.slice().sort((a, b) => b.score - a.score)[0]; // C++ Game: winner is the high scorer
    const winner = top.score >= WIN_SCORE ? top : null;
    const bidLine = teams
      ? `${teamNames[teamOf(bidder)]} bid ${bid.label}, took ${teamTricks(teamOf(bidder))}`
      : `${state.players[bidder].name} bid ${bid.label}, took ${state.players[bidder].tricks}`;
    const winLine = winner
      ? (teams ? ` 🏆 ${teamNames[teamOf(state.players.indexOf(top))]} win the game with ${top.score}!` : ` 🏆 ${winner.name} wins the game with ${winner.score}!`)
      : " Tap Next Hand.";
    msg(`Hand over: ${bidLine} — ${made ? "made it" : "set!"}` + winLine);
    showMainBtn();
    $("main-btn").textContent = winner ? "New Game" : "Next Hand";
    mainBtnMode = winner ? "new" : "next";
    state.phase = "idle"; render();
  }
  // Variant switch: Solo vs Teams (2v2). Switching resets to a fresh idle
  // game (scores cleared, Deal button up) — a hand in progress is discarded.
  // The choice is remembered across reloads when storage is available.
  function applyVariant(v, save) {
    variant = v;
    const fourBtn = document.getElementById("variant-four"), teamsBtn = document.getElementById("variant-teams"), threeBtn = document.getElementById("variant-three");
    if (fourBtn) fourBtn.className = v === "solo" ? "active" : "";
    if (teamsBtn) teamsBtn.className = v === "teams" ? "active" : "";
    if (threeBtn) threeBtn.className = v === "three" ? "active" : "";
    const tagline = document.getElementById("tagline");
    if (tagline) tagline.textContent = DEBUG.showHands
      ? (v === "teams" ? "Debug build — Teams 2v2, opposing team's hands are face up."
        : v === "three" ? "Debug build — Three player, kitty and hands face up."
        : "Debug build — opponents' hands are face up. Same game, same rules.")
      : (v === "teams" ? "Teams 2v2 — you & Karen vs Jarvis & Bender. First team to 31 wins."
        : v === "three" ? "Three player — 21 cards, a 3-card kitty, first to 31 wins."
        : "Hasenpfeffer, in your pocket. First to 31 wins.");
    if (save) { try { localStorage.setItem("haus-variant", v); } catch (_) {} }
  }
  function switchVariant(v) {
    if (v === variant) return;
    applyVariant(v, true);
    state.players = roster().map(name => ({ name, score: 0, tricks: 0, hand: [] }));
    state.dealer = 0; state.phase = "idle"; state.trump = null; state.plays = []; state.trickWinner = -1;
    state.bid = BIDS[0]; state.bidder = -1; state.bids = []; state.sitOut = -1; state.exchange = null;
    state.kitty = []; state.discardSel = [];
    handDrag = null;
    $("panel").innerHTML = ""; $("hand").innerHTML = "";
    showMainBtn(); $("main-btn").textContent = "Deal"; mainBtnMode = "new";
    msg(v === "teams" ? "Teams (2v2) — Karen is your partner across the table. Tap Deal to start."
      : v === "three" ? "Three player — 21 cards and a 3-card kitty. Tap Deal to start."
      : "Solo — every player for themselves. Tap Deal to start.");
    render();
  }
  const fourBtn = document.getElementById("variant-four"), teamsBtn = document.getElementById("variant-teams"), threeBtn = document.getElementById("variant-three");
  if (fourBtn) fourBtn.addEventListener("click", () => switchVariant("solo"));
  if (teamsBtn) teamsBtn.addEventListener("click", () => switchVariant("teams"));
  if (threeBtn) threeBtn.addEventListener("click", () => switchVariant("three"));
  try {
    const saved = localStorage.getItem("haus-variant");
    applyVariant(saved === "teams" || saved === "solo" || saved === "three" ? saved : "solo", false);
  } catch (_) { applyVariant("solo", false); }
  state.players = roster().map(name => ({ name, score: 0, tricks: 0, hand: [] }));
  render();
}

if (typeof module !== "undefined") module.exports = { SUITS, VALUES, BIDS, WIN_SCORE, buildDeck, buildDeckThree, shuffle, isTrump, isLeftBower, isRightBower, effectiveSuit, legalCards, trickWinner, cardStrength, scoreHand, scoreTeams, teamOf, estimateBid, botBid, botPlay, isColdHand, exchangeSkeleton, shouldAskExchange, exchangeReturnCard, exchangeGiftCard, exchangeCards, kittyDiscards };

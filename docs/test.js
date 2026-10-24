// Headless sanity tests for the Haus engine: node test.js
const assert = require("assert");
const G = require("./game.js");

// Regression: trump is null during deal/bidding, and the UI calls the bower
// helpers for every rendered card. They must return false, never throw —
// the old isLeftBower crashed rendering on any Jack, which truncated the
// player's hand on screen and stopped bidding from starting.
for (const c of G.buildDeck()) {
  assert.strictEqual(G.isLeftBower(c, null), false);
  assert.strictEqual(G.isRightBower(c, null), false);
  assert.strictEqual(G.isTrump(c, null), false);
  assert.strictEqual(G.effectiveSuit(c, null), c.suit);
}

// Deck: 24 cards, 9..A in 4 suits
const deck = G.buildDeck();
assert.strictEqual(deck.length, 24);
assert.strictEqual(new Set(deck.map(c => c.suit + c.value)).size, 24);

// Bowers: hearts trump -> J♥ right, J♦ left, both trump; J♣ is not
const hearts = "hearts";
assert.ok(G.isRightBower({ suit: "hearts", value: 2 }, hearts));
assert.ok(G.isLeftBower({ suit: "diamonds", value: 2 }, hearts));
assert.ok(G.isTrump({ suit: "diamonds", value: 2 }, hearts));
assert.ok(!G.isTrump({ suit: "clubs", value: 2 }, hearts));
assert.strictEqual(G.effectiveSuit({ suit: "diamonds", value: 2 }, hearts), "hearts");

// Trick: left bower beats trump ace; trump 9 beats led ace of another suit
let w = G.trickWinner([
  { player: 0, card: { suit: "hearts", value: 5 } },
  { player: 1, card: { suit: "diamonds", value: 2 } },
], hearts);
assert.strictEqual(w, 1);
w = G.trickWinner([
  { player: 0, card: { suit: "spades", value: 5 } },
  { player: 1, card: { suit: "hearts", value: 0 } },
], hearts);
assert.strictEqual(w, 1);

// Follow suit, C++ FLegalPlay style: judged by PRINTED suit.
// Led trump (hearts) with no printed heart in hand -> anything goes, left bower included.
const hand = [{ suit: "diamonds", value: 2 }, { suit: "clubs", value: 5 }];
assert.strictEqual(G.legalCards(hand, hearts, { suit: "hearts", value: 3 }).length, 2);
// Led diamonds -> the left bower (J♦) is a printed diamond and must follow.
assert.deepStrictEqual(G.legalCards(hand, hearts, { suit: "diamonds", value: 3 }), [hand[0]]);
// Led a suit you hold -> only that suit; led a suit you don't -> anything.
assert.strictEqual(G.legalCards(hand, hearts, { suit: "spades", value: 3 }).length, 2);

// Scoring (from the C++ build): bidder makes 4 with 5 tricks -> +5; misses 4 -> -4;
// Haus makes all 6 -> +12; misses -> -12; non-bidders get +1 per trick won.
function scored(bidderTricks, bidId, others = [0, 0, 0]) {
  const players = [0, 1, 2, 3].map(() => ({ score: 0, tricks: 0 }));
  players[0].tricks = bidderTricks; others.forEach((t, i) => players[i + 1].tricks = t);
  G.scoreHand(players, 0, G.BIDS.find(b => b.id === bidId));
  return players.map(p => p.score);
}
assert.deepStrictEqual(scored(5, "4", [1, 0, 0]), [5, 1, 0, 0]);
assert.deepStrictEqual(scored(3, "4"), [-4, 0, 0, 0]);
assert.deepStrictEqual(scored(6, "haus"), [12, 0, 0, 0]);
assert.deepStrictEqual(scored(5, "haus"), [-12, 0, 0, 0]);

// Full random games: every hand ends, tricks always total 6, someone reaches 31
for (let game = 0; game < 200; game++) {
  const players = [0, 1, 2, 3].map(() => ({ score: 0, tricks: 0, hand: [] }));
  let dealer = 0, guard = 0;
  while (!players.some(p => p.score >= G.WIN_SCORE) && guard++ < 500) {
    const d = G.shuffle(G.buildDeck());
    players.forEach(p => { p.tricks = 0; p.hand = []; });
    for (let r = 0; r < 6; r++) for (let i = 0; i < 4; i++) players[(dealer + 1 + i) % 4].hand.push(d.pop());
    let bid = G.BIDS[0], bidder = -1, turn = (dealer + 1) % 4;
    for (let n = 0; n < 4; n++) { const b = G.botBid(players[turn].hand, bid); if (b.amount > bid.amount) { bid = b; bidder = turn; } turn = (turn + 1) % 4; }
    if (bidder === -1) { bidder = dealer; bid = G.BIDS.find(b => b.id === "3"); }
    const trump = "hearts"; let leader = bidder;
    for (let t = 0; t < 6; t++) {
      const plays = []; let cur = leader;
      for (let n = 0; n < 4; n++) { const card = G.botPlay(players[cur].hand, trump, plays); players[cur].hand = players[cur].hand.filter(c => c !== card); plays.push({ player: cur, card }); cur = (cur + 1) % 4; }
      leader = G.trickWinner(plays, trump); players[leader].tricks++;
    }
    assert.strictEqual(players.reduce((a, p) => a + p.tricks, 0), 6);
    G.scoreHand(players, bidder, bid);
    dealer = (dealer + 1) % 4;
  }
  assert.ok(players.some(p => p.score >= G.WIN_SCORE), "game should finish");
}
console.log("All Haus engine tests passed.");

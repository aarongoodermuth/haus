# Haus — draft rules

Status: **working draft (v0.1 prototype)**. The baseline below is ported from Aaron's
existing C++ `haus` repo (2015–2022), cross-checked against published
Hasenpfeffer / Pepper / Double Hasenpfeffer rules.

**Decision (Aaron, 2026-10-23): when in doubt, go with the C++ rules.** His rules
are intentionally slightly non-standard. Rule details will be revisited later;
for now the C++ build is the source of truth and every question below is
resolved to the C++ behaviour unless marked otherwise.

Note on the name: published sources spell it **Hasenpfeffer** ("peppered hare").
The 48-card double-deck variant is also called **Hawsey / Hausey** — likely
where "Haus / Hausenpfeffer" comes from. See Q2.

## Baseline implemented in v0.1

- **Players:** 4, each against the others for scoring (ported from the C++ build — see Q1).
  You + 3 bots (Jarvis, Karen, Bender).
- **Deck:** 24 cards — 9, 10, J, Q, K, A in each suit. No joker.
- **Deal:** 6 cards each, clockwise, starting left of the dealer. Dealer rotates each hand.
- **Bidding:** One round, **starting left of the dealer** and going
  clockwise, with the **dealer bidding last** (Aaron's rule, overriding
  C++ `Round::DoBet`, which starts at the dealer). The dealer starts the
  round **under for 3**: the standing bid is 3 held by the dealer (C++
  `DEFAULT_DEALER`), so a non-dealer's first bid must be **4 or higher**
  — bid 3 is never offered to a non-dealer, since it wouldn't beat the
  standing 3. Each bid must beat the current high bid.
  - *Haus* = a bid to take all 6 tricks, worth **12 points**. No card
    exchange, no playing alone — all four players play the hand out.
    Haus is the exception to "must beat the current bid": if someone has
    bid Haus, a later bidder may also bid Haus, and the **latest Haus
    bid wins the bid**.
  - If nobody outbids the standing 3, the dealer **takes it**: the dealer
    cannot pass out of the hand, and must either take the under for 3 or
    raise to 4+. If someone has bid higher, the dealer may pass or
    outbid like anyone else.
- **Trump:** The high bidder names trump and leads the first trick.
  - Trump rank: **right bar** (Jack of trump) highest, then **left bar**
    (Jack of the other suit of the same colour — it counts as trump for
    winning tricks), then A, K, Q, 10, 9.
  - Other suits rank A, K, Q, J, 10, 9. No "no-trump" call.
- **Play:** Must follow the suit led if you can — judged by the card's
  **effective suit**: a bar is trump for every purpose, including
  following suit. So with clubs trump and spades led, the J♠ is a club,
  not a spade — a hand holding J♠ and no other spades is void in spades
  and may play anything. Conversely, when trump is led, the left bar
  must follow it like any other trump card. Highest trump wins the trick;
  otherwise the highest card of the suit led. Trick winner leads next.
- **Scoring (per player, from the C++ build):**
  - Bidder makes the bid → scores the greater of tricks taken or the bid amount
    (Haus made → 12).
  - Bidder misses → loses the bid amount (Haus missed → −12).
  - Non-bidders score 1 point per trick they personally took.
- **Winning:** First player to **31 points** (checked at end of a hand).

## Resolved — by defaulting to the C++ build (2026-10-23)

1. **Partnerships or individual?** → Individual scoring (C++), even though
   published Hasenpfeffer/Pepper is 2-vs-2 partnerships.
2. **24-card or 48-card deck?** → 24 cards / 6 tricks (C++), not the 48-card
   "Hausey" double-deck variant (12 cards, minimum bid 6).
3. **"Haus" bid** → All 6 tricks for +12 / −12, no partner exchange, no
   playing alone (C++ `BetChoice::TWELVE`).
4. **Left bar follow-suit** → ~~By printed suit (C++ `FLegalPlay`)~~ —
   **corrected by Aaron 2026-10-24: a bar is always trump**, including
   for following suit (see Play above). The C++ printed-suit rule is not
   followed here.
5. **No-trump / high / low calls?** → None. Trump is always a suit (C++).
6. **Game target** → 31 (C++ `FIsWinner`), winner is the high scorer.
7. **All-pass rule** → Dealer forced at 3 (C++ `DEFAULT_DEALER`).
8. **Joker / widow** → Neither (C++ deck is exactly 24 cards).
9. **Bid order** → ~~Starts at the dealer, clockwise, one pass around
   (C++ `DoBet`)~~ — **corrected by Aaron 2026-10-24: the dealer bids
   last**, one pass around starting left of the dealer. The standing bid
   is 3 held by the dealer (under for 3): non-dealers must bid 4+,
   and if nobody outbids the standing 3 the dealer takes it (no Pass
   for the dealer in that case) or raises.
10. **Who leads?** → High bidder leads the first trick (C++).

## One deliberate exception

11. **Trick comparison for off-suit cards.** C++ `CardComparer::FGreaterThan`
    compares non-trump cards by value alone, with no notion of the suit led —
    read literally, an off-suit Ace would beat a led 9. That looks like a bug
    in the unfinished C++ code rather than a house rule, so the web build
    uses the standard reading (off-suit cards can't win a trick) and flags it
    here. Easy to flip if that really was intended.

12. **Bar follow-suit.** C++ `Rules::FLegalPlay` judges following suit by
    printed suit, which would make the left bar follow its printed suit.
    Aaron corrected this 2026-10-24: a bar is always trump, including
    for following suit (see Play above and item 4).

Everything else about the rules is deferred — Aaron will have more to say
later. This file is where rule changes get recorded as they're decided.

## Teams variant (2v2, added 2026-10-25)

The same game with partners: the four seats form two teams of players
across from each other (south + north vs west + east). Switch between
Solo and Teams on the page; the choice is remembered across reloads.

- **Bidding alone, made together.** One player bids and names trump, as
  in solo — but the bid is made or missed on the bidding team's
  COMBINED tricks (bidder + partner), not the bidder's alone. You never
  see your partner's cards.
- **Under for 4, no 3 bid.** The standing bid is 4 held by the dealer
  (not 3), and the 3 bid does not exist in this variant at all:
  non-dealers must bid 5+, and a dealer facing an un-outbid standing 4
  takes the under for 4 or raises. Bidding still starts left of the
  dealer, dealer bids last; latest Haus still wins.
- **Scoring ports to team totals.** Bidding team makes → the team gets
  max(team tricks, bid); misses → the team loses the bid amount; the
  other team gets +1 per trick it won. Both teammates carry the team
  total, and the first team to 31 wins. Haus is still 12 points, but
  now it means the TEAM takes all 6 tricks.
- **Bot partner play.** Bots in teams mode don't waste cards on a trick
  their partner is already winning (second-hand-low convention), a
  bidding team's partner plays to keep the bid alive, and a Haus
  bidder's partner sweeps trump first just like the bidder.

## Haus exchange (teams only, added 2026-10-25)

An obscure rule for the teams variant. If a player bids Haus and wins
the bid, then BEFORE naming trump they may ask their partner for a
card:

1. The partner passes one card to the bidder.
2. The bidder, BEFORE looking at the passed card, passes one card back.
3. The partner then sits out the hand — they play no cards. The bidder
   names trump on the final hand and must take all 6 tricks alone
   (their partner's tricks stay at 0, so the team total is the bidder's
   alone). Tricks are played 3-handed, skipping the sitter's seat.
4. The exchange is optional: the bidder may decline and name trump on
   the dealt hand with their partner playing as normal.

Strategy, as Aaron described it:

- **When to ask:** only when one card away from a *cold* hand — a hand
  nobody can beat in any order, played alone. Typically five cold cards
  plus one weak back-end card of a non-trump suit (a 9 or queen, say)
  that the bidder hopes to upgrade. A hand is cold when every suit
  group in it is a top-prefix of that suit's ranking (trump: right bar
  > left bar > A > K > Q > 10 > 9; off-suits: A > K > Q > J > 10 > 9)
  and, if the hand holds any off-suit cards, it holds at least 4 trump
  (leading the trump prefix draws the opponents' trump out before the
  off-suit prefixes are led). Bots ask exactly when removing one card
  leaves such a skeleton with at least 3 trump, and pass back that weak
  card — the one being upgraded — committed before seeing the gift.
- **What the partner gives:** trump is named after the exchange, so the
  partner cannot condition on it — rank by universal value: an ace
  first, then a king, then a bar (any Jack is a bar under some trump),
  then the rest by value. The gift costs the partner nothing, since
  they sit out afterwards.

## Three Player variant (added 2026-10-25)

Three players (you + two bots) on a 21-card deck: every 9 is removed
except the 9 of hearts. Each player is dealt 6 cards and the remaining
3 go face down in the middle as the kitty.

- **Bidding like teams:** the dealer holds a standing 4 and bids last;
  non-dealers must bid 5+; a dealer facing an un-outbid standing 4
  takes the under for 4 or raises. Latest Haus still wins. Scoring is
  solo (each player's own tricks, first to 31).
- **Kitty after trump:** the bid winner names trump FIRST, then takes
  the kitty (9 cards in hand), then discards 3 cards that are not
  played, leaving 6 — and only then does play begin. The kitty is
  shown face down in the middle until taken (face up on the debug
  site). Kitty cards join the right end of the hand, sorted among
  themselves — they are not interleaved with the dealt cards. The hand
  row never wraps: cards shrink to fit one row, 6 or 9, at any screen
  width.
- **Bot play:** bots bid with a kitty bonus of 1.3 tricks — measured over
  5000 simulated hands, where actual tricks with the kitty fit
  1.32 + 1.01 × estimate. That bundles the kitty itself (+0.9 tricks:
  keep the best 6 of 9) and the estimate running low in 3-player play
  (+0.4: with only 2 opponents, winners are safer and trump is more
  dominant). The naming premium with a kitty is 0.5, so bid 5 starts at
  eff 4.5 (bid 6 keeps its 5.75 line): forced-bid measurement showed
  eff [4.5,4.75) hands make 69% when they bid 5. Bots also clear trump
  before leading a side Ace when they hold trump (all variants, not
  just 3P): measured in 3P, led Aces were trumped 42% of the time
  before clearing and 16% after; bid-5 makes rose 77% → 85% and bid-6
  41% → 86%; in 4P solo, bid-4 makes rose 84% → 98%. Bots discard
  their 3 weakest cards — trump kept over off-suit cards, bars kept
  over low trump, and singletons shed first to open voids.

## Sources

- Aaron's C++ repo `aarongoodermuth/haus` — `Rules.h/.cpp`, `Bet.h`,
  `Player.h/.cpp`, `Round.cpp`, `Game.cpp` (baseline for v0.1).
- Denexa Games, "Hasenpfeffer" — 25-card (joker + widow), bid 1–6, game to 10.
- White Knuckle Cards, "Pepper" — 24-card, Little/Big Pepper, game to 30, no-trump allowed.
- Owl Works, "Rabbit Stew and a Card Game Too!" — 24-card, "shoot the moon"
  with/without a partner card exchange (+12 / +24), game to 30.
- Wikipedia, "Bid Euchre" (Double Hasenpfeffer / Hawsy notes) and
  Pennsylvania Dutch Life (Hasenpfeffer) — 48-card double deck, 12 tricks,
  minimum bid 6, dealer stuck at 6, game to 62.

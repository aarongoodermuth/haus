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
- **Bidding:** One round, **starting at the dealer** and going clockwise
  (C++ `Round::DoBet`). Bids: pass, 3, 4, 5, 6, or **Haus**. Each bid must
  beat the current high bid.
  - *Haus* = a bid to take all 6 tricks, worth **12 points**. No card
    exchange, no playing alone — all four players play the hand out.
  - If everyone passes, the **dealer is forced to bid 3** ("screw the dealer").
- **Trump:** The high bidder names trump and leads the first trick.
  - Trump rank: **right bower** (Jack of trump) highest, then **left bower**
    (Jack of the other suit of the same colour — it counts as trump for
    winning tricks), then A, K, Q, 10, 9.
  - Other suits rank A, K, Q, J, 10, 9. No "no-trump" call.
- **Play:** Must follow the suit led if you can — judged by the card's
  **printed suit**, per C++ `Rules::FLegalPlay`. So the left bower does not
  satisfy following trump, and it *must* be played as its printed suit when
  that suit is led and it's your only card of it. For winning, though, it
  counts as trump. Highest trump wins the trick; otherwise the highest card
  of the suit led. Trick winner leads next.
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
4. **Left bower follow-suit** → By printed suit (C++ `FLegalPlay`), as
   described under Play above. Deliberately non-standard vs Euchre.
5. **No-trump / high / low calls?** → None. Trump is always a suit (C++).
6. **Game target** → 31 (C++ `FIsWinner`), winner is the high scorer.
7. **All-pass rule** → Dealer forced at 3 (C++ `DEFAULT_DEALER`).
8. **Joker / widow** → Neither (C++ deck is exactly 24 cards).
9. **Bid order** → Starts at the dealer, clockwise, one pass around
   (C++ `DoBet`). The dealer's forced-3 default applies if nobody outbids it.
10. **Who leads?** → High bidder leads the first trick (C++).

## One deliberate exception

11. **Trick comparison for off-suit cards.** C++ `CardComparer::FGreaterThan`
    compares non-trump cards by value alone, with no notion of the suit led —
    read literally, an off-suit Ace would beat a led 9. That looks like a bug
    in the unfinished C++ code rather than a house rule, so the web build
    uses the standard reading (off-suit cards can't win a trick) and flags it
    here. Easy to flip if that really was intended.

Everything else about the rules is deferred — Aaron will have more to say
later. This file is where rule changes get recorded as they're decided.

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

# Concept Index

Every concept explained in a `coding-questions/` write-up gets a row here,
and the row accumulates each question that used it. This is the file that
makes the directory worth more than the sum of its explanations: it turns
"I've seen this before" into a link.

Use it two ways:
- **Before writing an explanation** — if the concept already has a row,
  link to the best existing treatment and only write what *this* question
  adds, rather than explaining prefix sums for the fourth time.
- **When revising** — a concept with four questions attached and a shaky
  confidence rating is the thing to drill next.

## Concepts
| concept | best explanation | also used in | repo cross-refs | confidence |
|---------|------------------|--------------|-----------------|:----------:|
| range covering on a line | [ct-01 §2.1](contests/abc-475/CONTEST.md#21-range-covering-on-a-line-prefix-sums--binary-search--used-in-c) | | | |
| segmented sieve | [ct-01 §2.2](contests/abc-475/CONTEST.md#22-segmented-sieve--used-in-d) | | | |
| canonical coin systems | [ct-01 §2.3](contests/abc-475/CONTEST.md#23-canonical-coin-systems--digit-decomposition--used-in-b) | | | |
| binary search with the STL | [ct-01 §2.4](contests/abc-475/CONTEST.md#24-binary-search-with-the-stl--used-in-c) | | | |

## Column notes
- **concept** — a short, stable, lowercase tag. Reuse an existing string
  rather than coining a near-duplicate (`0/1 knapsack`, not
  `knapsack (0/1)`).
- **best explanation** — the one write-up to re-read first. Usually where
  the concept was treated in most depth, not necessarily the first time it
  appeared.
- **also used in** — `cq-NNN` / `ct-NN` ids, not paths.
- **repo cross-refs** — deeper treatments elsewhere: `concepts/`,
  `system-design/`, `game-development/roadmap/`.
- **confidence** — 1–5, set by you, not by the agent. Blank until you say.

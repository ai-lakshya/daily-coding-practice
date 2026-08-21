# System Design Index

The map for the `system-design/` track. Every topic doc is listed here.
Read this before starting a session — it's how you tell a new topic from
one that already has a doc to extend.

## Topics
| id | sub-topic | topic | doc | sessions | confidence | first_logged | last_updated |
|----|-----------|-------|-----|---------:|:----------:|--------------|--------------|
| sd-001 | databases | Transaction Isolation Levels | `databases/transaction-isolation-levels.md` | 1 | pending | 2026-08-19 | 2026-08-21 |
| sd-002 | databases | Transactions & ACID Properties | `databases/transactions-acid.md` | 1 | pending | 2026-08-21 | 2026-08-21 |

## Canonical sub-topic directories
Pick the closest fit. Create a new directory only if a topic genuinely
fits none of these — and add it to this list in the same session.

| directory | covers |
|-----------|--------|
| `fundamentals/` | Requirements gathering, back-of-envelope estimation, latency numbers, design process, non-functional requirements |
| `networking/` | DNS, TCP/UDP, HTTP versions, TLS, CDNs, proxies (forward/reverse), WebSockets, gRPC |
| `scaling/` | Vertical vs horizontal, statelessness, load balancing, autoscaling, sharding strategy at the app tier |
| `databases/` | SQL vs NoSQL, indexing, normalization, transactions & ACID, isolation levels, replication, partitioning/sharding, query performance |
| `caching/` | Cache-aside / write-through / write-back, eviction policies, TTLs, invalidation, CDN & browser caching, Redis/Memcached |
| `consistency/` | CAP, PACELC, consistency models, quorums, consensus (Raft/Paxos), distributed transactions, 2PC, sagas |
| `messaging/` | Queues, pub/sub, Kafka, event-driven architecture, delivery semantics, backpressure, dead-letter queues |
| `distributed-systems/` | Consistent hashing, leader election, clocks & ordering, idempotency, gossip, service discovery |
| `reliability/` | Redundancy & failover, circuit breakers, retries & backoff, bulkheads, graceful degradation, disaster recovery, SLIs/SLOs/SLAs |
| `observability/` | Metrics, logging, distributed tracing, alerting, health checks |
| `security/` | AuthN vs AuthZ, sessions/JWT/OAuth, rate limiting, encryption at rest & in transit, common attack surfaces |
| `architecture/` | Monolith vs microservices, API design (REST/GraphQL/RPC), API gateways, CQRS, event sourcing, BFF, serverless |
| `storage/` | Object/blob storage, file systems, data lakes & warehouses, search engines, time-series stores |
| `case-studies/` | End-to-end designs — URL shortener, news feed, chat, rate limiter, video streaming, etc. |

## Conventions
- Doc path: `system-design/<sub-topic>/<kebab-slug>.md` — no date prefix.
  These are living topic notes that get extended across sessions, unlike
  the dated one-off notes in `concepts/`.
- ids are sequential `sd-NNN`, zero-padded, never reused.
- One topic = one doc. A second session on the same topic appends a
  `### Session N` block to `## Discussion Log`; it never forks a new file.
- New docs start from `system-design/_TEMPLATE.md`.
- `confidence` is the user's own 1-5 self-rating from the latest session.

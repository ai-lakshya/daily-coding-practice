/*
    Problem link: https://atcoder.jp/contests/abc475/tasks/abc475_e

    Solution idea (same partition_point descent as e.cpp, without the per-query sort):
    1. The undetermined set is always exactly the participants sharing one fixed
       prefix, so sorting the correctness bitstrings descending makes it a contiguous
       block, and each question splits that block with one partition_point. That part
       of e.cpp was already O(K log N) per query -- the predicate is O(1).
    2. What blew the budget was rebuilding the sorted array every query: the copy is
       O(N*K) and the sort is O(N log N) string comparisons at O(K) each.
    3. The flips are all known upfront, so simulate them offline and collect every key
       any participant will ever hold -- at most N + Q of them. Sort that ONCE. Ranks
       in this static array never move again, so partition_point can binary-search it
       directly, exactly as before.
    4. Which participants are actually *present* at a given rank does change, so a
       Fenwick tree over ranks holds the live counts. Block [lo, hi) at question d
       splits at rank p = partition_point(...); the number of undetermined who got d
       right is the live count in [lo, p), one Fenwick prefix query. Since lo only
       ever moves to p, the prefix at lo carries over -- one query per question.
    5. Qualifiers are always a rank-prefix, so participant i passes iff the number of
       live participants ranked at or above them is <= the qualifier count. That is
       e.cpp's `it <= upper` test, with `it` still the last occurrence of their key.

    Complexity: O((N + Q) log(N + Q)) setup, O(Q * K * log(N + Q)) queries.
*/

#include <algorithm>
#include <array>
#include <cstdint>
#include <functional>
#include <iostream>
#include <string>
#include <vector>

// K <= 200 bits. Question d lives in word d/64 at bit 63-(d%64), so comparing the
// words most-significant-first is the same as comparing the bitstrings lexicographically.
using Key = std::array<std::uint64_t, 4>;

inline int getBit(const Key& key, int d)
{
    return static_cast<int>((key[d >> 6] >> (63 - (d & 63))) & 1ULL);
}

inline void flipBit(Key& key, int d)
{
    key[d >> 6] ^= (1ULL << (63 - (d & 63)));
}

// Live participant count per rank, so the descent can ask "how many are present in
// this rank range" while the static key array stays fixed.
class Fenwick
{
public:
    explicit Fenwick(int size) : tree(size + 1, 0) {}

    void add(int i, int delta)
    {
        for(i++ ; i < static_cast<int>(tree.size()) ; i += i & -i)
            tree[i] += delta;
    }

    // number of live participants at ranks [0, i)
    int prefix(int i) const
    {
        int sum = 0;
        for( ; i > 0 ; i -= i & -i)
            sum += tree[i];

        return sum;
    }

private:
    std::vector<int> tree;
};

int main()
{
    std::ios::sync_with_stdio(false);
    std::cin.tie(nullptr);

    int n, m, k;
    std::cin >> n >> m >> k;

    std::string T;
    std::cin >> T;

    // correctness bitstring, packed
    std::vector<Key> cur(n, Key{});
    for(int i = 0 ; i < n ; i++)
    {
        std::string temp;
        std::cin >> temp;

        for(int d = 0 ; d < k ; d++)
            if(temp[d] == T[d])
                flipBit(cur[i], d);
    }

    int q;
    std::cin >> q;

    std::vector<int> qWho(q), qBit(q);
    for(int t = 0 ; t < q ; t++)
    {
        std::cin >> qWho[t] >> qBit[t];
        qWho[t]--;
        qBit[t]--;
    }

    // Every key that will ever exist: the N starting ones plus the one each flip creates.
    std::vector<Key> keys;
    keys.reserve(static_cast<std::size_t>(n) + q);
    keys.insert(keys.end(), cur.begin(), cur.end());

    {
        std::vector<Key> sim = cur;
        for(int t = 0 ; t < q ; t++)
        {
            flipBit(sim[qWho[t]], qBit[t]);
            keys.push_back(sim[qWho[t]]);
        }
    }

    // The one and only sort.
    std::sort(keys.begin(), keys.end(), std::greater<Key>());
    keys.erase(std::unique(keys.begin(), keys.end()), keys.end());

    auto keyCount = static_cast<int>(keys.size());

    auto rankOf = [&keys](const Key& key)
    {
        return static_cast<int>(
            std::lower_bound(keys.begin(), keys.end(), key, std::greater<Key>()) - keys.begin());
    };

    // Resolve each query's before/after rank offline so the hot loop does no key lookups.
    std::vector<int> rank(n);
    for(int i = 0 ; i < n ; i++)
        rank[i] = rankOf(cur[i]);

    std::vector<int> qOld(q), qNew(q);
    {
        std::vector<Key> sim = cur;
        std::vector<int> simRank = rank;

        for(int t = 0 ; t < q ; t++)
        {
            int who = qWho[t];
            qOld[t] = simRank[who];

            flipBit(sim[who], qBit[t]);
            simRank[who] = rankOf(sim[who]);
            qNew[t] = simRank[who];
        }
    }

    Fenwick live(keyCount);
    for(int i = 0 ; i < n ; i++)
        live.add(rank[i], 1);

    std::string out;
    out.reserve(static_cast<std::size_t>(q) * 4);

    for(int t = 0 ; t < q ; t++)
    {
        // the flip persists, so move this participant to their new rank for good
        live.add(qOld[t], -1);
        live.add(qNew[t], 1);

        int lo = 0;
        int hi = keyCount;
        int qualified = 0;
        int prefLo = 0;         // live count at ranks [0, lo), carried down the descent

        for(int d = 0 ; d < k ; d++)
        {
            // Same split as e.cpp, but on the static key array: first rank in the
            // block whose key has question d wrong.
            auto pred = [d](const Key& key){ return getBit(key, d) == 1; };
            auto it = std::partition_point(keys.begin() + lo, keys.begin() + hi, pred);

            auto p = static_cast<int>(it - keys.begin());
            int prefP = live.prefix(p);
            int correct = prefP - prefLo;

            if(qualified + correct <= m)
            {
                // they all fit -- promote them, undetermined continue on the '0' side
                qualified += correct;
                lo = p;
                prefLo = prefP;
            }
            else
            {
                // too many -- the ones who got d wrong are out
                hi = p;
            }
        }

        // qualifiers are the top `qualified` by rank, and a tie group never straddles
        // that boundary, so testing this participant's last occurrence is enough
        out += (live.prefix(qNew[t] + 1) <= qualified) ? "Yes\n" : "No\n";
    }

    std::cout << out;

    return 0;
}

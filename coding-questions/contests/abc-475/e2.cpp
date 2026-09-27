/*
    Problem link: https://atcoder.jp/contests/abc475/tasks/abc475_e

    Solution idea:
    1. Reduce every participant to a correctness bitstring C[i], where C[i][d] = '1'
       iff they answered question d the same as T. Only these bitstrings matter.
    2. Key structural fact: the "still undetermined" set is always exactly the set of
       participants sharing one fixed prefix. Step d either keeps the '1' side
       (qualify them) or keeps the '0' side (eliminate the rest) -- either way the
       survivors share one more prefix bit. So the whole procedure is a single walk
       down the bits, and the qualifiers are the blocks branching off it.
    3. Sorting descending and running partition_point per question makes that walk
       explicit, but re-sorting on every query is O(Q * N log N * K). A binary trie
       keyed on the bitstring stores the same order implicitly: the count of
       participants under prefix P is one node lookup, so the walk is O(K) and a
       query's bit flip is an O(K) erase + insert.
    4. After the walk, "path" holds the prefix the undetermined group carried at each
       step. Participant i qualifies iff C[i] first differs from path at some d where
       C[i][d] == '1' (that is the branch that was qualified at step d). Matching the
       whole path means they were never resolved, which counts as not qualifying.

    Complexity: O((N + Q) * K) time, O(N * K) trie nodes.
*/

#include <array>
#include <iostream>
#include <string>
#include <vector>

// Binary trie over the correctness bitstrings, one level per question.
// Node 0 is a null sentinel (count 0, children 0) so that walking off the end
// of the trie keeps reporting empty without a special case. The root is node 1.
class PrefixTrie
{
public:
    PrefixTrie(int questions, int participants) : K(questions)
    {
        auto cap = static_cast<std::size_t>(participants) * questions + 2;
        ch.reserve(cap);
        cnt.reserve(cap);

        newNode();  // 0 -> null sentinel
        newNode();  // 1 -> root
    }

    void insert(const std::string& s)
    {
        int cur = root;
        cnt[cur]++;

        for(int d = 0 ; d < K ; d++)
        {
            int b = s[d] - '0';
            if(ch[cur][b] == 0)
                ch[cur][b] = newNode();

            cur = ch[cur][b];
            cnt[cur]++;
        }
    }

    void erase(const std::string& s)
    {
        int cur = root;
        cnt[cur]--;

        for(int d = 0 ; d < K ; d++)
        {
            int b = s[d] - '0';
            int nxt = ch[cur][b];
            cnt[nxt]--;

            // Count hit zero, so nothing else lives under nxt -- the rest of the
            // subtree is exactly this string's remaining path. Detach and recycle it.
            if(cnt[nxt] == 0)
            {
                ch[cur][b] = 0;
                recycle(nxt, s, d + 1);
                return;
            }

            cur = nxt;
        }
    }

    // Runs the qualification procedure and writes the boundary prefix into path:
    // path[d] is the bit the still-undetermined group carries after question d.
    void walk(int m, std::string& path) const
    {
        int cur = root;
        int qualified = 0;

        for(int d = 0 ; d < K ; d++)
        {
            // Undetermined participants who got question d right.
            int correct = cnt[ch[cur][1]];

            if(qualified + correct <= m)
            {
                // They all fit -- qualify them, the undetermined carry on with '0'.
                qualified += correct;
                path[d] = '0';
            }
            else
            {
                // Too many -- the ones who got it wrong are out, survivors carry '1'.
                path[d] = '1';
            }

            cur = ch[cur][path[d] - '0'];
        }
    }

private:
    static constexpr int root = 1;

    int K;
    std::vector<std::array<int, 2>> ch;
    std::vector<int> cnt;
    std::vector<int> freeList;

    int newNode()
    {
        if(!freeList.empty())
        {
            // recycle() already zeroed this node's counters and children.
            int id = freeList.back();
            freeList.pop_back();
            return id;
        }

        ch.push_back({0, 0});
        cnt.push_back(0);
        return static_cast<int>(ch.size()) - 1;
    }

    void recycle(int node, const std::string& s, int d)
    {
        while(node != 0)
        {
            int nxt = (d < K) ? ch[node][s[d] - '0'] : 0;

            cnt[node] = 0;
            ch[node] = {0, 0};
            freeList.push_back(node);

            node = nxt;
            d++;
        }
    }
};

// Participant qualifies iff their bitstring leaves the boundary path on a '1'
// branch -- that is the group that was promoted at that question.
bool qualifies(const std::string& c, const std::string& path)
{
    auto k = static_cast<int>(path.size());

    for(int d = 0 ; d < k ; d++)
        if(c[d] != path[d])
            return c[d] == '1';

    // Never diverged: still undetermined when the questions ran out.
    return false;
}

int main()
{
    std::ios::sync_with_stdio(false);
    std::cin.tie(nullptr);

    int n, m, k;
    std::cin >> n >> m >> k;

    std::string T;
    std::cin >> T;

    // only store the correctness strings here.
    std::vector<std::string> C;
    C.reserve(n);

    PrefixTrie trie(k, n);

    for(int i = 0 ; i < n ; i++)
    {
        std::string temp;
        std::cin >> temp;

        for(int j = 0 ; j < k ; j++)
            temp[j] = (temp[j] == T[j]) ? '1' : '0';

        trie.insert(temp);
        C.push_back(std::move(temp));
    }

    int q;
    std::cin >> q;

    std::string path(k, '0');
    std::string out;
    out.reserve(static_cast<std::size_t>(q) * 4);

    while(q--)
    {
        int i, j;
        std::cin >> i >> j;
        i--;
        j--;

        // The flip persists into later queries, so update the trie for real.
        trie.erase(C[i]);
        C[i][j] = (C[i][j] == '1') ? '0' : '1';
        trie.insert(C[i]);

        trie.walk(m, path);
        out += qualifies(C[i], path) ? "Yes\n" : "No\n";
    }

    std::cout << out;

    return 0;
}

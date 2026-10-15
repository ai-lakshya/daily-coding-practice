/*
    Problem link: https://atcoder.jp/contests/aalc001/tasks/aalc001_b
*/

#include <algorithm>
#include <array>
#include <climits>
#include <iostream>
#include <numeric>

using ll = long long;

namespace cts
{
    inline constexpr ll arrSize = 1'000'005;
    inline constexpr ll treeSize = 4'000'020;
    inline constexpr ll deadValue = LLONG_MAX;
    inline constexpr bool oneBased = true;
    inline constexpr bool includeRightEnd = true;
}

std::array<ll, cts::arrSize> nums;
std::array<ll, cts::treeSize> tree;
// NEW: lazy[v] = amount added to every element under v that v's children have not received yet.
std::array<ll, cts::treeSize> lazy;

// read input and initialize the numsArray by modifying this function.
void initializeNums(ll n)
{
    for(ll i = 0 + cts::oneBased ; i < n + cts::oneBased ; i++)
        std::cin >> nums[i];
}

// predicate function (should be an associative operation, (a+b)+c == a+(b+c)).
ll pred(ll leftVal, ll rightVal)
{
    // NEW: min is associative, so tree[v] is the minimum of v's range. was leftVal + rightVal (sum).
    return std::min(leftVal, rightVal);
}

// helper functions to find the left and right subtree indexes in the segment tree.
// modify these when switching between 0-based and 1-based indexing.
ll leftTreeInd(ll treeInd)
{
    if(cts::oneBased)
        return 2 * treeInd; // 1-based.
    else
        return 2 * treeInd + 1; // 0-based.
}

ll rightTreeInd(ll treeInd)
{
    if(cts::oneBased)
        return 2 * treeInd + 1; // 1-based.
    else
        return 2 * treeInd + 2; // 0-based.
}

// make tree function. uses the global arrays so make sure initialization takes place correctly.
ll makeTree(ll L, ll R, ll treeInd)
{
    if(L == R)
    {
        lazy[treeInd] = 0;
        return tree[treeInd] = nums[L];
    }

    ll mid = std::midpoint(L, R);
    ll leftInd = leftTreeInd(treeInd);
    ll rightInd = rightTreeInd(treeInd);
    ll leftVal = makeTree(L, mid, leftInd);
    ll rightVal = makeTree(mid+1, R, rightInd);
    lazy[treeInd] = 0;
    return tree[treeInd] = pred(leftVal, rightVal);
}

// add a value to an entire range of the segment tree.
// NEW: comment corrected. We push lazy values down while recursing, not only in getVal.
ll setVal(ll L, ll R, std::pair<ll,ll> targetInd, ll targetVal, ll treeInd)
{
    // no intersection of the range with this node.
    if(L > targetInd.second || R < targetInd.first)
    {
        return cts::deadValue;
    }

    // this entire node falls in the target range.
    if(L >= targetInd.first && R <= targetInd.second)
    {
        // minimum increases exactly by the number added to the range.
        lazy[treeInd] += targetVal;
        return tree[treeInd] += targetVal;
    }

    ll mid = std::midpoint(L, R);
    ll leftInd = leftTreeInd(treeInd);
    ll rightInd = rightTreeInd(treeInd);

    // NEW: partial overlap, so push this node's pending add into its children before reading or changing them.
    if(lazy[treeInd] != 0)
    {
        tree[leftInd] += lazy[treeInd];
        lazy[leftInd] += lazy[treeInd];
        tree[rightInd] += lazy[treeInd];
        lazy[rightInd] += lazy[treeInd];
        lazy[treeInd] = 0;
    }

    // NEW: the range can cover both children, so recurse into both. was a single if/else on one index.
    setVal(L, mid, targetInd, targetVal, leftInd);
    setVal(mid + 1, R, targetInd, targetVal, rightInd);

    // after updating the two subtrees, update the value upstream as well.
    // NEW: read the children from tree[] because the recursive calls above return values we no longer use.
    return tree[treeInd] = pred(tree[leftInd], tree[rightInd]);
}

// get the value from the segment tree for a segment.
// [TL, TR] represents the segment from the query whose value is needed.
// [L, R] is the segment which the current tree node refers to.
ll getVal(ll TL, ll TR, ll L, ll R, ll treeInd)
{
    // this node is completely outside of the query interval.
    if(R < TL || TR < L)
        return cts::deadValue;

    if(TL <= L && TR >= R)
        return tree[treeInd];

    ll mid = std::midpoint(L, R);
    ll leftInd = leftTreeInd(treeInd);
    ll rightInd = rightTreeInd(treeInd);

    // NEW: partial overlap, so push this node's pending add into its children before reading them.
    if(lazy[treeInd] != 0)
    {
        tree[leftInd] += lazy[treeInd];
        lazy[leftInd] += lazy[treeInd];
        tree[rightInd] += lazy[treeInd];
        lazy[rightInd] += lazy[treeInd];
        lazy[treeInd] = 0;
    }

    ll leftVal = getVal(TL, TR, L, mid, leftInd);
    ll rightVal = getVal(TL, TR, mid + 1, R, rightInd);
    return pred(leftVal, rightVal);
}

int main()
{
    std::ios::sync_with_stdio(false);
    std::cin.tie(nullptr);

    ll n, m;
    std::cin >> n >> m;

    initializeNums(n);
    makeTree(0 + cts::oneBased, n-1 + cts::oneBased, cts::oneBased);

    while(m--)
    {
        ll q, a, b;
        std::cin >> q >> a >> b;

        if(q == 1)
        {
            // NEW: a type-1 query has one more number, x, which is added to every element in [a, b].
            ll x;
            std::cin >> x;
            setVal(0 + cts::oneBased, n-1 + cts::oneBased, {a, b}, x, cts::oneBased);
        }
        else
        {
            std::cout << getVal(a, b-1 + cts::includeRightEnd, 0 + cts::oneBased, n-1 + cts::oneBased, cts::oneBased) << "\n";
        }
    }

    return 0;
}

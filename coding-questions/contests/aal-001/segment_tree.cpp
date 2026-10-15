/*
    Header file contains implementation for segment tree.
    Copy this to the top of the solution where needed and make appropriate changes.

    This file contains a demo with the sum predicate, modify when needed accordingly.
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
    inline constexpr ll deadValue = 0;
    inline constexpr bool oneBased = false;
    inline constexpr bool includeRightEnd = false;
}

std::array<ll, cts::arrSize> nums;
std::array<ll, cts::treeSize> tree;

// read input and initialize the numsArray by modifying this function.
void initializeNums(ll n)
{
    for(ll i = 0 + cts::oneBased ; i < n + cts::oneBased ; i++)
        std::cin >> nums[i];
}

// predicate function (should be an associative operation, (a+b)+c == a+(b+c)).
ll pred(ll leftVal, ll rightVal)
{
    return leftVal + rightVal;
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
        return tree[treeInd] = nums[L];

    ll mid = std::midpoint(L, R);
    ll leftInd = leftTreeInd(treeInd);
    ll rightInd = rightTreeInd(treeInd);
    ll leftVal = makeTree(L, mid, leftInd);
    ll rightVal = makeTree(mid+1, R, rightInd);
    return tree[treeInd] = pred(leftVal, rightVal);
}

// set a single value in the segment tree.
ll setVal(ll L, ll R, ll targetInd, ll targetVal, ll treeInd)
{
    // found the target node and made the changes
    if(L == R && L == targetInd)
        return tree[treeInd] = targetVal;
    
    // make the changes in the two subtrees recursively.
    ll mid = std::midpoint(L, R);
    ll leftInd = leftTreeInd(treeInd);
    ll rightInd = rightTreeInd(treeInd);
    ll leftVal = tree[leftInd];
    ll rightVal = tree[rightInd];
    if(targetInd >= L && targetInd <= mid)
        leftVal = setVal(L, mid, targetInd, targetVal, leftInd);
    else
        rightVal = setVal(mid + 1, R, targetInd, targetVal, rightInd);
    
    // after updating the two subtrees, update the value upstream as well.
    return tree[treeInd] = pred(leftVal, rightVal);
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
            setVal(0 + cts::oneBased, n-1 + cts::oneBased, a, b, cts::oneBased);
        }
        else
        {
            std::cout << getVal(a, b-1 + cts::includeRightEnd, 0 + cts::oneBased, n-1 + cts::oneBased, cts::oneBased) << "\n";
        }
    }

    return 0;
}


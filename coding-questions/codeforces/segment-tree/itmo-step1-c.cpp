/*
    Problem link: https://codeforces.com/edu/course/2/lesson/4/1/practice/contest/273169/problem/C

*/

#include <climits>
#include <iostream>
#include <numeric>
#include <vector>

using ll = long long;

// predicate functon for the segment tree.
// it needs to perform an associative operation.
std::pair<ll, ll> pred(std::pair<ll, ll> leftVal, std::pair<ll, ll> rightVal)
{
    std::pair<ll, ll> ans;
    if(leftVal.first == rightVal.first)
    {
        ans.first = leftVal.first;
        ans.second = leftVal.second + rightVal.second;
    }
    else if(leftVal.first < rightVal.first)
    {
        return leftVal;
    }
    else
    {
        return rightVal;
    }

    return ans;
}

// construct the tree from the input array.
std::pair<ll, ll> makeTree(const std::vector<ll>& nums, std::vector<std::pair<ll, ll>>& tree, ll L, ll R, ll currTreeInd = 0)
{
    // we have reached a leaf node.
    if(L == R)
    {
        tree[currTreeInd].first = nums[L];
        tree[currTreeInd].second = 1;
        return tree[currTreeInd];
    }

    // recursively make the two child trees.
    ll mid = std::midpoint(L, R);
    auto leftVal = makeTree(nums, tree, L, mid, currTreeInd * 2 + 1);
    auto rightVal = makeTree(nums, tree, mid + 1, R, currTreeInd * 2 + 2);
    return tree[currTreeInd] = pred(leftVal, rightVal);
}

// set the value of a particular index to a new value.
// we would also then need to update the values of all nodes upstream recursively.
std::pair<ll, ll> set(const std::vector<ll>& nums, std::vector<std::pair<ll, ll>>& tree, ll L, ll R, ll changeInd, ll newVal, ll currTreeInd = 0)
{
    // we have reached the index where change needs to be made.
    if(L == R)
    {
        tree[currTreeInd].first = newVal;
        tree[currTreeInd].second = 1;
        return tree[currTreeInd];
    }

    // recursively make the change in the two child trees.
    ll mid = std::midpoint(L, R);
    ll leftInd = currTreeInd * 2 + 1;
    ll rightInd = currTreeInd * 2 + 2;
    auto leftVal = tree[leftInd];
    auto rightVal = tree[rightInd];
    if(L <= changeInd && mid >= changeInd)
        leftVal = set(nums, tree, L, mid, changeInd, newVal, leftInd);
    else
        rightVal = set(nums, tree, mid + 1, R, changeInd, newVal, rightInd);
    return tree[currTreeInd] = pred(leftVal, rightVal);
}

// return the value of the predicate function for any given [l, r] segment on the input array.
std::pair<ll, ll> getVal(const std::vector<ll>& nums, std::vector<std::pair<ll, ll>>& tree, ll TL, ll TR, ll L, ll R, ll currTreeInd = 0)
{
    // this node is completely outside of the query interval.
    if(R < TL || TR < L)
        return {LLONG_MAX, 0};

    if(TL <= L && TR >= R)
        return tree[currTreeInd];

    ll mid = std::midpoint(L, R);
    ll leftInd = currTreeInd * 2 + 1;
    ll rightInd = currTreeInd * 2 + 2;

    auto leftVal = getVal(nums, tree, TL, TR, L, mid, leftInd);
    auto rightVal = getVal(nums, tree, TL, TR, mid + 1, R, rightInd);
    return pred(leftVal, rightVal);
}

int main()
{
    ll n, m;
    std::cin >> n >> m;

    std::vector<ll> nums(n);
    for(ll i = 0 ; i < n ; i++)
        std::cin >> nums[i];

    // Setup the segment tree before processing the queries.
    std::vector<std::pair<ll, ll>> tree(4 * n);
    makeTree(nums, tree, 0, n - 1);
    
    while(m--)
    {
        int q, a, b;
        std::cin >> q >> a >> b;

        if(q == 1)
        {
            set(nums, tree, 0, n - 1, a, b);
        }
        else
        {
            auto val = getVal(nums, tree, a, b - 1, 0, n - 1);
            std::cout << val.first << " " << val.second << "\n";
        }
    }

    return 0;
}
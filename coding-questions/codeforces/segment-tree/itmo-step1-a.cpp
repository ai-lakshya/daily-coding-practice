/*
    Problem link: https://codeforces.com/edu/course/2/lesson/4/1/practice/contest/273169/problem/A
    Solution link: 

    Time: 
    Space: 
*/

#include <iostream>
#include <numeric>
#include <vector>

using ll = long long;

// predicate functon for the segment tree.
// it needs to perform an associative operation.
ll pred(ll leftVal, ll rightVal)
{
    return leftVal + rightVal;
}

// construct the tree from the input array.
ll makeTree(const std::vector<ll>& nums, std::vector<ll>& tree, ll L, ll R, ll currTreeInd = 0)
{
    // we have reached a leaf node.
    if(L == R)
        return tree[currTreeInd] = nums[L];

    // recursively make the two child trees.
    ll mid = std::midpoint(L, R);
    ll leftVal = makeTree(nums, tree, L, mid, currTreeInd * 2 + 1);
    ll rightVal = makeTree(nums, tree, mid + 1, R, currTreeInd * 2 + 2);
    return tree[currTreeInd] = pred(leftVal, rightVal);
}

// set the value of a particular index to a new value.
// we would also then need to update the values of all nodes upstream recursively.
ll set(const std::vector<ll>& nums, std::vector<ll>& tree, ll L, ll R, ll changeInd, ll newVal, ll currTreeInd = 0)
{
    // we have reached the index where change needs to be made.
    if(L == R)
        return tree[currTreeInd] = newVal;

    // recursively make the change in the two child trees.
    ll mid = std::midpoint(L, R);
    ll leftInd = currTreeInd * 2 + 1;
    ll rightInd = currTreeInd * 2 + 2;
    ll leftVal = tree[leftInd];
    ll rightVal = tree[rightInd];
    if(L <= changeInd && mid >= changeInd)
        leftVal = set(nums, tree, L, mid, changeInd, newVal, leftInd);
    else
        rightVal = set(nums, tree, mid + 1, R, changeInd, newVal, rightInd);
    return tree[currTreeInd] = pred(leftVal, rightVal);
}

// return the value of the predicate function for any given [l, r] segment on the input array.
ll getVal(const std::vector<ll>& nums, std::vector<ll>& tree, ll TL, ll TR, ll L, ll R, ll currTreeInd = 0)
{
    // this node is completely outside of the query interval.
    if(R < TL || TR < L)
        return 0;

    if(TL <= L && TR >= R)
        return tree[currTreeInd];

    ll mid = std::midpoint(L, R);
    ll leftInd = currTreeInd * 2 + 1;
    ll rightInd = currTreeInd * 2 + 2;

    ll leftVal = getVal(nums, tree, TL, TR, L, mid, leftInd);
    ll rightVal = getVal(nums, tree, TL, TR, mid + 1, R, rightInd);
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
    std::vector<ll> tree(4 * n, 0);
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
            std::cout << getVal(nums, tree, a, b - 1, 0, n - 1) << "\n";
        }
    }

    return 0;
}
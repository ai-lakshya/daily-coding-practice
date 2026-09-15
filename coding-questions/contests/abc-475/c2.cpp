/*
    Problem link: https://atcoder.jp/contests/abc475/tasks/abc475_c
    Solution idea:
    Approach 2: O(nlogn) time using binary search on prefix sum arrays.
*/

#include <cmath>
#include <iostream>
#include <numeric>
#include <vector>

using ll = long long;

// Go as much left as possible then go right
ll leftCountFirst(ll startPos, ll totalCost, std::vector<ll>& a, std::vector<ll>& pre, std::vector<ll>& post)
{
    auto n = pre.size();
    ll maxVisCount = 1;
    for(auto i = startPos - 1 ; i >= 0 ; i--)
    {
        ll totalCostLocal = totalCost;
        ll minLeft = startPos;
        ll maxRight = startPos;

        // how much it costs to go from startPos -> i.
        ll costLeft = post[i] - post[startPos];
        if(costLeft > totalCost)
            break;

        minLeft = i;
        totalCostLocal -= costLeft;

        // try to go right as much as possible with remaining cost using binary search.
        ll left = minLeft;
        ll right = n;
        while(left != right - 1)
        {
            auto curr = std::midpoint(left, right);
            if(pre[curr] - pre[minLeft] <= totalCostLocal)
                left = curr;
            else
                right = curr;
        }

        if(left == minLeft)
            maxRight = startPos;
        else
            maxRight = std::max(startPos, left);
        
        maxVisCount = std::max(maxVisCount, maxRight - minLeft + 1);
    }

    return maxVisCount;
}

// Go as much right as possible then go right
ll rightCountFirst(ll startPos, ll totalCost, std::vector<ll>& a, std::vector<ll>& pre, std::vector<ll>& post)
{
    auto n = pre.size();
    ll maxVisCount = 1;
    for(auto i = startPos + 1 ; i < n ; i++)
    {
        ll totalCostLocal = totalCost;
        ll minLeft = startPos;
        ll maxRight = startPos;

        // how much it costs to go from startPos -> i.
        ll costRight = pre[i] - pre[startPos];
        if(costRight > totalCost)
            break;

        maxRight = i;
        totalCostLocal -= costRight;

        // try to go left as much as possible with remaining cost using binary search.
        ll left = -1;
        ll right = maxRight;
        while(left != right - 1)
        {
            auto curr = std::midpoint(left, right);
            if(post[curr] - post[maxRight] <= totalCostLocal)
                right = curr;
            else
                left = curr;
        }

        if(right == maxRight)
            minLeft = startPos;
        else
            minLeft = std::min(startPos, right);
        
        maxVisCount = std::max(maxVisCount, maxRight - minLeft + 1);
    }

    return maxVisCount;
}

int main()
{
    ll n, s, l;
    std::cin >> n >> s >> l;

    std::vector<ll> a(n-1);
    for(auto i = 0 ; i < n-1 ; i++)
        std::cin >> a[i];

    // Build a prefix sum array.
    std::vector<ll> pre(n, 0);
    for(auto i = 1 ; i < n ; i++)
        pre[i] = pre[i-1] + a[i-1];

    // build a postfix sum array.
    std::vector<ll> post(n, 0);
    for(auto i = n - 2 ; i >= 0 ; i--)
        post[i] = post[i+1] + a[i];

    std::cout << std::max(leftCountFirst(s-1, l, a, pre, post), rightCountFirst(s-1, l, a, pre, post));

    return 0;
}
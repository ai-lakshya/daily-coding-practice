/*
    Problem link: https://atcoder.jp/contests/dp/tasks/dp_d

    Solution idea:
    1. Recursion of the type: either choose item-i or skip it. Then memoize because of constraints
    2. Dynamic Programming.
*/

#include <cmath>
#include <iostream>
#include <vector>

// item:        for which choice is to be made.
// currWeight:  accumulated weight before making a choice for item.
// totalWeight: total weight allowed in the knapsack.
// weights:     input weight and value vector.
long long rec(long long item, long long currWeight, long long totalWeight, const std::vector< std::vector<long long> >& weights, std::vector< std::vector<long long> >& memo)
{
    auto n = weights.size();
    if(item == n-1)
    {
        // take last item.
        if(currWeight + weights[item][0] <= totalWeight)
            return weights[item][1];
        // notTake last item.
        else
            return 0;
    }

    if(memo[item][currWeight] != -1)
        return memo[item][currWeight];

    long long take, notTake, ans;
    
    notTake = rec(item+1, currWeight, totalWeight, weights, memo);
    if(currWeight + weights[item][0] <= totalWeight)
    {
        take = weights[item][1] + rec(item+1, currWeight+weights[item][0], totalWeight, weights, memo);
        ans = std::max(take, notTake);
    }
    else
    {
        ans = notTake;
    }
    
    return memo[item][currWeight] = ans;
}

int main()
{
    // Input.
    long long n, w;
    std::cin >> n >> w;

    std::vector< std::vector<long long> > weights(n, std::vector<long long>(2));
    for(auto i = 0 ; i < n ; i++)
        std::cin  >> weights[i][0] >> weights[i][1];

    // Solution.
    std::vector< std::vector<long long> > memo(n, std::vector<long long>(w, -1));
    std::cout << rec(0, 0, w, weights, memo);

    return 0;
}
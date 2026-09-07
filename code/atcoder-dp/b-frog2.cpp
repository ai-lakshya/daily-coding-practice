/*
    Problem link - https://atcoder.jp/contests/dp/tasks/dp_b

    Solution Idea:
    1. Same concept as problem-a, just instead of step size of {1, 2} now the step sizes are {1, 2, ..., k}

    Time: O(nk)
    Space: O(k)
*/

#include <climits>
#include <iostream>
#include <vector>
#include <cmath>

int main()
{
    // Input.
    int n, k;
    std::cin >> n >> k;

    std::vector<int> heights(n);
    for(auto i = 0 ; i < n ; i++)
        std::cin >> heights[i];

    // Solution.

    std::vector<int> minCost(n, INT_MAX);
    minCost[0] = 0;
    for(auto i = 1 ; i < n ; i++)
    {
        for(auto j = i - k >= 0 ? i - k : 0 ; j < i ; j++)
            minCost[i] = std::min(minCost[i], minCost[j] + abs(heights[i] - heights[j]));
    }

    std::cout << minCost.back();

    return 0;
}
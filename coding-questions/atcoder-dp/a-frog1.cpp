/*
    Problem link - https://atcoder.jp/contests/dp/tasks/dp_a

    Solution Idea:
    1. Frog can only reach stone[i] from stone[i-1] or stone[i-2], so reach those first then calculate min for stone[i]
    => minCost[i] = min(minCost[i-1] + abs(height[i] - height[i-1]), minCost[i-2] + abs(height[i] - height[i-2]))
    Time: O(n)
    Space: O(1)
*/
#include <iostream>
#include <vector>
#include <cmath>

int main()
{
    // Input.
    int n;
    std::cin >> n;
    
    std::vector<int> heights(n);
    for(size_t i = 0 ; i < n ; i++)
        std::cin >> heights[i];

    // Solution.
    std::vector<int> minCost(3);
    minCost[0] = 0;
    minCost[1] = abs(heights[1] - heights[0]);
    for(size_t i = 2 ; i < n ; i++)
    {
        minCost[2] = std::min(minCost[0] + abs(heights[i-2] - heights[i]), minCost[1] + abs(heights[i-1] - heights[i]));
        minCost[0] = minCost[1];
        minCost[1] = minCost[2];
    }

    std::cout << minCost[1];

    return 0;
}

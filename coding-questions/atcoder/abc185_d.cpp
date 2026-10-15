/*
    Problem link: 
*/

#include <algorithm>
#include <climits>
#include <iostream>
#include <queue>
#include <vector>

int main()
{
    int n, m, temp;
    std::cin >> n >> m;

    std::vector<int> nums(m);
    for(int i = 0 ; i < m ; i++)
        std::cin >> nums[i];

    if(n == m)
    {
        std::cout << 0 << "\n";
        return 0;
    }
    else if(m == 0)
    {
        std::cout << 1 << "\n";
        return 0;
    }

    std::sort(nums.begin(), nums.end());
    std::vector<int> gaps;

    if(nums[0] != 1)
        gaps.push_back(nums[0] - 1);

    for(int i = 1 ; i < m ; i++)
    {
        if(nums[i] - nums[i-1] - 1 > 0)
            gaps.push_back(nums[i] - nums[i-1] - 1);
    }

    if(nums[m-1] != n)
        gaps.push_back(n - nums[m-1]);

    int minGap = INT_MAX;
    for(int g : gaps)
        minGap = std::min(minGap, g);

    int stampCnt = 0;
    for(int g : gaps)
        stampCnt += (g + minGap - 1) / minGap;

    std::cout << stampCnt << "\n";

    return 0;
}
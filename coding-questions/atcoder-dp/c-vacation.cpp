/*
    Problem link: https://atcoder.jp/contests/dp/tasks/dp_c

    Solution idea:
    1. If we choose option-a at day-i then we can pick the maximum cost accumulated till
    day-(i-1) and we have not chosen option-a on the previous day. Same for option-b, c.
*/

#include <iostream>
#include <vector>
#include <cmath>

int main()
{
    // Input.
    int n;
    std::cin >> n;

    std::vector<int> nums(3);
    std::vector< std::vector<int> > maxChoice(2, std::vector<int>(3));

    // Solution.
    for(auto j = 1 ; j <= n ; j++)
    {
        for(auto i = 0 ; i < 3 ; i++)
        {
            std::cin >> nums[i];
            if(j == 1)
                maxChoice[0][i] = nums[i];
        }

        if(j == 1)
            continue;

        maxChoice[1][0] = nums[0] + std::max(maxChoice[0][1], maxChoice[0][2]);
        maxChoice[1][1] = nums[1] + std::max(maxChoice[0][0], maxChoice[0][2]);
        maxChoice[1][2] = nums[2] + std::max(maxChoice[0][0], maxChoice[0][1]);

        for(auto i = 0 ; i < 3 ; i++)
            maxChoice[0][i] = maxChoice[1][i];
    }

    std::cout << std::max(maxChoice[0][0], std::max(maxChoice[0][1], maxChoice[0][2]));

    return 0;
}

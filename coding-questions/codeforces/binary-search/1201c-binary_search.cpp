/*
 * Problem link: https://codeforces.com/contest/1201/problem/C
 * Solution link:
 *
 * Time:
 * Space:
 */

#include <algorithm>
#include <iostream>
#include <ranges>
#include <vector>

int main()
{
    int n, k;
    std::cin >> n >> k;

    std::vector<int> nums(n);
    for(int i = 0 ; i < n ; i++)
        std::cin >> nums[i];

    std::sort(nums.begin(), nums.end());
    int mid = n / 2; // n guaranteed to be odd.

    auto pred = [&](int num)
    {
        int opsLeft = k;
        int cond = true;
        for(int i = mid ; i < n ; i++)
        {
            if(nums[i] < num)
                opsLeft -= num - nums[i];
            if(opsLeft < 0)
                cond = false;
        }

        return cond;
    };

    auto range = std::views::iota(nums[mid], nums[mid] + k + 1);
    auto it = std::ranges::partition_point(range.begin(), range.end(), pred);
    std::cout << static_cast<int>(*it - 1) << std::endl;

    return 0;
}

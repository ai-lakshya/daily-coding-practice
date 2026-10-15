/*
 * problem link: https://atcoder.jp/contests/abc478/tasks/abc478_c
 * solution idea: 
 * 
 * time:
 * space: 
*/

#include <algorithm>
#include <iostream>
#include <vector>

int main()
{
    int n, k;
    std::cin >> n >> k;

    std::vector<int> nums(n), numsAlt(n);
    for(int i = 0 ; i < n ; i++)
    {
        std::cin >> nums[i];
        numsAlt[i] = nums[i];
    }

    std::sort(numsAlt.begin(), numsAlt.end());
    int leftMost = n, rightMost = -1;
    for(int i = 0 ; i < n ; i++)
    {
        if(nums[i] != numsAlt[i])
        {
            leftMost = std::min(leftMost, i);
            rightMost = std::max(rightMost, i);
        }
    }

    if(rightMost - leftMost + 1 > k)
        std::cout << "No\n";
    else
        std::cout << "Yes\n";

    return 0;
}

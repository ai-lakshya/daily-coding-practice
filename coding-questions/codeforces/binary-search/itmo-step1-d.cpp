/*
    Problem link: https://codeforces.com/edu/course/2/lesson/6/1/practice/contest/283911/problem/D
    Solution idea:

    Time: 
    Space: 
*/

#include <algorithm>
#include <iostream>
#include <vector>

int main()
{
    int n;
    std::cin >> n;

    std::vector<int> nums(n);
    for(int i = 0 ; i < n ; i++)
        std::cin >> nums[i];

    std::sort(nums.begin(), nums.end());

    int k;
    std::cin >> k;
    for(int i = 0 ; i < k ; i++)
    {
        int l, r;
        std::cin >> l >> r;

        auto left = std::partition_point(nums.begin(), nums.end(), [&](int num){
            return num < l;
        });

        auto right = std::partition_point(nums.begin(), nums.end(), [&](int num){
            return num <= r;
        });

        std::cout << right - left << " ";
    }


    return 0;
}

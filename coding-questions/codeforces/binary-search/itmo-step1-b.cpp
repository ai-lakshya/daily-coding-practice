/*
    Problem link: https://codeforces.com/edu/course/2/lesson/6/1/practice/contest/283911/problem/B
    Solution idea:

    Time: 
    Space: 
*/

#include <algorithm>
#include <iostream>
#include <vector>

int main()
{
    int n, k;
    std::cin >> n >> k;

    std::vector<int> nums(n);
    for(int i = 0 ; i < n ; i++)
        std::cin >> nums[i];

    for(int i = 0 ; i < k ; i++)
    {
        int q;
        std::cin >> q;

        // returns iterator to the first element where predicate is false.
        // here it will be the first num > q.
        auto it = std::partition_point(nums.begin(), nums.end(), [&](int num){
            return num <= q;
        });

        if(it == nums.begin())
        {
            std::cout << 0 << "\n";
        }
        else
        {
            std::cout << (it - nums.begin()) << "\n";
        }
    }


    return 0;
}

/*
    Problem link: https://cses.fi/problemset/task/1640
    Solution idea: 
    Sort the numbers and then apply 2 pointers moving in opposite directions.

    Time: O(nlogn) for sorting and O(n) for 2-pointer
    Space: O(1) extra space
*/

#include <algorithm>
#include <cmath>
#include <iostream>
#include <vector>

using ll = long long;

int main()
{
    ll n, x;
    std::cin >> n >> x;
    std::vector< std::vector<ll> > nums(n, std::vector<ll>(2));
    for(ll i = 0 ; i < n ; i++)
    {
        std::cin >> nums[i][0];
        nums[i][1] = i + 1;
    }

    std::sort(nums.begin(), nums.end());

    bool ansExist = false;
    ll left = 0;
    ll right = n-1;
    while(left < right)
    {
        auto sum = nums[left][0] + nums[right][0];
        if(sum == x)
        {
            ansExist = true;
            break;
        }
        else if(sum < x)
        {
            left++;
        }
        else
        {
            right--;
        }
    }

    if(!ansExist)
        std::cout << "IMPOSSIBLE" << std::endl;
    else
        std::cout << std::min(nums[left][1], nums[right][1]) << " " << std::max(nums[left][1], nums[right][1]) << std::endl;

    return 0;
}

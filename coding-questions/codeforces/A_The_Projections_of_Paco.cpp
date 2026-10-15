#include <iostream>
#include <vector>

using ll = long long;

int main()
{
    long long t;
    std::cin >> t;
    while(t--)
    {
        ll n;
        std::cin >> n;

        std::vector<ll> nums(n);
        for(ll i = 0 ; i < n ; i++)
            std::cin >> nums[i];

        ll maxFound = nums[0];

        // first test to make maximum negative.
        ll sumTillNow = nums[0];
        for(ll i = 1 ; i < n ; i++)
        {
            ll locVal = std::max(nums[i], std::max(nums[i] - sumTillNow, nums[i] + sumTillNow));
            maxFound = std::max(maxFound, locVal);
            // make current number as negative as possible.
            nums[i] = std::min(nums[i], std::min(nums[i] - sumTillNow, nums[i] + sumTillNow));
        }

        // next test to make maximum positive.


    }



    return 0;
}
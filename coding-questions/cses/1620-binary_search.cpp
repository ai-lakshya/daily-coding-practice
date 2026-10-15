/*
 * Problem link:
 * Solution idea:
 *
 * Time:
 * Space:
 */

#include <algorithm>
#include <climits>
#include <cmath>
#include <iostream>
#include <ranges>
#include <vector>

using ll = long long;

int main()
{
    ll n, t;
    std::cin >> n >> t;

    ll maxTime = LLONG_MIN;
    std::vector<ll> k(n);
    for(ll i = 0 ; i < n ; i++)
    {
        std::cin >> k[i];
        maxTime = std::max(maxTime, k[i]);
    }

    auto pred = [&](ll time)
    {
        ll currMade = 0;
        for(ll i = 0 ; i < n ; i++)
        {
            currMade += time / k[i];
            if(currMade >= t)
                return false;
        }
        return true;
    };

    ll lowerLimit = 1;
    ll upperLimit = t * maxTime + 1;
    auto range = std::views::iota(lowerLimit, upperLimit);
    auto it = std::ranges::partition_point(range.begin(), range.end(), pred);
    std::cout << *it << std::endl;

    return 0;
}

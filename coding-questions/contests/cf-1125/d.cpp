#include <algorithm>
#include <climits>
#include <iostream>
#include <ranges>
#include <vector>

using ll = long long;

struct Lab
{
    ll a;
    ll b;
    ll c;
};

// calculate the number of operations required to reach >= threshold.
ll resolveLab(Lab lab, ll th)
{
    
}

int main()
{
    ll t;
    std::cin >> t;
    while(t--)
    {
        ll n, k;
        std::cin >> n >> k;

        std::vector<Lab> labs(n);
        for(ll i = 0 ; i < n ; i++)
            std::cin >> labs[i].a >> labs[i].b >> labs[i].c;

        // predicate should return true if 'num' can be achieved as the max(min) value.
        auto pred = [&](ll num)
        {
            ll ops = k;
            bool reachable = true;
            // each lab should be able to reach num within the max number of operations allowed .
            for(ll i = 0 ; i < n ; i++)
            {
                ll opsUsed = resolveLab(labs[i], num);

                if(ops - opsUsed < 0)
                {
                    reachable = false;
                    break;
                }
                else
                {
                    ops -= opsUsed;
                }
            }

            return reachable;
        };

        ll low = LLONG_MIN + 1;
        ll high = LLONG_MAX;
        auto range = std::views::iota(low, high);
        auto it = std::ranges::partition_point(range.begin(), range.end(), pred);
        
        std::cout << *it - 1 << "\n";
    }

    return 0;
}
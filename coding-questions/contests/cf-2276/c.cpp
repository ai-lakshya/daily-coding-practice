#include <algorithm>
#include <iostream>
#include <ranges>
#include <vector>

using ll = long long;

int main()
{
    ll t;
    std::cin >> t;
    while(t--)
    {
        ll n, k;
        std::cin >> n >> k;

        std::vector<ll> a(n);
        std::vector<ll> pref(n);
        for(ll i = 0 ; i < n ; i++)
        {
            std::cin >> a[i];
            if(i == 0)
                pref[i] = a[i];
            else
                pref[i] = pref[i-1] + a[i];
        }

        ll totalCnt = 0;

        for(ll i = k ; i < n ; i++)
        {
            ll sumHere = i == k ? pref[i] : pref[i] - pref[i-k-1];
            for(ll j = sumHere - (sumHere % k) + k ; ; j+=k)
            {
                auto it = std::ranges::partition_point(pref.begin() + i, pref.end(), [&](ll num){
                    return num < j;
                });
                
                if(it == pref.end())
                    break;

                
            }
        }

        std::cout << totalCnt << "\n";

    }



    return 0;
}
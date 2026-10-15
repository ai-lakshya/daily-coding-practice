#include <iostream>
#include <map>
#include <vector>

using ll = long long;

int main()
{
    ll t;
    std::cin >> t;
    while(t--)
    {
        ll n;
        std::cin >> n;

        std::vector<ll> a(n);
        for(ll i = 0 ; i < n ; i++)
            std::cin >> a[i];

        std::map<ll, std::vector<ll>> tp0;
        std::map<ll, std::vector<ll>> tp1;
        for(ll i = 0 ; i <= n - 1 - 4 ; i += 2)
        {
            ll tsum = a[i] + a[i+2] - a[i+4];
            tp0[tsum].push_back(i);
        }
        for(ll i = 1 ; i <= n - 1 - 4 ; i += 2)
        {
            ll tsum = a[i] + a[i+2] - a[i+4];
            tp1[tsum].push_back(i);
        }

        ll tcount = 0;

        // type-1: same sum in different tp
        for(auto [tsum, tv] : tp0)
        {
            if(tp1.find(tsum) == tp1.end())
                continue;
            tcount += tp0[tsum].size() * tp1[tsum].size();
        }

        // type-2: tp0 only
        for(auto [tsum, tv] : tp0)
        {
            for(ll i = 0 ; i < tv.size() ; i++)
            {
                ll j = i + 1;
                while(j < tv.size() && tv[j] <= tv[i] + 4)
                    j++;
                if(j < tv.size())
                    tcount += tv.size() - j;
            }
        }

        // type-3: tp1 only
        for(auto [tsum, tv] : tp1)
        {
            for(ll i = 0 ; i < tv.size() ; i++)
            {
                ll j = i + 1;
                while(j < tv.size() && tv[j] <= tv[i] + 4)
                    j++;
                if(j < tv.size())
                    tcount += tv.size() - j;
            }
        }

        std::cout << tcount << "\n";
    }


    return 0;
}
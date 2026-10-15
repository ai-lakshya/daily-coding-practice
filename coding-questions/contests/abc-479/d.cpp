#include <algorithm>
#include <climits>
#include <iostream>
#include <vector>

using ll = long long;

struct minAvail
{
    ll leftWithMinRight;
    ll rightWithMinLeft;
};

int main()
{
    ll n, q;
    std::cin >> n >> q;
    
    std::vector<ll> L(n+2, 0);
    std::vector<ll> R(n+2, 0);
    for(ll i = 2 ; i <= n ; i++)
        std::cin >> L[i];
    for(ll i = 1 ; i <= n-1 ; i++)
        std::cin >> R[i];

    std::vector<minAvail> pref(n + 2);
    pref[0].leftWithMinRight = INT_MAX;
    pref[n+1].rightWithMinLeft = INT_MAX;
    for(ll i = 1 ; i <= n ; i++)
        pref[i].leftWithMinRight = std::min(pref[i-1].leftWithMinRight, R[i]);
    for(ll i = n ; i >= 1 ; i--)
        pref[i].rightWithMinLeft = std::min(pref[i+1].rightWithMinLeft, L[i]);

    while(q--)
    {
        ll s, t;
        std::cin >> s >> t;

        ll ans = LLONG_MAX;

        if(s < t)
        {
            ans = std::min(R[s], pref[s-1].leftWithMinRight + L[s]);
        }
        else
        {
            ans = std::min(L[s], pref[s+1].rightWithMinLeft + R[s]);
        }

        std::cout << ans << "\n";
    }

    return 0;
}
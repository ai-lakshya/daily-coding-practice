#include <iostream>

using ll = long long;

int main()
{
    int t;
    std::cin >> t;
    while(t--)
    {
        ll x, y, k, cnt = 0;
        std::cin >> x >> y >> k;
        
        for(ll i = 0 ; i < k ; i++)
        {
            x += i;
            y += i;

            cnt += y % x;
        }

        std::cout << cnt << "\n";
    }


    return 0;
}
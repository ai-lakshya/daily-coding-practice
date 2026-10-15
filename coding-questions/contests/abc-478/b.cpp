/*
 * problem link: https://atcoder.jp/contests/abc478/tasks/abc478_b
 * solution idea:
 *
 * time:
 * space:
 */

#include <iostream>
#include <utility>
#include <vector>

using ll = long long;

int main()
{
    ll n, v;
    std::cin >> n >> v;

    std::vector< std::pair<ll, ll> > w(n);
    for(ll i = 0 ; i < n ; i ++)
    {
        std::cin >> w[i].first;
        w[i].second = i + 1;
    }

    ll maxFound = 0;
    ll currCost, currFound;
    for(ll i = 0 ; i < n ; i++)
    {
        for(ll j = 0 ; j < n ; j++)
        {
            if(j == i)
                continue;
            for(ll k = 0 ; k < n ; k++)
            {
                if(k == i || k == j)
                    continue;

                currCost = w[i].second + w[j].second + w[k].second;
                currFound = w[i].first + w[j].first + w[k].first;
                if(currCost <= v)
                    maxFound = std::max(maxFound, currFound);
            }
        }
    }

    std::cout << maxFound << std::endl;

    return 0;
}

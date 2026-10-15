#include <iostream>
#include <utility>
#include <vector>

using ll = long long;

ll rec(std::vector<std::pair<ll,ll>>& dish, ll ind, ll lim)
{
    if(ind >= dish.size())
        return 0;

    ll take = 0;
    ll notTake = rec(dish, ind+1, lim);
    if(dish[ind].second <= lim)
        take = dish[ind].first + rec(dish, ind+1, lim - dish[ind].second);
    return std::max(take, notTake);
}

int main()
{
    ll n, w;
    std::cin >> n >> w;

    std::vector<std::pair<ll,ll>> dish(n);
    for(ll i = 0 ; i < n ; i++)
        std::cin >> dish[i].first >> dish[i].second;

    std::cout << rec(dish, 0, w) << "\n";

    return 0;
}
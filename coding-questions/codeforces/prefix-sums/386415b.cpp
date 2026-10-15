/*
    Problem link: https://codeforces.com/group/isP4JMZTix/contest/386415/problem/B
    Solution idea: 

    Time: 
    Space: 
*/

#include <iostream>
#include <vector>

using ll = long long;

struct Opr
{
    ll left;
    ll right;
    ll val;
};

int main()
{
    ll n, m, k;
    std::cin >> n >> m >> k;

    std::vector<ll> a(n+2, 0);
    for(ll i = 1 ; i <= n ; i++)
        std::cin >> a[i];

    std::vector<Opr> ops(m + 1);
    for(ll i = 1 ; i <= m ; i++)
        std::cin >> ops[i].left >> ops[i].right >> ops[i].val;

    std::vector<ll> freq(m + 2, 0);
    ll x, y;
    for(ll i = 1 ; i <= k ; i++)
    {
        std::cin >> x >> y;
        freq[x]++;
        freq[y+1]--;
    }

    for(ll i = 1 ; i <= m ; i++)
    {
        freq[i] += freq[i-1];
        ops[i].val *= freq[i];
    }

    std::vector<ll> aplus(n+2, 0);
    for(ll i = 1 ; i <= m ; i++)
    {
        aplus[ops[i].left] += ops[i].val;
        aplus[ops[i].right + 1] -= ops[i].val;
    }

    for(ll i = 1 ; i <= n+1 ; i++)
        aplus[i] += aplus[i-1];

    for(ll i = 1 ; i <= n ; i++)
        std::cout << a[i] + aplus[i] << " ";
    std::cout << "\n";


    return 0;
}
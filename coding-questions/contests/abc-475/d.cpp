/*
    Problem link: https://atcoder.jp/contests/abc475/tasks/abc475_d
    Solution idea:
    1. Use segmented sieve to get the primes in the range,
    then use a hash map to check if the required conditions are true on the string.
*/

#include <cmath>
#include <iostream>
#include <string>
#include <unordered_map>
#include <vector>

using ll = long long;

std::vector<char> segmentedSieve(ll L, ll R)
{
    // initially all numbers in the range are assumed prime.
    std::vector<char> primes(R - L + 1, true);

    // 0, 1 can never be primes.
    if(L == 1)
        primes[0] = false;

    // segmented sieve without pre-computing primes.
    ll lim = sqrt(R);
    for(ll i = 2 ; i <= lim ; i++)
    {
        // 1. either i*i itself lies inside [L,R] then for correctness we start from there.
        // 2. otherwise start from the first multiple of i inside the range [L, R].
        ll lowerLimit = std::max(i * i, (L + i - 1) / i * i);
        for(ll j = lowerLimit ; j <= R ; j += i)
            primes[j - L] = false;
    }

    return primes;
}

int main()
{
    std::string S;
    std::cin >> S;

    ll numDigits = S.size();
    ll L = 1;
    for(ll i = 1 ; i < numDigits ; i++)
        L *= 10;
    ll R = L * 10 - 1;

    auto primes = segmentedSieve(L, R);

    for(ll i = 0 ; i < primes.size() ; i++)
    {
        if(!primes[i])
            continue;
        
        ll P = i + L;
        std::unordered_map<char, ll> strMap;
        std::unordered_map<ll, char> digMap;
        int last = S.size() - 1;
        while(P != 0 && last >= 0)
        {
            ll digit = P % 10;
            P /= 10;

            if(!strMap.contains(S[last]) && !digMap.contains(digit))
            {
                strMap[S[last]] = digit;
                digMap[digit] = S[last];
            }
            else if(!(strMap.contains(S[last]) && digMap.contains(digit) && strMap[S[last]] == digit && digMap[digit] == S[last]))
            {
                break;
            }

            last--;
        }

        if(last == -1)
        {
            std::cout << i + L << std::endl;
            return 0;
        }
    }

    std::cout << "-1" << std::endl;
    return 0;
}
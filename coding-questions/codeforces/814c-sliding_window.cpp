/*
    Problem link: https://codeforces.com/problemset/problem/814/C
    Solution idea: 

    Time: 
    Space: 
*/

#include <algorithm>
#include <cmath>
#include <iostream>
#include <ranges>
#include <string>

int main()
{
    std::ios::sync_with_stdio(false);
    std::cin.tie(nullptr);

    int n;
    std::cin >> n;
    std::string s;
    std::cin >> s;

    int q;
    std::cin >> q;
    while(q--)
    {
        int m;
        char c;
        std::cin >> m >> c;

        int maxPatchPossible = m;
        for(const auto cl : s)
            if(cl == c)
                maxPatchPossible++;

        if(maxPatchPossible >= n)
        {
            std::cout << n << '\n';
            continue;
        }

        // run binary search to find the maximum i for which predicate is true.
        auto searchRange = std::views::iota(1, maxPatchPossible + 1);
        auto pred = [&](int i) -> bool
        {
            int windowValidChars = 0;

            // setup initial window.
            for(int j = 0 ; j < i ; j++)
            {
                if(s[j] == c)
                    windowValidChars++;
            }

            if(windowValidChars + m >= i)
                return true;

            // run the sliding window.
            for(int j = i ; j < n ; j++)
            {
                if(s[j] == c)
                    windowValidChars++;

                int windowFront = j - i;
                if(s[windowFront] == c)
                    windowValidChars--;
            
                if(windowValidChars + m >= i)
                    return true;
            }

            return false;
        };

        auto it = std::ranges::partition_point(searchRange, pred) - 1;
        std::cout << *it << '\n';
    }

    return 0;
}

/*
 * Problem link: https://codeforces.com/edu/course/2/lesson/6/2/practice/contest/283932/problem/A
 * Solution idea:
 *
 * Time:
 * Space:
 */

#include <cmath>
#include <iostream>
#include <ranges>
#include <algorithm>

using ll = long long;

int main()
{
    ll w, h, n;
    std::cin >> w >> h >> n;

    ll left = 1;
    ll right = n * std::max(w, h);

    auto pred = [&](ll side)
    {
        ll big_wise = side / std::max(w, h);
        ll small_wise = side / std::min(w, h);

        // guarding against zero and calculating ceil without any complications.
        return small_wise == 0 ? true : big_wise < (n + small_wise - 1) / small_wise;
    };

    auto range = std::views::iota(left, right + 1);
    auto it = std::ranges::partition_point(range.begin(), range.end(), pred);
    auto idx = static_cast<ll>(it - range.begin() + 1);
    std::cout << idx << std::endl;


    return 0;
}

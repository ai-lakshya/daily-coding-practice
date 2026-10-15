/*
 * problem link: https://atcoder.jp/contests/abc478/tasks/abc478_a
 * solution idea: 
 * 
 * time:
 * space: 
 */

#include <iostream>
#include <vector>

int main()
{
    int n, m;
    std::cin >> n >> m;

    int total = m;

    std::vector<int> cnt(n, 0);
    for(int pass = 1 ; pass <= (n + m - 1) / n ; pass++)
    {
        for(int j = 0 ; j < n ; j++)
        {
            if(total > 0)
            {
                cnt[j]++;
                total--;
            }
        }
    }

    for(auto i : cnt)
        std::cout << i << "\n";

    return 0;
}

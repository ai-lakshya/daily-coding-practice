/*
    Problem link: https://atcoder.jp/contests/abc475/tasks/abc475_b
    Solution idea:
    1. Since all transactions are less than 1e6 and we only need the change we can do the following.
*/

#include <cmath>
#include <iostream>
#include <vector>

constexpr int maxValue = 1e6;

int main()
{
    int N;
    std::cin >> N;

    std::vector<int> totalCoins(3, 0);

    for(size_t i = 1 ; i <= N ; i++)
    {
        int val;
        std::cin >> val;

        int change = (maxValue - val) % 1000;
        for(int i = 0 ; i < 3 ; i++)
        {
            totalCoins[i] += change % 10;
            change /= 10;
        }
    }
    
    std::cout << totalCoins[0] << " " << totalCoins[1] << " " << totalCoins[2] << std::endl;

    return 0;
}

/*
    Problem link: https://atcoder.jp/contests/abc477/tasks/abc477_d
*/

#include <iostream>
#include <vector>

int main()
{
    int n, q;
    std::cin >> n >> q;

    // initially all squares are uncovered and their color is 0.
    std::vector<int> parent(n + 1, 101);
    std::vector<int> colors(n + 1, 0);
    std::vector<int> uncover_time(n + 1, 0);
    int lastPaintTime = 0;
    int currColor = 0;
    for(int t = 1 ; t <= q ; t++)
    {
        int i, j;
        char c;

        std::cin >> i;
        if(i == 1)
        {
            // cover/uncover j.
            std::cin >> j;

            if(parent[j] == 101)
            {
                // uncover -> cover
                if(lastPaintTime > uncover_time[j])
                {
                    parent[j] = currColor;
                    colors[j] = currColor;
                }
                else
                {
                    parent[j] = colors[j];
                }
            }
            else
            {
                // cover -> uncover
                parent[j] = 101;
                uncover_time[j] = t;
            }
        }
        else
        {
            // color all uncovered squares with c.
            std::cin >> c;

            currColor = c - 'a';
            lastPaintTime = t;
        }
    }

    for(int i = 1 ; i <= n ; i++)
    {
        if(parent[i] == 101)
        {
            if(lastPaintTime > uncover_time[i])
                std::cout << static_cast<char>('a' + currColor);
            else
                std::cout << static_cast<char>('a' + colors[i]);
        }
        else
        {
            std::cout << static_cast<char>('a' + colors[i]);
        }
    }

    return 0;
}
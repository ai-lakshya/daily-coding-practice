/*
    Problem link: https://codeforces.com/problemset/problem/279/B
    Solution idea:


    Time: 
    Space: 
*/

#include <cmath>
#include <iostream>
#include <queue>

int main()
{
    int n, t;
    std::cin >> n >> t;
    
    std::queue<int> window;
    int windowSum = 0;
    int maxLen = 0;
    
    for(int i = 0 ; i < n ; i++)
    {
        int a;
        std::cin >> a;

        window.push(a);
        windowSum += a;

        while(windowSum > t)
        {
            windowSum -= window.front();
            window.pop();
        }

        // update the maximum window length found till now.
        maxLen = std::max(maxLen, static_cast<int>(window.size()));
    }

    std::cout << maxLen << std::endl;

    return 0;
}
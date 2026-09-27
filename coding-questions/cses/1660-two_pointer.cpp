/*
    Problem link: https://cses.fi/problemset/task/1660
    Solution idea:


    Time: 
    Space: 
*/

#include <iostream>
#include <queue>

int main()
{
    int n, x;
    std::cin >> n >> x;

    std::queue<int> window;
    int windowSum = 0;
    int count = 0;
    
    for(int i = 0 ; i < n ; i++)
    {
        int a;
        std::cin >> a;

        window.push(a);
        windowSum += a;

        while(windowSum > x)
        {
            windowSum -= window.front();
            window.pop();
        }

        if(windowSum == x)
            count++;
    }

    std::cout << count << std::endl;


    return 0;
}
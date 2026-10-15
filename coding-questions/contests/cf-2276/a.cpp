#include <iostream>
#include <utility>
#include <vector>

int main()
{
    int t;
    std::cin >> t;
    while(t--)
    {
        int a, b;
        std::cin >> a >> b;

        if(a <= b)
        {
            std::cout << "-1\n";
            continue;
        }
    
        int cnt = 0;
        while(a > 0 && b > 0 && a > b + 1)
        {
            a -= 2;
            b -= 1;
            cnt++;
        }

        if(b > 0)
            std::cout << cnt + 1 << "\n";
        else
            std::cout << cnt << "\n";
    }


    return 0;
}
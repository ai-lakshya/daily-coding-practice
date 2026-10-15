#include <climits>
#include <iostream>
#include <vector>

int main()
{
    int t;
    std::cin >> t;
    while(t--)
    {
        int a, b;
        std::cin >> a >> b;

        if(b - a > 1)
            std::cout << -1 << "\n";
        else
            std::cout << b + ((a-b)%2 == 0 ? a-b : a-b+1) << "\n";
    }

    return 0;
}
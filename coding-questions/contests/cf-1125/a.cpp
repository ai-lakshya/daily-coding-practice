#include <iostream>

int main()
{
    int t;
    std::cin >> t;

    while(t--)
    {
        int x, y, r;
        std::cin >> x >> y >> r;

        std::cout << x-r << " " << y << "\n";
    }


    return 0;
}
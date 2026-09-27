#include <cmath>
#include <iostream>

using ll = long long;

int main()
{
    ll t;
    std::cin >> t;
    while(t--)
    {
        ll A, B, C;
        std::cin >> A >> B >> C;

        if( std::abs((A+C)-B) >= std::abs(A-B) )
            std::cout << std::abs((A+C)-B) << std::endl;            
        else
            std::cout << std::abs(A-B) << std::endl;
    }

    return 0;
}
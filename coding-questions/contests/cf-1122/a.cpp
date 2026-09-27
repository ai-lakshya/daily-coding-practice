#include <cmath>
#include <iostream>

int main()
{
    int t;
    std::cin >> t;
    while(t--)
    {
        int n, a, ans = 0;
        std::cin >> n;
        for(int i = 0 ; i < 3 ; i++)
        {
            std::cin >> a;
            ans = std::max(ans, n - a);
        }
        
        std::cout << ans << std::endl;
    }

    return 0;
}
#include <iostream>
#include <vector>

int main()
{
    int t;
    std::cin >> t;
    while(t--)
    {
        int n, k, a;
        std::cin >> n >> k;

        std::vector<int> freq(n + 2, 0);
        for(int i = 0 ; i < n ; i++)
        {
            std::cin >> a;
            freq[a]++;
        }

        for(int i = 0 ; i <= n + 1 ; i++)
        {
            if(freq[i] < 2 * k - 1)
            {
                std::cout << "NO\n";
                break;
            }
            else if(freq[i] == 2 * k - 1)
            {
                std::cout << "YES\n";
                break;
            }
        }
    }

    return 0;
}
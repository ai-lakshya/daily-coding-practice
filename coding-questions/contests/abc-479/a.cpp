#include <iostream>

int main()
{
    int n;
    std::cin >> n;

    int cnt = 0;
    int num;
    for(int i = 1 ; i <= n ; i++)
    {
        std::cin >> num;
        if(i % 2 == 0)
            continue;
        cnt += num;
    }

    std::cout << cnt << "\n";

    return 0;
}
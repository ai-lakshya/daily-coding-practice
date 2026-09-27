#include <algorithm>
#include <cmath>
#include <iostream>
#include <vector>

int main()
{
    int n, d;
    std::cin >> n >> d;

    std::vector<std::vector<int>> x(n, std::vector<int>(2, 0));
    for(int i = 0 ; i < n ; i++)
    {
        std::cin >> x[i][0];
        x[i][1] = i;
    }

    std::sort(x.begin(), x.end());

    std::vector<int> ans;
    if( std::abs(x[0][0] - x[1][0]) >= d )
        ans.push_back(x[0][1]);
    if( std::abs(x[n-1][0] - x[n-2][0]) >= d )
        ans.push_back(x[n-1][1]);

    for(int i = 1 ; i <= n-2 ; i++)
    {
        if(std::abs(x[i][0] - x[i-1][0]) >= d && std::abs(x[i][0] - x[i + 1][0]) >= d)
            ans.push_back(x[i][1]);
    }

    std::sort(ans.begin(), ans.end());

    std::cout << ans.size() << "\n";
    for(auto i : ans)
        std::cout << (i + 1) << " ";
    std::cout << "\n";

    return 0;
}
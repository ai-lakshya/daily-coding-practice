/*
    Problem link: 
    Solution idea: 

    Time: 
    Space: 
*/

#include <climits>
#include <iostream>
#include <vector>

int main()
{
    int n, m;
    std::cin >> n >> m;

    std::vector<int> a(n);
    for(int i = 0 ; i < n ; i++)
        std::cin >> a[i];
    std::vector<int> b(m);
    for(int i = 0 ; i < m ; i++)
        std::cin >> b[i];

    int rMaxMin = INT_MIN;
    int rMin = INT_MAX;

    int left = 0;
    int right = 0;

    while(left < n)
    {
        rMin = INT_MAX;
        while(right < m && b[right] <= a[left])
            right++;

        if(right == 0)
            rMin = std::min(rMin, std::abs(a[left] - b[right]));
        else if(right == m)
            rMin = std::min(rMin, std::abs(a[left] - b[right-1]));
        else
            rMin = std::min(rMin, std::min(std::abs(a[left] - b[right-1]), std::abs(a[left] - b[right])));

        rMaxMin = std::max(rMaxMin, rMin);
        left++;
    }

    std::cout << rMaxMin << std::endl;


    return 0;
}
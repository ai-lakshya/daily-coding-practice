/*
    Problem link: https://codeforces.com/group/isP4JMZTix/contest/386415/problem/A
    Solution idea: 

    Time: 
    Space: 
*/

#include <climits>
#include <iostream>
#include <vector>

int main()
{
    int n, k;
    std::cin >> n >> k;

    std::vector<int> h(n+1, 0);
    std::vector<int> pref(n+1, 0);
    for(int i = 1 ; i <= n ; i++)
    {
        std::cin >> h[i];
        pref[i] = pref[i-1] + h[i];
    }

    int minSumFound = INT_MAX;
    int minStartInd = -1;

    for(int i = 1 ; i + k - 1 <= n ; i++)
    {
        int j = i + k - 1;
        if(pref[j] - pref[i-1] < minSumFound)
        {
            minSumFound = pref[j] - pref[i-1];
            minStartInd = i;
        }
    }

    std::cout << minStartInd << "\n";


    return 0;
}
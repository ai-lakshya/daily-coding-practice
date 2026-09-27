/*
    Problem link: https://codeforces.com/problemset/problem/1777/C
    Solution idea: 

    Time: 
    Space: 
*/

#include <algorithm>
#include <climits>
#include <cmath>
#include <iostream>
#include <vector>
#include <map>
#include <queue>
#include <set>

std::set<int> getDivisors(int n)
{
    std::set<int> divisors;
    divisors.insert(1);
    divisors.insert(n);

    for(int i = 2 ; i <= static_cast<int>(sqrt(n)) ; i++)
    {
        if(n % i == 0)
        {
            divisors.insert(i);
            divisors.insert(n / i);
        }
    }

    return divisors;
}

int main()
{
    int t;
    std::cin >> t;
    while(t--)
    {
        int n, m;
        std::cin >> n >> m;
        std::vector<int> nums(n);

        // populate the divisors map along with the input in O(n*sqrt(n)) time.
        std::map<int, std::set<int>> divs;
        for(int i = 0 ; i < n ; i++)
        {
            std::cin >> nums[i];
            divs[nums[i]] = getDivisors(nums[i]);
        }

        std::sort(nums.begin(), nums.end());

        std::queue<int> window;
        int minDiff = INT_MAX;
        int countUniqueDivs = 0;
        std::vector<int> divFreq(m+1, 0);
        for(int i = 0 ; i < n ; i++)
        {
            window.push(nums[i]);
            for(const auto d : divs[nums[i]])
            {
                if(d <= m)
                {
                    if(divFreq[d] == 0)
                        countUniqueDivs++;
                    divFreq[d]++;
                }
            }

            while(countUniqueDivs >= m)
            {
                int windowFront = window.front();
                window.pop();

                if(countUniqueDivs == m)
                    minDiff = std::min(minDiff, std::abs(windowFront - nums[i]));

                for(const auto d : divs[windowFront])
                {
                    if(d <= m)
                    {
                        if(divFreq[d] == 1)
                            countUniqueDivs--;
                        divFreq[d]--;
                    }
                }
            }
        }

        std::cout << (minDiff == INT_MAX ? -1 : minDiff) << std::endl;
    }

    return 0;
}
/*
    Problem link: https://codeforces.com/group/isP4JMZTix/contest/480518/problem/A
    Solution idea: 
*/

#include <iostream>
#include <utility>
#include <vector>

using ll = long long;

struct Query
{
    int t;
    int x;
    int y;
};

int main()
{
    int n, m;
    std::cin >> n >> m;

    std::vector<int> a(n+1);
    for(int i = 1 ; i <= n ; i++)
        std::cin >> a[i];

    std::vector<int> lastChange(n+1, -1);
    std::vector<Query> queries(m);
    std::vector<std::pair<int,int>> type2queries;
    type2queries.push_back({0, 0});
    for(int i = 0 ; i < m ; i++)
    {
        std::cin >> queries[i].t;
        if(queries[i].t == 1)
        {
            std::cin >> queries[i].x >> queries[i].y;
        }
        else if(queries[i].t == 2)
        {
            std::cin >> queries[i].x;
            type2queries.push_back({i+1, queries[i].x});
        }
        else
        {
            std::cin >> queries[i].x;
        }
    }

    std::vector<int> type2pref(type2queries.size() + 1, 0);
    for(int i = 1 ; i <= type2queries.size() ; i++)
        type2pref[i] = type2pref[i-1] + type2queries[i].second;

    

    return 0;
}

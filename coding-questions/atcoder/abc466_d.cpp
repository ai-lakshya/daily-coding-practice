/*
    Problem link: https://atcoder.jp/contests/abc466/tasks/abc466_d
*/

#include <iostream>
#include <set>
#include <vector>

int main()
{
    int n, m;
    std::cin >> n >> m;

    std::set<int> xPres, yPres;
    std::vector<std::pair<int,int>> xyAns;

    std::vector<std::pair<int,int>> queries(m);

    for(int i = 0 ; i < m ; i++)
        std::cin >> queries[i].first >> queries[i].second;

    for(int i = m-1 ; i >= 0 ; i--)
    {
        int r = queries[i].first;
        int c = queries[i].second;

        if(!(xPres.count(r) > 0 || yPres.count(c) > 0))
            xyAns.push_back({r, c});
        xPres.insert(r);
        yPres.insert(c);
    }

    std::cout << xyAns.size() << "\n";

    return 0;
}
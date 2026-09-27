#include <algorithm>
#include<iostream>
#include <string>
#include <vector>

int main()
{
    int q;
    std::string s, t;
    std::cin >> q >> s >> t;

    std::vector<std::vector<int>> pres;
    for(int i = 0 ; i < s.size() ; i++)
    {
        if(s[i] == t[0])
        {
            bool dirty = false;
            for(int j = 0 ; j < t.size() ; j++)
            {
                if(i + j < s.size() && s[i+j] == t[j])
                    continue;
                dirty = true;
                break;
            }
            if(!dirty)
                pres.push_back({i, static_cast<int>(i + t.size() - 1)});
        }
    }

    for(int i = 0 ; i < q ; i++)
    {
        int l, r;
        std::cin >> l >> r;
        l--;
        r--;

        auto left = std::partition_point(pres.begin(), pres.end(), [&](std::vector<int> a){
            return l > a[0];
        });

        auto right = std::partition_point(pres.begin(), pres.end(), [&](std::vector<int> a){
            return r >= a[1];
        });

        auto leftId = left - pres.begin();
        auto rightId = right - pres.begin();
        bool found = false;
        for(int j = leftId ; j < rightId ; j++)
        {
            if(l <= pres[j][0] && r >= pres[j][1])
            {
                std::cout << "Yes\n";
                found = true;
                break;
            }
        }

        if(!found)
            std::cout << "No\n";
    }

    return 0;
}
 
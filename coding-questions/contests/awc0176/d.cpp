#include <algorithm>
#include <iostream>
#include <vector>

int main()
{
    int n, m, k;
    std::cin >> n >> m >> k;

    std::vector<int> s(n);
    for(int i = 0 ; i < n ; i++)
        std::cin >> s[i];

    std::vector<int> r(m);
    for(int i = 0 ; i < m ; i++)
        std::cin >> r[i];

    std::sort(s.begin(), s.end());
    while(k--)
        s.pop_back();

    std::sort(r.begin(), r.end());

    int left = 0;
    int right = 0;
    int cnt = 0;

    while(left < s.size() && right < r.size())
    {
        if(s[left] >= r[right])
        {
            cnt++;
            left++;
            right++;
        }
        else
        {
            left++;
        }
    }

    std::cout << cnt << "\n";

    return 0;
}
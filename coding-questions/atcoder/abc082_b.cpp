/*
    Problem link: https://atcoder.jp/contests/abc082/tasks/abc082_b
*/

#include <algorithm>
#include <functional>
#include <iostream>
#include <string>

int main()
{
    std::string s, t;
    std::cin >> s >> t;

    std::sort(s.begin(), s.end());
    std::sort(t.begin(), t.end(), std::greater<char>());

    int left = 0;
    int right = 0;

    while(left < s.size() && right < t.size() && s[left] == t[right])
    {
        left++;
        right++;
    }

    if(left < s.size() && right < t.size() && s[left] < t[right])
    {
        std::cout << "Yes\n";
    }
    else if(left == s.size() && right < t.size())
    {
        std::cout << "Yes\n";
    }
    else
    {
        std::cout << "No\n";
    }

    return 0;
}
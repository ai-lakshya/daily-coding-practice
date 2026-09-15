/*
    Problem link: https://atcoder.jp/contests/abc475/tasks/abc475_a
    Solution idea: 
    1. To make answer efficient, use string.reserve to allocate the required memory always.
*/

#include <iostream>
#include <string>

int main()
{
    std::string S;
    std::cin >> S;

    std::string ans;
    ans.reserve(S.size() * 2 - 1);

    for(size_t i = 0 ; i < S.size() ; i++)
    {
        ans.push_back(S[i]);
        if(i != S.size() - 1)
            ans.push_back('o');
    }

    std::cout << ans << std::endl;

    return 0;
}
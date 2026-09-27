/*
    Problem link: https://atcoder.jp/contests/abc476/tasks/abc476_a
*/

#include <iostream>
#include <string>

int main()
{
    std::string s;
    std::cin >> s;

    int n = s.size();
    if(s[n-1] == 'e')
        std::cout << s + "r" << std::endl;
    else
        std::cout << s + "er" << std::endl;

    return 0;
};
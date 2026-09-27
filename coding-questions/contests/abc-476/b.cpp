/*
    Problem link: https://atcoder.jp/contests/abc476/tasks/abc476_b
*/

#include <iostream>
#include <string>

int main()
{
    int n;
    std::string s, t;
    std::cin >> n >> s >> t;

    bool ans = true;
    for(int i = 0 ; i < n ; i ++)   
    {
        if(t[i] == '*') 
            continue;

        if(s[i] != t[i])
        {
            ans = false;
            break;
        }
    }

    std::cout << (ans ? "Yes" : "No") << std::endl;

    return 0;
}
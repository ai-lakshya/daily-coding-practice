#include <iostream>
#include <string>

int main()
{
    int n;
    std::cin >> n;

    std::string s;
    std::cin >> s;

    int cnt = 0;
    for(int i = 0 ; i < n - 1 ; i++)
    {
        if(s[i] == 'C' && s[i+1] == 'A')
            cnt++;
    }

    std::cout << cnt << "\n";


    return 0;
}
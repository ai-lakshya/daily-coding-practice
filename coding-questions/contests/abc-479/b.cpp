#include <iostream>
#include <string>

int main()
{
    int n;
    std::cin >> n;

    std::string s;
    std::cin >> s;

    int maxFound = 0;
    int currVal = 0;
    for(int i = 0 ; i < s.size() ; i++)
    {
        if(s[i] == 'R')
        {
            currVal = 0;
            for(int j = i ; j < s.size() ; j++)
            {
                if(s[j] == 'R')
                {
                    currVal++;
                }
                else
                {
                    break;
                }
            }
            maxFound = std::max(maxFound, currVal);
        }
    }

    std::cout << maxFound << "\n";

    return 0;
}
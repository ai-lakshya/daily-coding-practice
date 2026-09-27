/*
    Problem link: https://codeforces.com/problemset/problem/701/C
    Solution idea: 

    Time: 
    Space: 
*/

#include <climits>
#include <iostream>
#include <queue>
#include <set>
#include <string>

template <typename T>
inline unsigned char uchar_cast(T val)
{
    return static_cast<unsigned char>(val);
}

int main()
{
    int n;
    std::cin >> n;

    std::string S;
    std::cin >> S;
    
    std::set<char> uniqueChars;
    for(int i = 0 ; i < n ; i++)
        uniqueChars.insert(S[i]);
    int globalUniqueCount = static_cast<int>(uniqueChars.size());
    
    int minLength = INT_MAX;
    std::queue<char> window;
    // std::multiset<char> windowChars;
    std::vector<int> windowChars(128, 0);
    int countWindowUnique = 0;

    for(int i = 0 ; i < n ; i++)
    {
        // add the new character to the window first.
        window.push(S[i]);
        if(windowChars[uchar_cast(S[i])] == 0)
            countWindowUnique++;
        windowChars[uchar_cast(S[i])]++;

        // resize window if necessary.
        while(countWindowUnique >= globalUniqueCount)
        {
            // check if correct ans is achieved.
            if(countWindowUnique == globalUniqueCount)
                minLength = std::min(minLength, static_cast<int>(window.size()));

            char windowFront = window.front();
            window.pop();

            if(windowChars[uchar_cast(windowFront)] == 1)
                countWindowUnique--;
            windowChars[uchar_cast(windowFront)]--;
        }
    }
    
    std::cout << minLength << std::endl;

    return 0;
}


#include <algorithm>
#include <iostream>
#include <string>
#include <vector>

int main()
{
    std::vector<int> stack;
    std::vector<int> left;

    int t;
    std::cin >> t;
    while(t--)
    {
        int n;
        std::cin >> n;

        std::string s;
        std::cin >> s;

        stack.clear();
        left.clear();

        for(int i = 0 ; i < n ; i++)
        {
            if(s[i] == '1')
            {
                stack.push_back(i+1);
            }
            else if(s[i] == '2')
            {
                if(stack.size() != 0)
                {
                    stack.pop_back();
                    left.push_back(i+1);
                }
            }
        }

        for(auto num : left)
            stack.push_back(num);

        std::sort(stack.begin(), stack.end());

        std::cout << stack.size() << "\n";
        for(auto num : stack)
            std::cout << num << " ";
        std::cout << "\n";
    }

    return 0;
}
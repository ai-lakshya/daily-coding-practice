#include <iostream>
#include <vector>

int main()
{
    int t;
    std::cin >> t;
    while(t--)
    {
        int n;
        std::cin >> n;

        int cntZero = 0;
        std::vector<int> ques(n);
        for(int i = 0 ; i < n ; i++)
        {
            std::cin >> ques[i];
            if(ques[i] == 0)
                cntZero++;    
        }

        if(cntZero < 2)
            std::cout << -1 << "\n";
        else if(ques[0] == 0 && ques[n-1] == 0)
            std::cout << 0 << "\n";
        else if(ques[0] != 0 && ques[n-1] == 0)
            std::cout << 1 << "\n";
        else if(ques[0] == 0 && ques[n-1] != 0)
            std::cout << 1 << "\n";
        else
            std::cout << 2 << "\n";
    }

    return 0;
}
#include <iostream>

int main()
{
    char c;
    std::cin >> c;

    if(c == 'B')
        std::cout << "Y\n";
    else if(c == 'Y')
        std::cout << "R\n";
    else
        std::cout << "B\n";



    return 0;
}
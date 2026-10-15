#include <algorithm>
#include <climits>
#include <iostream>
#include <vector>

int main()
{
    int n;
    std::cin >> n;

    std::vector<int> a(n), b(n);
    for(int i = 0 ; i < n ; i++)
        std::cin >> a[i];
    for(int i = 0 ; i < n ; i++)
        std::cin >> b[i];

    std::sort(a.begin(), a.end());
    std::sort(b.begin(), b.end());

    int cntWin = 0;
    int maxDiff;
    int left, right;
    bool lastWin = false;
    if(a[n-1] > b[n-1])
    {
        cntWin++;
        maxDiff = a[n-1] - b[n-1];
        lastWin = true;
        left = 0;
        right = n-2;
    }
    else
    {
        maxDiff = a[0] - b[n-1];
        left = 1;
        right = n-2;
    }

    while(( left < n - 1 + !lastWin ) && (right < n - 1))
    {
        if(a[left] <=)
    }

    return 0;
}
/*
    Problem link: https://atcoder.jp/contests/abc476/tasks/abc476_c
*/

#include <iostream>
#include <queue>

int main()
{
    int n;
    std::cin >> n;

    std::priority_queue<int> pq;
    for(int i = 1 ; i <= n ; i++)
    {
        int temp;
        std::cin >> temp;
        pq.push(temp);
        
        if(i >= 3)
        {
            std::vector<int> rem;
            for(int j = 1 ; j <= 2 ; j++)
            {
                rem.push_back(pq.top());
                pq.pop();
            }

            std::cout << pq.top() << std::endl;

            for(auto num : rem)
                pq.push(num);
        }
    }

    return 0;
}
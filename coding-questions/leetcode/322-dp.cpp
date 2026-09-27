/*
    Problem idea: https://leetcode.com/problems/coin-change/description/
    Solution idea: 

    Time: 
    Space: 
*/

#include <climits>
#include <vector>

using namespace std;

class Solution 
{
private:

public:
    int rec(vector<int>& coins, int amount, vector<int>& dp)
    {
        if(amount == 0)
            return 0;
        else if(amount < 0)
            return -1;
        
        // check the memo table.
        if(dp[amount] != -2)
            return dp[amount];

        int ans = INT_MAX;
        for(auto d :  coins)
        {
            auto dAns = rec(coins, amount - d, dp);
            if(dAns == -1 || dAns == INT_MAX)
                continue;
            else
                ans = min(ans, 1 + dAns);
        }

        return dp[amount] = ans;
    }

    int coinChange(vector<int>& coins, int amount) 
    {
        vector<int> dp(amount+1, -2);
        auto ans = rec(coins, amount, dp);
        return ans == INT_MAX ? -1 : ans;
    }
};
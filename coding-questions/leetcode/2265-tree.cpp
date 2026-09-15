/*
    Problem link: https://leetcode.com/problems/count-nodes-equal-to-average-of-subtree/description/

    Solution Idea:
    1. Tree DFS: run a dfs from the root. Every node returns the answer for its own subproblem.
*/

#include <cmath>
#include <vector>

// Definition for a binary tree node.
struct TreeNode {
    int val;
    TreeNode *left;
    TreeNode *right;
    TreeNode() : val(0), left(nullptr), right(nullptr) {}
    TreeNode(int x) : val(x), left(nullptr), right(nullptr) {}
    TreeNode(int x, TreeNode *left, TreeNode *right) : val(x), left(left), right(right) {}
};

class Solution {
public:
    // return the {sum, num_nodes, ans_count} for the subtree rooted at root.
    std::vector<int> dfs(TreeNode* root)
    {
        // both children null.
        if(root->left == nullptr && root->right == nullptr)
            return {root->val, 1, 1};
        
        std::vector<int> emptyAns{0, 0, 0};
        auto leftVal = root->left == nullptr ? emptyAns : dfs(root->left);
        auto rightVal = root->right == nullptr ? emptyAns : dfs(root->right);

        auto rootVal = root->val + leftVal[0] + rightVal[0];
        auto rootCount = 1 + leftVal[1] + rightVal[1];

        auto rootIsValidAns = root->val == static_cast<int>(floor(rootVal / rootCount));
        auto rootAns = rootIsValidAns ? 1 + leftVal[2] + rightVal[2] : leftVal[2] + rightVal[2];
        
        return {rootVal, rootCount, rootAns};
    }

    int averageOfSubtree(TreeNode* root) {
        auto treeVal = dfs(root);
        return treeVal[2];
    }
};
/*
    Problem link: https://atcoder.jp/contests/abc475/tasks/abc475_e
    Solution idea: 

*/

#include <iostream>
#include <string>
#include <vector>

struct TreeNode
{
    TreeNode* left = nullptr;               // 0
    TreeNode* right = nullptr;              // 1
    std::vector<std::string> val;      // strings being stored.
};

void insertLeaf(TreeNode* root, std::string v)
{
    auto node = root;
    for(int i = 1 ; i <= v.size() ; i++)
    {
        auto vsub = v.substr(0, i);
        if(node == nullptr)
        {
            node = new TreeNode;
            node->val.push_back("");
        }

        if(v[i-1] == '1')
        {
            // insert into right sub tree
            if(node->right == nullptr)
                node->right = new TreeNode;
            node = node->right;
        }
        else
        {
            // insert into left sub tree
            if(node->left == nullptr)
                node->left = new TreeNode;
            node = node->left;
        }

        node->val.push_back(vsub);
    }
}

TreeNode* initializeTree(std::vector<std::string>& c)
{
    TreeNode* root = nullptr;
    for(auto str : c)
        insertLeaf(root, str);
    return root;
}

int main()
{
    int n, m, k;
    std::cin >> n >> m >> k;

    std::string t;
    std::cin >> t;

    std::vector<std::string> s(n);
    for(int i = 0 ; i < n ; i++)
        std::cin >> s[i];

    // Create the correctness strings. The tree is created using these strings.
    std::vector<std::string> c(n);
    for(int i = 0 ; i < n ; i++)
    {
        std::string cLocal = "";
        for(int j = 0 ; j < s[i].size() ; j++)
        {
            if(s[i][j] == t[j])
                cLocal += '1';
            else
                cLocal += '0';
        }
        c[i] = cLocal;
    }

    // Initialise the tree


    return 0;
}


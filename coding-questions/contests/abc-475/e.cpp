/*
    Problem link: https://atcoder.jp/contests/abc475/tasks/abc475_e
    Solution idea: 

*/

#ifndef DSA_TRIE_H
#define DSA_TRIE_H

#include <iostream>
#include <map>
#include <queue>
#include <set>
#include <string>

namespace dsa
{

///////////////////////////////////////////////////////////////////////////////
// Represents an individual node for a prefix tree (trie).
class TrieNode
{
    // Delete copy constructor and copy assignment operators.
    TrieNode(TrieNode&) = delete;
    TrieNode operator=(TrieNode& other) = delete;

public:
    // Contructor.
    TrieNode() = default;

    // Get the edge link map from this node.
    std::map<char, TrieNode*>& getEdgeLinksModifiable()
    {
        return mEdges;
    }

    // Set the prefix represented by this node.
    void setNodePrefix(const std::string& pref)
    {
        mCurrPrefix = pref;
    }

    // Toggle the flag to tell if this node terminates as a word.
    void toggleComplete()
    {
        mComplete = !mComplete;
    }

    // Increment count of words with the current prefix.
    void incrementCount()
    {
        mWordCount++;
    }

    // Check if a word terminates at this node.
    bool getCompleteStatus() const
    {
        return mComplete;
    }

    // Get the count of words which have the current prefix in the trie.
    int getWordCount() const
    {
        return mWordCount;
    }

    // Get the prefix stored at this node.
    std::string getNodePrefix() const
    {
        return mCurrPrefix;
    }

private:
    // Store the edge links outgoing from this node. 
    // Links are stored based on the ASCII code of the character.
    std::map<char, TrieNode*> mEdges;
    // Flag to tell if this node terminates as a word.
    bool mComplete{false};
    // Store the prefix represented by this node.
    std::string mCurrPrefix{""};
    // Count the number of words in the trie which have this prefix.
    int mWordCount{0};
};

///////////////////////////////////////////////////////////////////////////////
// Represents a prefix tree (trie).
class Trie
{
public:
    // Constructor.
    Trie()
        : mRoot(new TrieNode)
    {
    }

    // Add a key to the trie.
    void insert(const std::string& key)
    {
        // If the key already exists no need to create it.
        if(contains(key))
            return;

        // This ensure pre process happens before any kind of retrieval.
        mDirty = true;

        auto currNode = mRoot;
        for(size_t i = 0 ; i < key.size() ; i++)
        {
            auto& edgeLinkMap = currNode->getEdgeLinksModifiable();
            if(edgeLinkMap.find(key[i]) == edgeLinkMap.end())
                edgeLinkMap[key[i]] = new TrieNode;
            
            // Move to the newly created node.
            currNode = edgeLinkMap[key[i]];
            currNode->incrementCount();
            
            // Also insert the character in the alphabet set of the trie.
            mAlphabet.insert(key[i]);
            currNode->setNodePrefix(key.substr(0, i + 1));

            // Word terminates here.
            if(i == static_cast<int>(key.size())-1)
                currNode->toggleComplete();
        }
    }

    // Find a key in the trie.
    bool contains(const std::string& key, bool asPrefix = false) const
    {
        auto currNode = mRoot;
        for(size_t i = 0 ; i < key.size() ; i++)
        {
            auto& edgeLinkMap = currNode->getEdgeLinksModifiable();
            if(edgeLinkMap.find(key[i]) == edgeLinkMap.end())
                return false;
            currNode = edgeLinkMap[key[i]];
        }

        if(!asPrefix && !currNode->getCompleteStatus())
            return false;
        return true;
    }

    // does the subtree rooted at given node contains a given key.
    bool contains(TrieNode* root, const std::string& key) const
    {
        const auto& nodeStr = root->getNodePrefix();
        int keyInd = 0;
        for(size_t i = 0 ; i < key.size() ; i++)
        {
            if(nodeStr[i] != key[i])
                return false;
            keyInd++;
        }

        auto keySubstr = key.substr(keyInd);
        std::cout << keySubstr << std::endl;

        auto currNode = root;
        for(size_t i = 0 ; i < key.substr(keyInd).size() ; i++)
        {
            auto edgeLinks = currNode->getEdgeLinksModifiable();
            if(edgeLinks.find(key[i]) == edgeLinks.end())
                return false;
            currNode = edgeLinks[key[i]];
        }

        if(currNode->getCompleteStatus())
            return true;
        return false;
    }

    // Find the node ptr corresponding to given prefix
    TrieNode* getPrefNodePtr(const std::string& pref)
    {
        auto currNode = mRoot;
        for(size_t i = 0 ; i < pref.size() ; i++)
        {
            auto edgeLinks = currNode->getEdgeLinksModifiable();
            if(edgeLinks.find(pref[i]) == edgeLinks.end())
                return nullptr;
            currNode = edgeLinks[pref[i]];
        }
        return currNode;
    }

    // Get the number of words which match a prefix.
    int wordCount(const std::string& key)
    {
        auto currNode = mRoot;
        for(size_t i = 0 ; i < key.size() ; i++)
        {
            auto edgeLinkMap = currNode->getEdgeLinksModifiable();
            if(edgeLinkMap.find(key[i]) == edgeLinkMap.end())
                return false;
            currNode = edgeLinkMap[key[i]];
        }

        return currNode->getWordCount();
    }

    // Get a list of all the strings rooted at a given node.
    std::set<std::string> getAllWords()
    {
        if(!mDirty)
            return mAllWords;

        std::set<std::string> allWords;
        std::queue<TrieNode*> processing;
        for(auto [c, ptr] : mRoot->getEdgeLinksModifiable())
            processing.push(ptr);

        while(!processing.empty())
        {
            auto prefNode = processing.front();
            processing.pop();

            if(prefNode->getCompleteStatus())
                allWords.insert(prefNode->getNodePrefix());

            for(auto [c, ptr] : prefNode->getEdgeLinksModifiable())
                processing.push(ptr);
        }

        mDirty = false;
        return mAllWords = allWords;
    }

    // Check if a participant has qualified.
    bool qualified(const std::string& correct, const std::string& ans, int k, int m)
    {
        std::string checkStr = "";
        int alreadyQualified = 0;
        for(int i = 0 ; i < k ; i++)
        {
            // start checking in the "***1" subtree.
            checkStr += '1';
            int countPossible = wordCount(checkStr);
            if(alreadyQualified + countPossible > m)
            {
                // cannot qualify the current candidates
                checkStr.back() = '0';
                continue;
            }

            // check if candidate is present in the current sub tree.
            auto currCheckPrefptr = getPrefNodePtr(checkStr);
            if(contains(currCheckPrefptr, ans))
                return true;
            alreadyQualified += countPossible;
            checkStr.back() = '0';
        }

        return false;
    }

    // Get the alphabet set of the trie.
    std::set<char> getAlphabetSet() const
    {
        return mAlphabet;
    }

private:
    // Store the root of the prefix tree.
    TrieNode* mRoot{nullptr};
    // Store the alphabet set of the words in the trie.
    std::set<char> mAlphabet;
    // Store all the words stored in the trie.
    std::set<std::string> mAllWords;
    // Tree dirty.
    bool mDirty{false};
};

} // namespace dsa

#endif

///////////////////////////////////////////////////////////////////////////////
// Driver code

#include <iostream>
#include <string>
#include <vector>

int main()
{
    int n, m, k;
    std::cin >> n >> m >> k;

    std::string t;
    std::cin >> t;

    std::vector<std::string> correctnessStr;
    correctnessStr.reserve(n);
    for(int i = 0 ; i < n ; i++)
    {
        std::string s;
        std::cin >> s;

        for(size_t j = 0 ; j < s.size() ; j++)
        {
            if(s[j] == t[j])
                s[j] = '1';
            else
                s[j] = '0';
        }

        correctnessStr.push_back(s);
    }

    auto trie = new dsa::Trie;
    for(const auto& s : correctnessStr)
        trie->insert(s);

    std::cout << "[TRIE] : trie contains the following words:" << std::endl;
    for(const auto& key : trie->getAllWords())
        std::cout << key << std::endl;

    // Now start processing the queries.
    std::cout << "[TRIE] candidate (2 - " << correctnessStr[1] << ") qualification status: " << trie->qualified(t, correctnessStr[1], k, m) << std::endl;

    return 0;
}
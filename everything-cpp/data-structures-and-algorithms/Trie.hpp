#ifndef DSA_TRIE_H
#define DSA_TRIE_H

// #include <cstddef>
// #include <functional>
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
#include <iostream>
#include <vector>

int main()
{
    std::ios::sync_with_stdio(false);
    std::cin.tie(nullptr);

    int n, temp;
    std::cin >> n;

    std::vector<int> a(n);
    for(int i = 0 ; i < n ; i++)
        std::cin >> a[i];

    std::vector<int> freq(n + 1, 0);
    std::vector<int> st;
    st.reserve(n);
    for(int i = 0 ; i < n ; i++)
    {
        std::cin >> temp;
        freq[temp]++;
        
        // try to deliver the top first.
        if(st.size() > 0 && freq[st.back()] > 0)
        {
            freq[st.back()]--;
            st.pop_back();
        }
        st.push_back(a[i]);
    }

    while(st.size() > 0 && freq[st.back()] > 0)
    {
        freq[st.back()]--;
        st.pop_back();
    }

    if(st.size() > 0)
        std::cout << "No\n";
    else
        std::cout << "Yes\n";


    return 0;
}
#include <fstream>
#include <functional>
#include <iostream>
#include <list>
#include <unordered_map>
#include <unordered_set>
#include <vector>

using Dag = std::unordered_map<char, std::unordered_set<char>>;

std::vector<std::string> GetWords();
void ProcessWordPair(const std::string& s1, const std::string s2, Dag* dag);
std::list<char> TopoSort(const Dag& dag);

// Expects one word per line in the given file.
// Probably won't like apostrophes, hyphens, etc.
std::vector<std::string> GetWords()
{
    std::vector<std::string> words;
    // std::string infilename("./dict/words");
    std::string infilename("./dict/british_dictionary_clean");
    std::ifstream infile(infilename);
    std::string word;

    while (getline(infile, word))
    {
        words.push_back(word);
    }

    // RVO FTW
    return words;
}

// Process pairs of words, recursively looking at letters.  Build DAG.
void ProcessWordPair(const std::string& s1, const std::string s2, Dag* dag)
{
    if (s1.empty() || s2.empty())
    {
        return;
    }

    // Find the first letters in the given strings that are different.
    char c1 = s1[0];
    char c2 = s2[0];
    if (c1 != c2)
    {
        // First make sure both letters are nodes in the dag.
        if (!dag->count(c1))
        {
            dag->insert(std::make_pair(c2, std::unordered_set<char>()));
        }
        if (!dag->count(c2))
        {
            dag->insert(std::make_pair(c2, std::unordered_set<char>()));
        }

        // Then insert c2 as a descendent of c1.
        (*dag)[c1].insert(c2);

        return;
    }
    else
    {
        // If index==size, substr returns empty string.
        ProcessWordPair(s1.substr(1), s2.substr(1), dag);
    }

    return;
}

std::list<char> TopoSort(const Dag& dag)
{
    // FROM WIKIPEDIA
    // L ← Empty list that will contain the sorted nodes
    // while there are unmarked nodes do
    //    select an unmarked node n
    //    visit(n)
    // function visit(node n)
    //    if n has a temporary mark then stop (not a DAG)
    //    if n is not marked (i.e. has not been visited yet) then
    //        mark n temporarily
    //        for each node m with an edge from n to m do
    //            visit(m)
    //        mark n permanently
    //        unmark n temporarily
    //        add n to head of L

    std::list<char> sorted_list;
    std::unordered_set<char> temp_marked_nodes;
    std::unordered_set<char> perm_marked_nodes;
    std::function<void(char)> visit = [&](char n)
    {
        if (temp_marked_nodes.count(n))
        {
            std::cout << "NOT A DAG" << std::endl;
            exit(1);
        }
        if (perm_marked_nodes.count(n))
        {
            return;
        }

        temp_marked_nodes.insert(n);

        // Iterate through the descendents of n in the graph.
        for (auto it = dag.at(n).begin(); it != dag.at(n).end(); ++it)
        {
            visit(*it);
        }
        perm_marked_nodes.insert(n);
        temp_marked_nodes.erase(n);
        sorted_list.push_front(n);
    };

    // Iterate through nodes in the graph until all are permanently marked.
    for (auto it = dag.begin(); it != dag.end(); ++it)
    {
        if (perm_marked_nodes.size() == dag.size())
        {
            break;
        }

        visit(it->first);
    }

    return sorted_list;
}

int main()
{
    std::vector<std::string> words = GetWords();
    Dag dag;

    // Build the graph.
    for (size_t i = 0; i < words.size() - 1; ++i)
    {
        ProcessWordPair(words[i], words[i + 1], &dag);
    }

    // Print the dependencies of each node in the graph.
    for (auto it = dag.begin(); it != dag.end(); ++it)
    {
        std::cout << it->first << " { ";
        for (auto it_set = it->second.begin(); it_set != it->second.end();
             ++it_set)
        {
            std::cout << *it_set << " ";
        }
        std::cout << "}" << std::endl;
    }
    std::cout << std::endl;

    // Sort it out.
    std::list<char> sort_order = TopoSort(dag);

    for (auto it = sort_order.begin(); it != sort_order.end(); ++it)
    {
        std::cout << *it << " ";
    }
    std::cout << std::endl;

    return 0;
}

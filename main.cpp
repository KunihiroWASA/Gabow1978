#include <iostream>
#include <vector>
#include <stack>
#include <queue>
#include <map>
#include <utility>
#include <algorithm>
#include <iterator>

typedef int                   Node;
typedef std::vector<Node>     AdjList;
typedef std::map<Node, AdjList>  Graph;
typedef std::pair<Node, Node> Edge;

std::ostream& operator<<(std::ostream& os, const Edge e)
{
    os << "(" << e.first << ", " << e.second << ")"; 
    return os; 
}

class Subtree {
    std::vector<Node> nodes; 
    std::vector<Edge> edges; 

    public: 

    Subtree():nodes({}), edges({}){}; 


    Subtree & operator=(const Subtree & other) {
        nodes.clear(); 
        edges.clear(); 
        std::copy(other.nodes.begin(), other.nodes.end(), std::back_inserter(nodes));  
        std::copy(other.edges.begin(), other.edges.end(), std::back_inserter(edges));  
        return (*this); 
    }

    std::size_t size() {
        return nodes.size(); 
    }

    bool add(Edge e) {
        if (has(e.first) and !has(e.second)) {
            add(e.second);  
            edges.push_back(e); 
            return true; 
        }
        return false; 
    }
    bool add(Node v) {
        if (!has(v)) {
            nodes.push_back(v); 
            return true; 
        }
        std::cout << "Error: Node is added more than twice; " << std::endl; 
        return false; 
    }

    bool deletee(Edge e) {
        edges.erase(std::remove(edges.begin(), edges.end(), e), edges.end()); 
        nodes.erase(std::remove(nodes.begin(), nodes.end(), e.second), nodes.end()); 
        return true; 
    }

    bool has(Node n) {
        return nodes.end() != std::find(nodes.begin(), nodes.end(), n); 
    }

    bool has(Edge e) {
        return edges.end() != std::find(edges.begin(), edges.end(), e); 
    }

    void output() 
    {
        for (auto& e : edges) {
            std::cout << e << " "; 
        }
        std::cout << "[ ";
        for (auto& n : nodes) {
            std::cout << n << " " ;  
        }
        std::cout  << "]" << std::endl; 
    }

    bool bridge_test(Node v, Graph & __g) 
    {
        for (auto& a: __g) {
            Node w = a.first; 
            for (auto& u : a.second) {
                if (u == v) {
                    if (!is_decendent_of(w, v)) {
                        return true; 
                    }
                }
            }
        }
        return false; 
    } 


    // Is w a decendent of v ? 
    bool is_decendent_of(Node w, Node v)
    {
        bool f = true; 
        while (f) {
            f = false; 
            for (auto& e : edges) {
                if (e.second == w) {
                    if (e.first == v) {
                        return true; 
                    } else {
                        f = true; 
                        w = e.first;
                        break; 
                    }
                }
            }
        }
        return false; 
    }
};

class S {

    Graph __g; 
    int n; 

    Subtree T; 
    Subtree L; 
    std::stack<Edge> F; 
    void GROW(); 
    void OUTPUT(); 

    void updateF(Node v, std::queue<std::pair<int, Edge>> & __restore_queue, int & push_num); 
    void restoreF(Node v, std::queue<std::pair<int, Edge>> & __restore_queue, int pop_num); 
    void remove(Edge e); 
    void add(Edge e); 

    void outputF(); 


    public:
        S(){}; 
        void MAIN(Graph & g, Node r); 
};

void S::GROW() 
{
    if ((int)T.size() == n) {
        L = T; 
        OUTPUT(); 
    } else {
        std::stack<Edge> FF; 
        bool b; 
        do {
            if (F.empty()) {
                break; 
            }
            Edge e = F.top(); F.pop(); 
            if (!T.add(e)) {
                T.output(); 
                std::cout << e << std::endl; 
                std::cout << "ERROR" << std::endl; 
                exit(1); 
            }

            Node v = e.second; 

            int pushed_num; 
            std::queue<std::pair<int, Edge>> __restore_queue; 
            updateF(v, __restore_queue, pushed_num); 
            GROW(); 
            restoreF(v, __restore_queue, pushed_num); 

            remove(e); 
            FF.push(e); 

            b = true; 
            if (L.bridge_test(v, __g)) {
                b = false; 
            }
        } while (!b);

        while (!FF.empty()) {
            Edge e = FF.top(); 
            F.push(e); 
            add(e); 
            FF.pop(); 
        }
    }
}

void S::remove(Edge e) {
    // remove e from T
    T.deletee(e); 
    // remove e from G
    __g[e.first].erase(std::remove(__g[e.first].begin(), __g[e.first].end(), e.second), __g[e.first].end()); 
}

void S::add(Edge e) {
    // add e from G
    __g[e.first].push_back(e.second); 
}

void S::updateF(Node v, std::queue<std::pair<int, Edge>> & __restore_queue, int & push_num) {
    push_num = 0; 
    for (auto& w : __g[v]) {
        if (!T.has(w)) {
            F.push(std::make_pair(v, w)); 
            ++push_num; 
        }
    }

    std::stack<Edge> _F; 
    
    int i = 0; 
    while (!F.empty()) {
        Edge e = F.top(); 
        if (T.has(e.first) and e.second == v) {
            __restore_queue.push(std::make_pair(i, e)); 
        } else {
            _F.push(e); 
        }
        F.pop(); 

        ++i; 
    }
    while (!_F.empty()) {
        F.push(_F.top()); 
        _F.pop(); 
    }
}

void S::restoreF(Node v, std::queue<std::pair<int, Edge>> & __restore_queue, int pop_num) {
    std::stack<Edge> _F; 
    
    int i = 0; 
    while (!F.empty()) {
        if (!__restore_queue.empty()) {
            if (__restore_queue.front().first == i++) {
                Edge e = __restore_queue.front().second; 
                _F.push(e); 
                __restore_queue.pop(); 
                continue; 
            }
        }

        Edge e = F.top(); 
        _F.push(e); 
        F.pop(); 
    }

    while(!__restore_queue.empty()) {
        Edge e = __restore_queue.front().second; 
        _F.push(e); 
        __restore_queue.pop(); 
    }

    while (!_F.empty()) {
        F.push(_F.top()); 
        _F.pop(); 
    }


    while (pop_num-- != 0 ) {
        F.pop(); 
    }

}

void S::OUTPUT()
{
    std::cout << "ST: "; 
    L.output();  
}

void S::MAIN(Graph & g, Node r) {
    __g = g;
    n   = (int)__g.size();
    for (auto& v : __g[r]) {
        F.push(std::make_pair(r, v)); 
    }
    T.add(r); 
    GROW(); 
}


void S::outputF() {
    std::stack<Edge> _F; 
    std::cout << "F: "; 
    while (!F.empty()) {
        Edge e = F.top(); 
        _F.push(e); 
        F.pop(); 

        std::cout << e << " ->  "; 
    }
    std::cout << "|" << std::endl; 

    while (!_F.empty()) {
        F.push(_F.top()); 
        _F.pop(); 
    }
}

int main(int argc, char const* argv[])
{
    Graph g = {
        {1, {2, 3}}, 
        {2, {3, 4}},
        {3, {2}}, 
        {4, {3}}
    }; 

    S s; 
    s.MAIN(g, 1);  
    return 0;
}

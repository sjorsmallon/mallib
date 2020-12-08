

template <typename T>
struct Node {
    T val;
    Node *next;

    Node(const T& t = T()) : val(t) {}
};

template <typename T>
struct node_iter {
    Node<T>* current;
    node_iter(Node<T>* current): current(current) {}

    const node_iter& operator++() { current = current->next; return *this; }
    node_iter operator++(int) {
        node_iter result = *this; ++(*this); return result;
    }
    T& operator*() { return current->val; }
};

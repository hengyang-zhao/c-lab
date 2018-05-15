#include <iostream>
#include <list>
#include <stack>
#include <string>

struct Tree
{
    struct Node
    {
        int value;
        Node *left;
        Node *right;

        Node(int v): value(v), left(nullptr), right(nullptr) {}
        ~Node()
        {
            if (left) delete left;
            if (right) delete right;
        }
    };

    Node *root;

    Tree(): root(nullptr) {}
    ~Tree() { Clear(); }

    void Clear()
    {
        if (root) {
            delete root;
            root = nullptr;
        }
    }

    void ParseNullInclusivePreorder(std::istream &input);
    void FormatNullInclusivePreorder(std::ostream &output) const;
};

void Tree::ParseNullInclusivePreorder(std::istream &input)
{
    Clear();
    std::string token;
    std::stack<Node **> stack;
    stack.push(&root);

    while (input >> token) {
        Node **node = stack.top();
        stack.pop();

        if (token != "null") {
            int value = std::stoi(token);
            *node = new Node(value);
            stack.push(&(*node)->right);
            stack.push(&(*node)->left);
        }
    }
}

void Tree::FormatNullInclusivePreorder(std::ostream &output) const
{
    std::stack<Node *> stack;
    stack.push(root);
    bool is_first_element = true;

    while (not stack.empty()) {
        Node *node = stack.top();
        stack.pop();

        output << (is_first_element ? "" : " ")
               << (node ? std::to_string(node->value) : "null");

        if (node) {
            stack.push(node->right);
            stack.push(node->left);
        }
        is_first_element = false;
    }

    output << std::endl;
}

int main()
{
    Tree tree;
    tree.ParseNullInclusivePreorder(std::cin);
    tree.FormatNullInclusivePreorder(std::cout);
}

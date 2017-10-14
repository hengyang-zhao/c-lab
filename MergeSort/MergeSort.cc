#include <iostream>
#include <random>
#include <sstream>
#include <stdexcept>

template <class T>
struct Node
{
    T value;
    Node<T> *next;
};

template <class T>
std::ostream &operator<<(std::ostream &os, const Node<T> &node)
{
    return os << '<' << node.value << " @" << &node << '>';
};

template <class T>
Node<T> *MergeSort(Node<T> *head)
{
    // Recursion terminals
    if (head == nullptr or head->next == nullptr) return head;

    // Split
    Node<T> *middle;
    {
        Node<T> **slow = &head;
        for (Node<T> *fast = head->next;;) {
            if (fast) fast = fast->next; else break;
            if (fast) fast = fast->next; else break;
            slow = &(*slow)->next;
        }
        middle = (*slow)->next;
        (*slow)->next = nullptr;
    }

    // Sort
    head = MergeSort(head);
    middle = MergeSort(middle);

    // Merge
    Node<T> *sorted_head;
    for (Node<T> **sorted_tail(&sorted_head);;) {
        if (head == nullptr) {
            *sorted_tail = middle;
            break;
        } else if (middle == nullptr) {
            *sorted_tail = head;
            break;
        } else {
            if (head->value < middle->value) {
                *sorted_tail = head;
                sorted_tail = &head->next;
                head = head->next;
            } else {
                *sorted_tail = middle;
                sorted_tail = &middle->next;
                middle = middle->next;
            }
        }
    }

    return sorted_head;
}

template <class T>
void PrintList(const Node<T> *head, const std::string &title)
{
    std::cout << "---- " << title << " ----" << std::endl;
    while (head) {
        std::cout << *head << std::endl;
        head = head->next;
    }
}

template <class T>
T LexicalCast(const std::string &s)
{
    T result;
    std::stringstream ss(s);
    ss >> result;
    if (not ss or not ss.eof()) throw std::invalid_argument(s);
    return result;
}

int main(int argc, char **argv)
{
    constexpr int size = 10;
    Node<int> nodes[size];

    std::default_random_engine rnd_engine;
    std::uniform_int_distribution<int> dist(0, 999);

    if (argc > 1) rnd_engine.seed(LexicalCast<int>(argv[1]));

    for (int i = 0; i < size; ++i) {
        nodes[i].next = (i == size - 1) ? nullptr : &nodes[i + 1];
        nodes[i].value = dist(rnd_engine);
    }

    static_assert(size > 0);
    PrintList(&nodes[0], "Original");
    PrintList(MergeSort(&nodes[0]), "Sorted");

    return 0;
}

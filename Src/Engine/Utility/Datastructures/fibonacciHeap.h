//
// Created by Kelvin Macartney on 04/04/2020.
//

#ifndef VENUS_FIBONACCIHEAP_H
#define VENUS_FIBONACCIHEAP_H

#include <functional>
#include <list>
#include <cmath>

namespace Venus::Utility::DataStructures {
    template<typename T>
    struct FibHeapNode {
        float key;

        FibHeapNode *left;
        FibHeapNode *right;
        FibHeapNode *parent;
        FibHeapNode *child;

        int degree;
        bool mark;

        T element;
    };

    template<typename T>
    class FibonacciHeap {
    public:
        FibHeapNode<T> *m_minNode;
        int m_numOfNodes;

        FibonacciHeap() {  // initialize a new and empty Fib Heap
            m_minNode = nullptr;
            m_numOfNodes = 0;
        }

        ~FibonacciHeap() {
            _clear(m_minNode);
        }

        /* Insert a node with key value new_key
           and return the inserted node*/
        FibHeapNode<T> *insert(T newNode, float key) {
            FibHeapNode<T> *node = _create_node(newNode, key);
            _insert_node(node);

            return node;
        }

        /* Merge current heap with another*/
        void merge(FibonacciHeap<T> &another) {
            m_minNode = _merge(m_minNode, another.m_minNode);
            m_numOfNodes += another.m_numOfNodes;

            another.m_minNode = nullptr; // so that another
            another.m_numOfNodes = 0;
        }

        /* Return the key of the minimum node*/
        T getMinimum() {
            if (this->m_minNode == nullptr)
                return nullptr;

            return this->m_minNode->element;
        }

        /* Decrease the key of node x to newKey*/
        void updateKey(FibHeapNode<T> *x, float newKey) {
            _decrease_key(x, newKey);
        }

        /*Delete a specified node*/
        void popMinimum() {
            FibHeapNode<T> *min = m_minNode;

            if (min != nullptr) // the heap is not empty
            {
                _unparent_all(min->child);
                _merge(min, min->child); // merge the child circular list into root
                _remove_from_circular_list(min);
                if (min != min->right) // the heap will be empty after the operation
                {
                    m_minNode = min->right; // minNode need not be the minimum at this time
                    _consolidate();
                } else m_minNode = nullptr;
                m_numOfNodes--;
            }
        }

        size_t size() {
            return this->m_numOfNodes;
        }

    private:

        FibHeapNode<T> *_create_node(T newNode, float newKey) {
            auto *node = new FibHeapNode<T>();

            node->key = newKey;
            node->element = newNode;
            node->left = node;
            node->right = node;

            node->parent = nullptr;
            node->child = nullptr;
            node->degree = 0;
            node->mark = false;
            return node;
        }

        void _insert_node(FibHeapNode<T> *newNode) {
            m_minNode = _merge(m_minNode, newNode);
            m_numOfNodes++;
        }

        void _remove_from_circular_list(FibHeapNode<T> *x) {
            if (x->right == x) { // the root list only has one node before the operation
                return;
            }
            FibHeapNode<T> *leftSib = x->left;
            FibHeapNode<T> *rightSib = x->right;
            leftSib->right = rightSib;
            rightSib->left = leftSib;
        }

        FibHeapNode<T> *_merge(FibHeapNode<T> *a, FibHeapNode<T> *b) {
            if (a == nullptr)
                return b;
            if (b == nullptr)
                return a;

            if (a->key > b->key) // swap node
            {
                FibHeapNode<T> *temp = a;
                a = b;
                b = temp;
            }

            FibHeapNode<T> *aRight = a->right;
            FibHeapNode<T> *bLeft = b->left;

            a->right = b;
            b->left = a;
            aRight->left = bLeft;
            bLeft->right = aRight;

            return a;
        }

        void _make_child(FibHeapNode<T> *child, FibHeapNode<T> *parent) {
            _remove_from_circular_list(child);
            child->left = child->right = child;
            child->parent = parent;
            parent->child = _merge(parent->child, child); // add child into parent's children list
            parent->degree++;
            child->mark = false;
        }

        void _consolidate() {
            int Dn = (int) (log2(m_numOfNodes) / log2(1.618));
            auto **A = new FibHeapNode<T> *[Dn + 1];

            for (int i = 0; i < Dn + 1; i++)
                A[i] = nullptr;

            FibHeapNode<T> *x = m_minNode;
            bool breakFlag = false;
            while (true) {
                int d = x->degree;
                while (A[d] != nullptr) {
                    FibHeapNode<T> *y = A[d];
                    if (y == x) {
                        breakFlag = true;  // when y == x, all root nodes have different degree
                        break;               // so break out of the whole loop
                    }
                    if (x->key > y->key) // swap x and y, so x always points to the
                        // node with smaller key
                    {
                        FibHeapNode<T> *temp = x;
                        x = y;
                        y = temp;
                    }
                    _make_child(y, x); // make y the child of x
                    A[d++] = nullptr; // now the new node has (d + 1) child, so A[d] = nullptr,d = d + 1
                }
                if (breakFlag)
                    break;
                A[x->degree] = x;
                x = x->right; // to next node in the root list
            }
            m_minNode = x;  // update the m_minNode
            FibHeapNode<T> *iter = x;
            do {
                if (iter->key < m_minNode->key)
                    m_minNode = iter;
                iter = iter->right;
            } while (iter != x);
            delete[]A;
        }

        void _unparent_all(FibHeapNode<T> *x) {
            if (x == nullptr)
                return;
            FibHeapNode<T> *y = x;
            do {
                y->parent = nullptr;
                y = y->right;
            } while (y != x);
        }

        void _decrease_key(FibHeapNode<T> *x, float newKey) {
            x->key = newKey;
            FibHeapNode<T> *y = x->parent;

            if (y != nullptr && x->key < y->key) {
                _cut(x, y);
                _cascading_cut(y);
            }

            if (x->key < m_minNode->key)
                m_minNode = x;

            _consolidate();
        }

        void _cut(FibHeapNode<T> *x, FibHeapNode<T> *y) {
            _remove_from_circular_list(x);
            if (x->right == x) // x is the only child of y
            {
                y->child = nullptr;
            } else {
                y->child = x->right; // update y's child
            }
            y->degree--;
            _merge(m_minNode, x);
            x->parent = nullptr;
            x->mark = false;
        }

        void _cascading_cut(FibHeapNode<T> *y) {
            FibHeapNode<T> *z = y->parent;

            if (z != nullptr) {
                if (!y->mark)
                    y->mark = true;
                else {
                    _cut(y, z);
                    _cascading_cut(z);
                }
            }
        }

        void _clear(FibHeapNode<T> *x) {
            if (x != nullptr) {
                FibHeapNode<T> *t1 = x;
                do {
                    FibHeapNode<T> *t2 = t1;
                    t1 = t1->right;
                    _clear(t2->child);
                    delete t2;
                } while (t1 != x);
            }
        }
    };
}

#endif //VENUS_FIBONACCIHEAP_H

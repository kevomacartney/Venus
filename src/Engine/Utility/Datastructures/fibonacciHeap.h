//
// Created by Kelvin Macartney on 04/04/2020.
//

#ifndef VENUS_BINARYHEAP_H
#define VENUS_BINARYHEAP_H

namespace Venus::Utility::DataStructures {
    template<typename T>
    class BinaryHeap {
    public:
        BinaryHeap(bool isMinHeap = true);

        /**
         * Adds a new node to the binary heap
         * @param node
         */
        void addNode(T node);
    private:
        bool _isMinHeap{false};
        T *_heap;


    };
}

#endif //VENUS_BINARYHEAP_H

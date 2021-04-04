#pragma once

#ifndef __SLSTACK_H__
#define __SLSTACK_H__
#include <stdint.h>

namespace slstack {
    template <class _Type>
    struct StackNode {
        StackNode() : super(nullptr), sub(nullptr), data(nullptr) { }
        ~StackNode() { if ((this->data)) { delete this->data; }}
        StackNode(StackNode<_Type> *, StackNode<_Type> *, const _Type &);
        StackNode(const _Type &);
        
        StackNode<_Type> *super;
        StackNode<_Type> *sub;
        _Type *data;
    };

    template <class _Type>
    StackNode<_Type>::StackNode(StackNode<_Type> *super, StackNode<_Type> *sub, const _Type &data)
    : super(super), sub(sub), data(new _Type(data)) { }

    template <class _Type>
    StackNode<_Type>::StackNode(const _Type &data)
    : super(nullptr), sub(nullptr), data(new _Type(data)) { }

    template <class _Type>
    class Stack {
    public:
        Stack() : _top(nullptr), _length(0) { }
        ~Stack();

        Stack<_Type> &push(const _Type &);
        Stack<_Type> &pop(_Type &);
        inline bool empty() const;
        inline size_t size() const;

    private:
        StackNode<_Type> *_top;
        size_t _length;
    };

    template <class _Type>
    Stack<_Type> &Stack<_Type>::push(const _Type &data) {
        auto element = new StackNode<_Type>(data);
        if (!(element->data)) { return *this; }
        if (!(this->_top)) {
            this->_top = element;
        }
        else {
            this->_top->sub = element;
            element->super = this->_top;
            this->_top = this->_top->sub;
        }
        this->_length++;
        
        return *this;
    }

    template <class _Type>
    Stack<_Type> &Stack<_Type>::pop(_Type &data) {
        if ((this->_top)) {
            data = *(this->_top->data);
            delete this->_top->sub;
            this->_top = (this->_top->super) ? this->_top->super : nullptr;
            this->_length--;
            (this->_top)?(this->_top->sub = nullptr):(nullptr);
        }
        return *this;
    }

    template <class _Type>
    Stack<_Type>::~Stack() {
        while (!this->empty()) {
            if ((this->_top->super)) {
                this->_top =  this->_top->super;
                delete this->_top->sub;
            }
            else {
                delete this->_top;
                this->_top = nullptr;
            }
        }
        this->_length = 0;
    }

    template <class _Type>
    bool Stack<_Type>::empty() const {
        return !(this->_top);
    }

    template <class _Type>
    size_t Stack<_Type>::size() const {
        return (this->_length);
    }
}

#endif /* __SLSTACK_H__ */

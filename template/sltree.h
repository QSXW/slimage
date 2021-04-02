#pragma once

#ifndef __SLTREE_H__
#define __SLTREE_H__
#include <iostream>
#include <cstring>
#include <cstdint>

namespace sltree {
    enum TraversalMethods {
        Preorder,
        Inorder,
        Postorder
    };

    namespace slRedBlackTree {
        #define NIL (nullptr)
        #define REDBLACK_LEFT 0
        #define REDBLACK_RIGHT 1
        enum RedBlackColor {
            Black = 0,
            Red = 1
        };

        template <class _Type>
        struct RedBlackNode {
        public:
            RedBlackNode() :super(NIL), sub{ NIL, NIL }, color(RedBlackColor::Black), data(nullptr) { }
            ~RedBlackNode() { if ((this->data)) { delete data; } }
            RedBlackNode(const _Type &);
            RedBlackNode(RedBlackNode<_Type> *, RedBlackNode<_Type> *, RedBlackNode<_Type> *, uint8_t, const _Type &);

            void destroy(RedBlackNode<_Type> *);
            inline RedBlackNode<_Type> *Super() const noexcept;
            inline RedBlackNode<_Type> *Sibling() const noexcept;
        public:
            RedBlackNode<_Type> *super;
            RedBlackNode<_Type> *sub[2];
            uint8_t color;
            _Type *data;
        };

        template <class _Type>
        void RedBlackNode<_Type>::destroy(RedBlackNode<_Type> *node) {
            if ((node)) {
                this->destroy(node->sub[REDBLACK_LEFT]);
                this->destroy(node->sub[REDBLACK_RIGHT]);
                delete node;
                node = nullptr;
            }
        }

        template <class _Type>
        RedBlackNode<_Type>::RedBlackNode(RedBlackNode<_Type> *super, RedBlackNode<_Type> *left, RedBlackNode<_Type> *right, uint8_t color, const _Type &data)
        : super(super), sub{ left, right }, color(color), data(new _Type(data)) { }
        
        template <class _Type>
        RedBlackNode<_Type>::RedBlackNode(const _Type &data) : super(NIL), sub{ NIL, NIL }, color(RedBlackColor::Red), data(new _Type(data)) { }

        template <class _Type>
        RedBlackNode<_Type> *RedBlackNode<_Type>::Super() const noexcept {
            return (this->super ? this->super : NIL);
        }

        template <class _Type>
        RedBlackNode<_Type> *RedBlackNode<_Type>::Sibling() const noexcept {
            return (this->Super() ? (this->Super()->sub[(this->Super()->sub[REDBLACK_LEFT] == this) ? REDBLACK_RIGHT : REDBLACK_RIGHT]) : NIL);
        }

        template <class _Type>
        class RedBlackTree {
        public:
            RedBlackTree() : _root(nullptr), _length(0) { }
            ~RedBlackTree() {  }

        public:
            void destroy();
            inline size_t size() const;
            RedBlackNode<_Type>* find(const _Type &);
            RedBlackTree<_Type>& insert(const _Type &);
            RedBlackTree<_Type>& remove(RedBlackNode<_Type> *);
            inline RedBlackTree<_Type>& replace(const _Type &, const _Type &);
            inline RedBlackTree<_Type> & insert(const _Type &&);
            inline RedBlackTree<_Type>& insert(const std::initializer_list<_Type> &);
            inline RedBlackTree<_Type>& remove(const _Type &);
            inline void traversal(uint32_t);

        private:
            void _LeftRotate_(RedBlackNode<_Type> *);
            void _RightRotate_(RedBlackNode<_Type> *);
            void _InsertFixUp_(RedBlackNode<_Type> *);
            inline void _Transplant_(RedBlackNode<_Type> *, RedBlackNode<_Type> *);
            inline RedBlackNode<_Type> * _Minimum_(RedBlackNode<_Type> *);
            inline RedBlackNode<_Type> * _Maximum_(RedBlackNode<_Type> *);
            void _DeleteFixUp_(RedBlackNode<_Type> *);
            inline size_t _InsertSucceed_();
            inline size_t _DeleteSucceed_();
            void _PreorderTraversal_(RedBlackNode<_Type> *);
            void _InorderTraversal_(RedBlackNode<_Type> *);
            void _PostorderTraversal_(RedBlackNode<_Type> *);

        public:
            using valueType = _Type;
            using caller = void(RedBlackTree<_Type>::*)(RedBlackNode<_Type> *);
        private:
            static const caller (vfptr[]);
            RedBlackNode<_Type> *_root;
            size_t _length;
        };

        template <class _Type>
        size_t RedBlackTree<_Type>::_InsertSucceed_() {
            return this->_length += 1;
        }

        template <class _Type>
        size_t RedBlackTree<_Type>::_DeleteSucceed_() {
            return this->_length -= 1;
        }

        template <class _Type>
        size_t RedBlackTree<_Type>::size() const {
            return this->_length;
        }

        template <class _Type>
        void RedBlackTree<_Type>::destroy() {
            if (this->_root) {
                this->_root->destroy(this->_root);
                memset(static_cast<void *>(this), 0x0, sizeof(*this));
            }
        }

        template <class _Type>
        RedBlackNode<_Type>* RedBlackTree<_Type>::find(const _Type &data) {
            RedBlackNode<_Type> *currentNode = this->_root;
            while (currentNode && (data != *(currentNode->data))) {
                currentNode = currentNode->sub[(data < *(currentNode->data)) ? REDBLACK_LEFT : REDBLACK_RIGHT];
            }
            return currentNode;
        }

        template <class _Type>
        RedBlackTree<_Type> &RedBlackTree<_Type>::insert(const _Type &data) {
            RedBlackNode<_Type> *node = new RedBlackNode<_Type>(data);
            if (!(this->_root)) {
                node->color = RedBlackColor::Black;
                this->_root = node;
                this->_InsertSucceed_();
                return *this;
            }

            RedBlackNode<_Type> *currentNode = this->_root;
            RedBlackNode<_Type> *potentialSuper = currentNode;
            while ((currentNode)) {
                potentialSuper = currentNode;
                currentNode = currentNode->sub[(*(node->data) < *(currentNode->data)) ? REDBLACK_LEFT : REDBLACK_RIGHT];
            }
            node->super = potentialSuper;
            node->super->sub[(*(node->data) < *(node->super->data)) ? REDBLACK_LEFT : REDBLACK_RIGHT] = node;

            this->_InsertFixUp_(node);
            this->_InsertSucceed_();

            return *this;
        }

        template <class _Type>
        RedBlackTree<_Type>& RedBlackTree<_Type>::insert(const _Type &&data) {
            return this->insert(data);
        }

        template <class _Type>
        RedBlackTree<_Type>& RedBlackTree<_Type>::insert(const std::initializer_list<_Type> &list) {
            for (auto it = list.begin(); it < list.end(); it++) {
                this->insert(*it);
            }
            return *this;
        }

        template <class _Type>
        void RedBlackTree<_Type>::_Transplant_(RedBlackNode<_Type> *u, RedBlackNode<_Type> *v) {
            if (!(u->super)) {
                this->_root = v;
            }
            else if (u == u->super->sub[REDBLACK_LEFT]) {
                u->super->sub[REDBLACK_LEFT] = v;
            }
            else {
                u->super->sub[REDBLACK_RIGHT] = v;
            }
            if ((v)) {
                v->super = (u) ? u->super : NIL;
            }
        }

        template <class _Type>
        RedBlackTree<_Type>& RedBlackTree<_Type>::replace(const _Type &o, const _Type &n) {
            this->remove(o);
            this->insert(n);
            return *this;
        }

        template <class _Type>
        RedBlackNode<_Type> * RedBlackTree<_Type>::_Minimum_(RedBlackNode<_Type> *x) {
            while ((x) && (x->sub[REDBLACK_LEFT])) {
               x = x->sub[REDBLACK_LEFT];
            }
            return x;
        }

        template <class _Type>
        RedBlackNode<_Type> * RedBlackTree<_Type>::_Maximum_(RedBlackNode<_Type> *x) {
            while ((x) && (x->sub[REDBLACK_RIGHT])) {
               x = x->sub[REDBLACK_RIGHT];
            }
            return x;
        }

        template <class _Type>
        void RedBlackTree<_Type>::_DeleteFixUp_(RedBlackNode<_Type> *node) {
            while (node != this->_root && node->color == RedBlackColor::Black) {
                if (node == node->super->sub[REDBLACK_LEFT]) {
                    RedBlackNode<_Type> *w = node->super->sub[REDBLACK_RIGHT];
                    if (w->color == RedBlackColor::Red) {
                        w->color = RedBlackColor::Black;
                        node->super->color = RedBlackColor::Red;
                        this->_LeftRotate_(node->super);
                        w = node->super->sub[REDBLACK_RIGHT];
                    }
                    if (w->sub[REDBLACK_LEFT]->color == RedBlackColor::Black && w->sub[REDBLACK_RIGHT]->color == RedBlackColor::Black) {
                        w->color = RedBlackColor::Red;
                        node = node->super;
                    }
                    else {
                        if (w->sub[REDBLACK_RIGHT]->color == RedBlackColor::Black) {
                            w->sub[REDBLACK_LEFT]->color = RedBlackColor::Black;
                            w->color = RedBlackColor::Red;
                            this->_RightRotate_(w);
                            w = w->super->sub[REDBLACK_RIGHT];
                        }
                        w->color = node->super->color;
                        w->sub[REDBLACK_RIGHT]->color = node->super->color = RedBlackColor::Black;
                        this->_LeftRotate_(node->super);;
                        node = this->_root;
                    }
                }
                else {
                    RedBlackNode<_Type> *w = node->super->sub[REDBLACK_LEFT];
                    if (w->color == RedBlackColor::Red) {
                        w->color = RedBlackColor::Black;
                        node->super->color = RedBlackColor::Red;
                        this->_RightRotate_(node->super);
                        w = node->super->sub[REDBLACK_LEFT];
                    }
                    if (w->sub[REDBLACK_LEFT]->color == RedBlackColor::Black && w->sub[REDBLACK_RIGHT]->color == RedBlackColor::Black) {
                        w->color = RedBlackColor::Red;
                        node = node->super;
                    }
                    else {
                        if (w->sub[REDBLACK_LEFT]->color == RedBlackColor::Black) {
                            w->sub[REDBLACK_RIGHT]->color = RedBlackColor::Black;
                            w->color = RedBlackColor::Red;
                            this->_LeftRotate_(w);
                            w = w->super->sub[REDBLACK_LEFT];
                        }
                        w->color = node->super->color;
                        w->sub[REDBLACK_LEFT]->color = node->super->color = RedBlackColor::Black;
                        this->_RightRotate_(node->super);;
                        node = this->_root;
                    }
                }
            }
            node->color = RedBlackColor::Black;
        }

        template <class _Type>
        RedBlackTree<_Type>& RedBlackTree<_Type>::remove(RedBlackNode<_Type> *node) {
            if (!(node)) {
                return *this;
            }
            RedBlackNode<_Type> *y = node;
            RedBlackNode<_Type> *x = nullptr;
            auto originalColor = y->color;

            if (!(node->sub[REDBLACK_LEFT])) {
                x = node->sub[REDBLACK_RIGHT];
                this->_Transplant_(node, x);
            }
            else if (!(node->sub[REDBLACK_RIGHT])) {
                x = node->sub[REDBLACK_LEFT];
                this->_Transplant_(node, x);
            }
            else {
                y = this->_Minimum_(node->sub[REDBLACK_RIGHT]);
                originalColor = y->color;
                x = y->sub[REDBLACK_RIGHT];
                if (y->super == node) {
                    if ((x) && (x->super = y));
                }
                else {
                    this->_Transplant_(y, y->sub[REDBLACK_RIGHT]);
                    y->sub[REDBLACK_RIGHT] = node->sub[REDBLACK_RIGHT];
                    y->sub[REDBLACK_RIGHT]->super = y;
                }
                this->_Transplant_(node, y);
                y->sub[REDBLACK_LEFT] = node->sub[REDBLACK_LEFT];
                y->sub[REDBLACK_LEFT]->super = y;
                y->color = node->color;
            }
            delete node;
            if (originalColor == RedBlackColor::Black) {
                if ((x)) {
                    this->_DeleteFixUp_(x);
                }
            }
            
            this->_DeleteSucceed_();
            return *this;
        }

        template <class _Type>
        RedBlackTree<_Type>& RedBlackTree<_Type>::remove(const _Type &data) {
            return (this->remove(this->find(data)));
        }

        template <class _Type>
        void RedBlackTree<_Type>::_InsertFixUp_(RedBlackNode<_Type> *node) {
            while (node != this->_root && node->super->color == RedBlackColor::Red) {
                RedBlackNode<_Type> *siblingOfSuper = nullptr;
                if (node->super == node->super->super->sub[REDBLACK_LEFT]) {
                    siblingOfSuper = node->super->super->sub[REDBLACK_RIGHT];
                    if ((siblingOfSuper) && siblingOfSuper->color == RedBlackColor::Red) {
                        node->super->color = siblingOfSuper->color = RedBlackColor::Black;
                        node->super->super->color = RedBlackColor::Red;
                        node = node->super->super;
                    }
                    else if (node == node->super->sub[REDBLACK_RIGHT]) {
                        node = node->super;
                        this->_LeftRotate_(node);
                    }
                    if ((node->super)) {
                        node->super->color = RedBlackColor::Black;
                        if ((node->super->super)) {
                            node->super->super->color = RedBlackColor::Red;
                            this->_RightRotate_(node->super->super);
                        }
                    }
                }
                else {
                    siblingOfSuper = node->super->super->sub[REDBLACK_LEFT];
                    if ((siblingOfSuper) && siblingOfSuper->color == RedBlackColor::Red) {
                        node->super->color = siblingOfSuper->color = RedBlackColor::Black;
                        node->super->super->color = RedBlackColor::Red;
                        node = node->super->super;
                    }
                    else if (node == node->super->sub[REDBLACK_LEFT]) {
                        node = node->super;
                        this->_RightRotate_(node);
                    }
                    if ((node->super)) {
                        node->super->color = RedBlackColor::Black;
                        if ((node->super->super)) {
                            node->super->super->color = RedBlackColor::Red;
                            this->_LeftRotate_(node->super->super);
                        }
                    }
                }
            }
            this->_root->color = RedBlackColor::Black;
        }
        
        template <class _Type>
        void RedBlackTree<_Type>::_LeftRotate_(RedBlackNode<_Type> *node) {
            /* the axis x-->y equals to node --> node->sub[right] */
            RedBlackNode<_Type> *sibling = node->sub[REDBLACK_RIGHT];
            node->sub[REDBLACK_RIGHT] = sibling->sub[REDBLACK_LEFT];
            if ((sibling->sub[REDBLACK_LEFT])) {
                sibling->sub[REDBLACK_LEFT]->super = node;
            }
            sibling->super = node->super;
            if (!(node->super)) {
                this->_root = sibling;
            }
            else {
                node->super->sub[(node == node->super->sub[REDBLACK_LEFT]) ? REDBLACK_LEFT : REDBLACK_RIGHT] = sibling;
            }
            sibling->sub[REDBLACK_LEFT] = node;
            node->super = sibling;
        }

        template <class _Type>
        void RedBlackTree<_Type>::_RightRotate_(RedBlackNode<_Type> *node) {
            RedBlackNode<_Type> *sibling = node->sub[REDBLACK_LEFT];
            node->sub[REDBLACK_LEFT] = sibling->sub[REDBLACK_RIGHT];
            if ((sibling->sub[REDBLACK_RIGHT])) {
                sibling->sub[REDBLACK_RIGHT]->super = node;
            }
            sibling->super = node->super;
            if (!(node->super)) {
                this->_root = sibling;
            }
            else {
                node->super->sub[(node == node->super->sub[REDBLACK_RIGHT]) ? REDBLACK_RIGHT : REDBLACK_LEFT] = sibling;
            }
            sibling->sub[REDBLACK_RIGHT] = node;
            node->super = sibling;
        }

        template <class _Type>
        void RedBlackTree<_Type>::traversal(uint32_t traveresalMethod) {
           (this->*vfptr[traveresalMethod])(this->_root);
        }
        
        template <class _Type>
        void RedBlackTree<_Type>::_PreorderTraversal_(RedBlackNode<_Type> *node) {
            
        }

        template <class _Type>
        void RedBlackTree<_Type>::_InorderTraversal_(RedBlackNode<_Type> *node) {
            if ((node)) {
                this->_InorderTraversal_(node->sub[REDBLACK_LEFT]);
            }
            if ((node)) {
                std::cout << "color: " << ((node->color) ? "\033[31;40mRED\033[0m" : "BLACK") << "    ";
                std::cout << "value: " << (*(node->data)) << "    ";
                if ((node->super)) {
                    std::cout << "super: " << *(node->super->data) << std::endl;
                }
                else {
                    std::cout << "super: " << 0 << std::endl;
                }
            }
            if ((node)) {
                this->_InorderTraversal_(node->sub[REDBLACK_RIGHT]);
            }
        }

        template <class _Type>
        void RedBlackTree<_Type>::_PostorderTraversal_(RedBlackNode<_Type> *node) {

        }

        template <class _Type>
        const typename RedBlackTree<_Type>::caller RedBlackTree<_Type>::vfptr[] = {
            reinterpret_cast<RedBlackTree<_Type>::caller>(&RedBlackTree<_Type>::_PreorderTraversal_),
            reinterpret_cast<RedBlackTree<_Type>::caller>(&RedBlackTree<_Type>::_InorderTraversal_),
            reinterpret_cast<RedBlackTree<_Type>::caller>(&RedBlackTree<_Type>::_PostorderTraversal_)
        };
    }

    using namespace slRedBlackTree;
    using IntContainer      = RedBlackTree<int>;
    using FloatContainer    = RedBlackTree<float>;
    using StringContainer   = RedBlackTree<std::string>;
}

#endif /* __SLTREE_H__ */

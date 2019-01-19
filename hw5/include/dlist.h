/****************************************************************************
  FileName     [ dlist.h ]
  PackageName  [ util ]
  Synopsis     [ Define doubly linked list package ]
  Author       [ Chung-Yang (Ric) Huang ]
  Copyright    [ Copyleft(c) 2005-present LaDs(III), GIEE, NTU, Taiwan ]
****************************************************************************/

#ifndef DLIST_H
#define DLIST_H

#include <cassert>

template <class T> class DList;

// DListNode is supposed to be a private class. User don't need to see it.
// Only DList and DList::iterator can access it.
//
// DO NOT add any public data member or function to this class!!
//
template <class T>
class DListNode
{
   friend class DList<T>;
   friend class DList<T>::iterator;

   DListNode(const T& d, DListNode<T>* p = 0, DListNode<T>* n = 0):
      _data(d), _prev(p), _next(n) {}

   // [NOTE] DO NOT ADD or REMOVE any data member
   T              _data;
   DListNode<T>*  _prev;
   DListNode<T>*  _next;
};


template <class T>
class DList
{
public:
   // TODO: decide the initial value for _isSorted
   DList() {
      _head = new DListNode<T>(T());
      _head->_prev = _head->_next = _head; 
      _isSorted = false;// _head is a dummy node
   }
   ~DList() { clear(); delete _head; }

   // DO NOT add any more data member or function for class iterator
   class iterator
   {
      friend class DList;

   public:
      iterator(DListNode<T>* n= 0): _node(n) {}
      iterator(const iterator& i) : _node(i._node) {}
      ~iterator() {} // Should NOT delete _node

      // TODO: implement these overloaded operators
      const T& operator * () const { return _node->_data; }
      T& operator * () { return _node->_data; }
      iterator& operator ++ () { _node = _node->_next; return *this; }
      iterator operator ++ (int) {iterator temp(*this); operator++(); return temp; }
      iterator& operator -- () { _node = _node->_prev; return *this; }
      iterator operator -- (int) {iterator temp(*this); operator--(); return temp; }

      iterator& operator = (const iterator& i) { _node=i._node; return *this;}
      bool operator != (const iterator& i) const { return i._node!=_node; }
      bool operator == (const iterator& i) const { return i._node==_node; }

   private:
      DListNode<T>* _node;
   };

   // TODO: implement these functions
   iterator begin() const { return _head->_next; }
   iterator end() const { return _head; }
   bool empty() const {if(_head==_head->_prev)return true; return false;}
   size_t size() const 
   { 
      size_t num=0; 
      DListNode<T>* temp=begin()._node;
      while(temp!=_head)
      {
         num++;
         temp=temp->_next;
      } 
      return num; 
   }

   void push_back(const T& x) 
   { 
       _isSorted = false;
      DListNode<T>* newnode = new DListNode<T>(x, _head->_prev, _head);
      _head->_prev->_next = newnode;
      _head->_prev = newnode;
   }
   
   void pop_front() 
   { 
         if(empty()) return;
         _head->_next=_head->_next->_next;
         delete _head->_next->_prev;
         _head->_next->_prev=_head;
   }

   void pop_back() 
   {
      if(empty()) return;
      _head->_prev = _head->_prev->_prev;
      delete _head->_prev->_next;
      _head->_prev->_next=_head;
   }

   // return false if nothing to erase
   bool erase(iterator pos) 
   { 
      if(empty()) return false;
      pos._node->_prev->_next = pos._node->_next;
      pos._node->_next->_prev = pos._node->_prev;
      delete pos._node;
      return true; 
   }
   bool erase(const T& x) 
   { 
      if(empty()) return false;
      DListNode<T>* temp = _head->_next;
      while(temp->_data!=x)
      {
         if(temp==_head)
         {
            return false;
         }
         temp=temp->_next;
      }
      temp->_prev->_next = temp->_next;
      temp->_next->_prev = temp->_prev;
      delete temp;
      return true;
   }

   void clear() 
   {
      while(_head!=_head->_prev)
      {
         pop_front();
      }
      _head->_prev = _head->_next = _head;

      _isSorted=false;
   }  // delete all nodes except for the dummy node


   void sort() const 
   { 
      if(_isSorted==true) return;
      for(iterator it1(_head->_prev);it1._node!=_head;it1--)
      {  
         for(iterator it2(begin());it2!=it1;it2++)
         {
            iterator temp = it2;
            ++temp;
            if(*it2>*temp)
            {  
               T change = *it2;
               *it2 = *temp;
               *temp = change;              
            }
         }
      }
      _isSorted = true;
   }

private:
   // [NOTE] DO NOT ADD or REMOVE any data member
   DListNode<T>*  _head;     // = dummy node if list is empty
   mutable bool   _isSorted; // (optionally) to indicate the array is sorted

   // [OPTIONAL TODO] helper functions; called by public member functions
};

#endif // DLIST_H

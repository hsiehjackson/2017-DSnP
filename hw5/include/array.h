/****************************************************************************
  FileName     [ array.h ]
  PackageName  [ util ]
  Synopsis     [ Define dynamic array package ]
  Author       [ Chung-Yang (Ric) Huang ]
  Copyright    [ Copyleft(c) 2005-present LaDs(III), GIEE, NTU, Taiwan ]
****************************************************************************/

#ifndef ARRAY_H
#define ARRAY_H

#include <cassert>
#include <algorithm>

using namespace std;

// NO need to implement class ArrayNode
//
template <class T>
class Array
{
public:
   // TODO: decide the initial value for _isSorted
   Array() : _data(0), _size(0), _capacity(0) {}
   ~Array() { delete []_data; }

   // DO NOT add any more data member or function for class iterator
   class iterator
   {
      friend class Array;

   public:
      iterator(T* n= 0): _node(n) {}
      iterator(const iterator& i): _node(i._node) {}
      ~iterator() {} // Should NOT delete _node

      // TODO: implement these overloaded operators
      const T& operator * () const { return (*_node); }
      T& operator * () { return (*_node); }
      iterator& operator ++ () { ++_node; return (*this); }
      iterator operator ++ (int) { iterator temp(*this); operator++(); return temp; }
      iterator& operator -- () { --_node; return (*this); }
      iterator operator -- (int) { iterator temp(*this); operator--(); return temp; }

      iterator operator + (int i) const { iterator temp(_node+i); return temp; }
      iterator& operator += (int i) { _node=_node+i; return (*this); }

      iterator& operator = (const iterator& i) { _node=i._node; return (*this); }

      bool operator != (const iterator& i) const { return i._node!=_node; }
      bool operator == (const iterator& i) const { return i._node==_node; }

   private:
      T*    _node;
   };

   // TODO: implement these functions
   iterator begin() const { if(_capacity==0)return 0; iterator temp(_data); return temp; }
   iterator end() const { if(_capacity==0)return 0; iterator temp(_data+_size); return temp; }
   bool empty() const 
   {  
      if(_size==0) return true;
      return false; 
   }
   size_t size() const { return _size; }

   T& operator [] (size_t i) { return *(_data+i); }
   const T& operator [] (size_t i) const { return *(_data+i); }

   void push_back(const T& x) 
   {
      _isSorted=false;
      if(_capacity==0)
      {
         _capacity = 1;
         _data = new T[_capacity];
      }
      else if(_size==_capacity)
      {
         _capacity = _capacity*2;
         
         T* olddata = _data;
         
         _data = new T[_capacity];

         size_t temp=0;

         while(temp!=_size)
         {
            _data[temp]=olddata[temp];
            temp++;
         }

         delete [] olddata;
      }
      _data[_size]=x;
      _size++;
   }
   void pop_front() 
   {
      _isSorted=false;
      if(empty()) return; 
      _data[0]=_data[_size-1];
      _size--;
   }
   void pop_back() 
   {
      _isSorted=false;
      if(empty()) return;
      _size--;
   }
   bool erase(iterator pos) 
   {
      _isSorted=false;
      if(empty())return false;
      size_t temp=0;
      while(_data+temp!=pos._node)
      {
         temp++;
      }
      _data[temp]=_data[_size-1]; 
      _size--;
      return true; 
   }
   bool erase(const T& x) 
   { 
      _isSorted=false;
      if(empty())return false;
      size_t temp=0;
      while(_data[temp]!=x)
      {
         if(temp==_size-1) return false;
         temp++;
      }

      _data[temp]=_data[_size-1]; 
      _size--;
      return true; 
   }

   void clear() 
   {    
    _isSorted=false;
    _size=0;
   }

   // [Optional TODO] Feel free to change, but DO NOT change ::sort()
   void sort() const { if(_isSorted==true) return; if (!empty()) ::sort(_data, _data+_size); _isSorted=true; }

   // Nice to have, but not required in this homework...
   // void reserve(size_t n) { ... }
   // void resize(size_t n) { ... }

private:
   // [NOTE] DO NOT ADD or REMOVE any data member
   T*            _data;
   size_t        _size;       // number of valid elements
   size_t        _capacity;   // max number of elements
   mutable bool  _isSorted;   // (optionally) to indicate the array is sorted

   // [OPTIONAL TODO] Helper functions; called by public member functions
};

#endif // ARRAY_H

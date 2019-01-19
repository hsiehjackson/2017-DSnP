/****************************************************************************
  FileName     [ myMinHeap.h ]
  PackageName  [ util ]
  Synopsis     [ Define MinHeap ADT ]
  Author       [ Chung-Yang (Ric) Huang ]
  Copyright    [ Copyleft(c) 2014-present LaDs(III), GIEE, NTU, Taiwan ]
****************************************************************************/

#ifndef MY_MIN_HEAP_H
#define MY_MIN_HEAP_H

#include <algorithm>
#include <vector>

template <class Data>
class MinHeap
{
public:
   MinHeap(size_t s = 0) 
   { 
    if (s != 0) _data.reserve(s); 
   }
   ~MinHeap() {}

   void clear() 
   { 
    _data.clear(); 
   }

   // For the following member functions,
   // We don't respond for the case vector "_data" is empty!
   const Data& operator [] (size_t i) const { return _data[i]; }   
   Data& operator [] (size_t i) { return _data[i]; }

   size_t size() const { return _data.size(); }

   // TODO
   const Data& min() const { return _data[0]; }
   void insert(const Data& d) 
   {
    size_t n = _data.size();
    _data.push_back(d);
    while(n>0)
    {
      size_t p = (n-1)/2;
      if(d<_data[p])
      {
        _data[n] = _data[p];
        n = p;
      }
      else
        break;
    }
    _data[n] = d;
   }


   void delMin() 
   { 
    delData(0);
   }


   void delData(size_t i) 
   {
    Data temp = _data.back();
    size_t x=i;
    size_t t=2*x+1;
    size_t n=_data.size();

    if(x==n) return;
    while(t<n)
    {
      if(t+1<n)
        if(_data[t+1]<_data[t])
          ++t;
      if(_data[t]<temp)
      {
      _data[x] = _data[t];
      x=t;
      t=2*x+1;
      }
      else
        break;
    } 

    int p=(x-1)/2;

    while(p>0)
    {
      if(temp < _data[p])
      {
        _data[x] = _data[p];
        x = p;
        p = (x-1)/2;
      }
      else
        break;
    }
      _data[x] = temp;  
      _data.pop_back();
   }

private:
   // DO NOT add or change data members
   vector<Data>   _data;
};

#endif // MY_MIN_HEAP_H

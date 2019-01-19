/****************************************************************************
  FileName     [ myHashSet.h ]
  PackageName  [ util ]
  Synopsis     [ Define HashSet ADT ]
  Author       [ Chung-Yang (Ric) Huang ]
  Copyright    [ Copyleft(c) 2014-present LaDs(III), GIEE, NTU, Taiwan ]
****************************************************************************/

#ifndef MY_HASH_SET_H
#define MY_HASH_SET_H

#include <vector>
#include <algorithm>

using namespace std;

//---------------------
// Define HashSet class
//---------------------
// To use HashSet ADT,
// the class "Data" should at least overload the "()" and "==" operators.
//
// "operator ()" is to generate the hash key (size_t)
// that will be % by _numBuckets to get the bucket number.
// ==> See "bucketNum()"
//
// "operator ==" is to check whether there has already been
// an equivalent "Data" object in the HashSet.
// Note that HashSet does not allow equivalent nodes to be inserted
//
template <class Data>
class HashSet
{
public:
   HashSet(size_t b = 0) : _numBuckets(0), _buckets(0) { if (b != 0) init(b); }
   ~HashSet() { reset(); }

   // TODO: implement the HashSet<Data>::iterator
   // o An iterator should be able to go through all the valid Data
   //   in the Hash
   // o Functions to be implemented:
   //   - constructor(s), destructor
   //   - operator '*': return the HashNode
   //   - ++/--iterator, iterator++/--
   //   - operators '=', '==', !="
   //
   class iterator
   {
      friend class HashSet<Data>;

   public:
      iterator(size_t n=0,vector<Data>* d=0, vector<Data>* b=0):num(n),dummy(d), _singlebucket(b){} 
      iterator(const iterator& i):num(i.num), dummy(i.dummy) ,_singlebucket(i._singlebucket){}
      ~iterator() {} 

     
      const Data& operator * () const { return (*_singlebucket)[num]; }
      iterator& operator ++ () 
      {
         if(num<(*_singlebucket).size()-1)
            num++;
         else
         {
            num=0;
            _singlebucket++;
            if(_singlebucket!=dummy)
            {
               while((*_singlebucket).size()==0){
                  _singlebucket++;
                  if(_singlebucket==dummy)
                     break;
               }
            }
         } 
         return (*this); 
      }

      iterator& operator -- ()
      {
         if(num>0)
            num--;
         else
         {
            _singlebucket--;
            while((*_singlebucket).size()==0){
               _singlebucket--;
            }
            num=(*_singlebucket).size()-1;

         }
         return (*this); 
      }

      iterator operator ++ (int) { iterator temp(*this); operator++(); return temp; }
      iterator operator -- (int) { iterator temp(*this); operator--(); return temp; }
      iterator& operator = (const iterator& i) 
      { 
         num = i.num;
         _singlebucket = i._singlebucket; 
         dummy = i.dummy;
         return (*this); 
      }
      bool operator == (const iterator& i) const {return _singlebucket==i._singlebucket && num==i.num && dummy==i.dummy;}
      bool operator != (const iterator& i) const {return !(_singlebucket==i._singlebucket && num==i.num && dummy==i.dummy);}
   private:
      size_t num;
      vector<Data>* dummy;
      vector<Data>* _singlebucket;
   };



   void init(size_t b) { _numBuckets = b; _buckets = new vector<Data>[b]; }
   void reset() {
      _numBuckets = 0;
      if (_buckets) { delete [] _buckets; _buckets = 0; }
   }
   void clear() {
      for (size_t i = 0; i < _numBuckets; ++i) _buckets[i].clear();
   }
   size_t numBuckets() const { return _numBuckets; }

   vector<Data>& operator [] (size_t i) { return _buckets[i]; }
   const vector<Data>& operator [](size_t i) const { return _buckets[i]; }

   // TODO: implement these functions
   //
   // Point to the first valid data
   iterator begin() const 
   { 
      size_t i = 0;
      while((*(_buckets+i)).empty())
      {
         i++;
         if(i==_numBuckets)
            return iterator(0,_buckets+_numBuckets,0);
      }
      return iterator(0,_buckets+_numBuckets,(_buckets+i)); 
   }
   // Pass the end
   iterator end() const 
   {
      size_t i = _numBuckets-1;
      while((*(_buckets+i)).empty())
      {
         i--;
         if(i==0)
            return iterator(0,_buckets+_numBuckets,0);
      }
      return iterator(0,_buckets+_numBuckets,_buckets+_numBuckets); 
   }
   // return true if no valid data
   bool empty() const { return begin()==end(); }
   // number of valid data
   size_t size() const 
   {
      size_t s = 0;
      for(size_t i=0; i<_numBuckets; i++)
      {
         s = s + (*(_buckets+i)).size();
      }
      return s; 
   }
   // check if d is in the hash...
   // if yes, return true;
   // else return false;
   bool check(const Data& d) const 
   { 
      size_t num = bucketNum(d);
      for(size_t i=0; i<_buckets[num].size();i++)
      {
         if(_buckets[num][i]==d)
            return true;
      }
      return false;
   }

   // query if d is in the hash...
   // if yes, replace d with the data in the hash and return true;
   // else return false;
   bool query(Data& d) const 
   { 
      size_t num = bucketNum(d);
      size_t i;
      for(i=0; i<_buckets[num].size();i++)
      {
         if(_buckets[num][i]==d) break;
      }   
      if(i != _buckets[num].size()) 
      {
         d = _buckets[num][i];
         return true;
      }
      else
         return false;
   }

   // update the entry in hash that is equal to d (i.e. == return true)
   // if found, update that entry with d and return true;
   // else insert d into hash as a new entry and return false;
   bool update(const Data& d) 
   {
      size_t num = bucketNum(d);
      size_t i;
      for(i=0; i<_buckets[num].size();i++)
      {
         if(_buckets[num][i]==d) break;
      }   

      if(i != _buckets[num].size())
      {
         _buckets[num][i] = d;
         return true;
      }
      else
      {
         _buckets[num].push_back(d);
         return false;
      }
   }

   // return true if inserted successfully (i.e. d is not in the hash)
   // return false is d is already in the hash ==> will not insert
   bool insert(const Data& d) 
   {
      if(check(d))
         return false;
      else
      {
         (*(_buckets+bucketNum(d))).push_back(d); 
         return true; 
      }
   }

   // return true if removed successfully (i.e. d is in the hash)
   // return fasle otherwise (i.e. nothing is removed)
   bool remove(const Data& d) 
   {

      size_t num = bucketNum(d);
      size_t i;
      for(i=0; i<_buckets[num].size();i++)
      {
         if(_buckets[num][i]==d) break;
      }   
      if(i != _buckets[num].size()) 
      {
         _buckets[num][i] = _buckets[num].back();
         _buckets[num].pop_back();
         return true;
      } 
      else
         return false;
   }

private:
   // Do not add any extra data member
   size_t            _numBuckets;
   vector<Data>*     _buckets;

   size_t bucketNum(const Data& d) const {
      return (d() % _numBuckets); }
};

#endif // MY_HASH_SET_H

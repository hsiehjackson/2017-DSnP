/****************************************************************************
  FileName     [ myHashMap.h ]
  PackageName  [ util ]
  Synopsis     [ Define HashMap and Cache ADT ]
  Author       [ Chung-Yang (Ric) Huang ]
  Copyright    [ Copyleft(c) 2009-present LaDs(III), GIEE, NTU, Taiwan ]
****************************************************************************/

#ifndef MY_HASH_MAP_H
#define MY_HASH_MAP_H

#include <vector>

using namespace std;

// TODO: (Optionally) Implement your own HashMap and Cache classes.

//-----------------------
// Define HashMap classes
//-----------------------
// To use HashMap ADT, you should define your own HashKey class.
// It should at least overload the "()" and "==" operators.
//
// class HashKey
// {
// public:
//    HashKey() {}
// 
//    size_t operator() () const { return 0; }
// 
//    bool operator == (const HashKey& k) const { return true; }
// 
// private:
// };
//
template <class HashKey, class HashData>
class HashMap
{
typedef pair<HashKey, HashData> HashNode;

public:
   HashMap(size_t b=0) : _numBuckets(0), _buckets(0) { if (b != 0) init(b); }
   ~HashMap() { reset(); }

   // [Optional] TODO: implement the HashMap<HashKey, HashData>::iterator
   // o An iterator should be able to go through all the valid HashNodes
   //   in the HashMap
   // o Functions to be implemented:
   //   - constructor(s), destructor
   //   - operator '*': return the HashNode
   //   - ++/--iterator, iterator++/--
   //   - operators '=', '==', !="
   //
   class iterator
   {
      friend class HashMap<HashKey, HashData>;

      public:

      iterator(size_t n=0,vector<HashNode>* d=0, vector<HashNode>* b=0):num(n),dummy(d), _singlebucket(b){} 
      iterator(const iterator& i):num(i.num), dummy(i.dummy) ,_singlebucket(i._singlebucket){}
      ~iterator() {} 

     
      const HashData & operator * () const { return (*_singlebucket)[num].second; }
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
      vector<HashNode>* dummy;
      vector<HashNode>* _singlebucket;
   };

   

   void init(size_t b) {
      reset(); _numBuckets = b; _buckets = new vector<HashNode>[b]; }
   void reset() {
      _numBuckets = 0;
      if (_buckets) { delete [] _buckets; _buckets = 0; }
   }
   void clear() {
      for (size_t i = 0; i < _numBuckets; ++i) _buckets[i].clear();
   }
   size_t numBuckets() const { return _numBuckets; }

   vector<HashNode>& operator [] (size_t i) { return _buckets[i]; }
   const vector<HashNode>& operator [](size_t i) const { return _buckets[i]; }

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
            return iterator(0,_buckets+_numBuckets,_buckets+_numBuckets);
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
         if(i==0&&(*(_buckets+i)).empty())
            return iterator(0,_buckets+_numBuckets,_buckets+_numBuckets);
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

   // check if k is in the hash...
   // if yes, return true;
   // else return false;
   bool check(const HashKey& k) const 
   {
      size_t num = bucketNum(k);
      for(size_t i=0; i<_buckets[num].size();i++)
      {
         if(_buckets[num][i].first==k)
            return true;
      }
      return false; 
   }

   // query if k is in the hash...
   // if yes, replace d with the data in the hash and return true;
   // else return false;
   bool query(const HashKey& k, HashData& d) const 
   {
      size_t num = bucketNum(k);
      for(size_t i=0; i<_buckets[num].size();i++)
      {
         if(_buckets[num][i].first==k)
         {
            d = _buckets[num][i].second;
            return true;
         }
      }
      return false;   
   }

   // update the entry in hash that is equal to k (i.e. == return true)
   // if found, update that entry with d and return true;
   // else insert d into hash as a new entry and return false;
   bool update(const HashKey& k, HashData& d) 
   { 
      size_t num = bucketNum(k);
      size_t i;
      for(i=0; i<_buckets[num].size();i++)
      {
         if(_buckets[num][i].first==k)
         {
            _buckets[num][i].second = d;
            return true;
         }
      }   
         _buckets[num].push_back(make_pair(k,d));
         return false;
   }

   // return true if inserted d successfully (i.e. k is not in the hash)
   // return false is k is already in the hash ==> will not insert
   bool insert(const HashKey& k, const HashData& d)
   {
      if(check(k))
         return false;
      else
         _buckets[bucketNum(k)].push_back(make_pair(k,d)); 
         return true;  
   }

   // return true if removed successfully (i.e. k is in the hash)
   // return fasle otherwise (i.e. nothing is removed)
   bool remove(const HashKey& k) 
   { 
      size_t num = bucketNum(k);
      size_t i;
      for(i=0; i<_buckets[num].size();i++)
      {
         if(_buckets[num][i].first==k)
         {
            _buckets[num][i] = _buckets[num].back();
            _buckets[num].pop_back();
            return true;
         }
      }   
      return false;
   }

private:
   // Do not add any extra data member
   size_t                   _numBuckets;
   vector<HashNode>*        _buckets;

   size_t bucketNum(const HashKey& k) const {
      return (k() % _numBuckets); }

};


//---------------------
// Define Cache classes
//---------------------
// To use Cache ADT, you should define your own HashKey class.
// It should at least overload the "()" and "==" operators.
//
// class CacheKey
// {
// public:
//    CacheKey() {}
//    
//    size_t operator() () const { return 0; }
//   
//    bool operator == (const CacheKey&) const { return true; }
//       
// private:
// }; 
// 
template <class CacheKey, class CacheData>
class Cache
{
typedef pair<CacheKey, CacheData> CacheNode;

public:
   Cache() : _size(0), _cache(0) {}
   Cache(size_t s) : _size(0), _cache(0) { init(s); }
   ~Cache() { reset(); }

   // NO NEED to implement Cache::iterator class

   // TODO: implement these functions
   //
   // Initialize _cache with size s
   void init(size_t s) { reset(); _size = s; _cache = new CacheNode[s]; }
   void reset() {  _size = 0; if (_cache) { delete [] _cache; _cache = 0; } }

   size_t size() const { return _size; }

   CacheNode& operator [] (size_t i) { return _cache[i]; }
   const CacheNode& operator [](size_t i) const { return _cache[i]; }

   // return false if cache miss
   bool read(const CacheKey& k, CacheData& d) const {
      size_t i = k() % _size;
      if (k == _cache[i].first) {
         d = _cache[i].second;
         return true;
      }
      return false;
   }
   // If k is already in the Cache, overwrite the CacheData
   void write(const CacheKey& k, const CacheData& d) {
      size_t i = k() % _size;
      _cache[i].first = k;
      _cache[i].second = d;
   }

private:
   // Do not add any extra data member
   size_t         _size;
   CacheNode*     _cache;
};


#endif // MY_HASH_H

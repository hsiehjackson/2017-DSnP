/****************************************************************************
  FileName     [ p2Table.h]
  PackageName  [ p2 ]
  Synopsis     [ Header file for class Row and Table ]
  Author       [ Chung-Yang (Ric) Huang ]
  Copyright    [ Copyleft(c) 2016-present LaDs(III), GIEE, NTU, Taiwan ]
****************************************************************************/
#ifndef P2_TABLE_H
#define P2_TABLE_H

#include <vector>
#include <string>
#include <iostream>
#include <iomanip>
#include <climits>

using namespace std;

class Row
{
public:
   // TODO: define constructor and member functions on your own
   Row(size_t i){
        _data = new int [i];
   }
   // Basic access functions
   const int operator[] (size_t i) const { return _data[i]; }
   int& operator[] (size_t i) { return _data[i]; }

private:
   int    *_data; // DO NOT change this definition. Use it to store data.
};

class Table
{
public:
   // TODO: define constructor and member functions on your own
   Table(){
          _nCols = 0;
   }
   void add(Row);
   void print();
   bool read(const string&);
   float AVE(size_t);
   int SUM(size_t);
   int MAX(size_t);
   int MIN(size_t);
   int DIST(size_t);
   void ADD();
   bool empty(size_t);
   void EXIT();
   // Basic access functions
   size_t nCols() const { return _nCols; }
   const Row& operator[] (size_t i) const { return _rows[i]; }
   Row& operator[] (size_t i) { return _rows[i]; }

private:
   size_t       _nCols; // You should record the number of columns.
   vector<Row>  _rows;  // DO NOT change this definition. Use it to store rows.
};

#endif // P2_TABLE_H

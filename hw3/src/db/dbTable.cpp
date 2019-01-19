/****************************************************************************
  FileName     [ dbTable.cpp ]
  PackageName  [ db ]
  Synopsis     [ Define database Table member functions ]
  Author       [ Chung-Yang (Ric) Huang ]
  Copyright    [ Copyleft(c) 2015-present LaDs(III), GIEE, NTU, Taiwan ]
****************************************************************************/

#include <iomanip>
#include <sstream>
#include <fstream>
#include <string>
#include <cctype>
#include <cassert>
#include <set>
#include <algorithm>
#include "dbTable.h"
#include "util.h"

using namespace std;

/*****************************************/
/*          Global Functions             */
/*****************************************/
ostream& operator << (ostream& os, const DBRow& r)
{
  
	for(size_t i=0;i<r.size();i++)
	{
		if(i>0)
		{
		 os<<" ";
		}
		if(r[i]==INT_MAX)
		{
		 os<<".";
		}	
		else
		{
		 os<<r[i];
		}
	}
   // TODO: to print out a row.
   // - Data are seperated by a space. No trailing space at the end.
   // - Null cells are printed as '.'
   return os;
}

ostream& operator << (ostream& os, const DBTable& t)
{
  
	for(size_t i=0;i<t.nRows();i++)
	{
	 for(size_t j=0;j<t.nCols();j++)
	 {
        if(t.getData(i,j)==INT_MAX)
        {
         os<<right<<setw(6)<<".";
        }
        else
        {
         os<<right<<setw(6)<<t.getData(i,j); 
        }
   }
        os<<endl;
  }
   // TODO: to print out a table
   // - Data are seperated by setw(6) and aligned right.
   // - Null cells are printed as '.'
  return os;
}

ifstream& operator >> (ifstream& ifs, DBTable& t)
{
 	string line;
  while(getline(ifs,line))
  {
   string data;
   DBRow temp;
   istringstream templine(line); 
   while (getline(templine,data,','))
   {    
    if(data.empty()||data=="\r")
    {
    temp.addData(INT_MAX); 
    }
    else
    {
    temp.addData(atoi(data.c_str()));  
    }    
   }
  
    if(line.back()==',')
    {
    temp.addData(INT_MAX); 
    }
   t.addRow(temp);
  }
   // TODO: to read in data from csv file and store them in a table
   // - You can assume the input file is with correct csv file format
   // - NO NEED to handle error file format
   ifs.ifstream::clear();
   return ifs;
}

/*****************************************/
/*   Member Functions for class DBRow    */
/*****************************************/
void
DBRow::removeCell(size_t c)
{
  _data.erase(_data.begin()+c);
   // TODO
}

/*****************************************/
/*   Member Functions for struct DBSort  */
/*****************************************/
bool
DBSort::operator() (const DBRow& r1, const DBRow& r2) const
{
  size_t i=0;
  while (i<_sortOrder.size())
  {
    if(r1[_sortOrder[i]]==r2[_sortOrder[i]])
    {
      i++;
    }
    else
    {
      return r1[_sortOrder[i]]<r2[_sortOrder[i]];
    }
  }
  return false;
   // TODO: called as a functional object that compares the data in r1 and r2
   //       based on the order defined in _sortOrder
}
/*****************************************/
/*   Member Functions for class DBTable  */
/*****************************************/
void
DBTable::reset()
{
   vector<DBRow> tmp; 
   tmp.swap(_table); 
   // TODO
}

void
DBTable::addCol(const vector<int>& d)
{
  for(size_t i=0;i<nRows();i++)
  {
    _table[i].addData(d[i]);
  }
   // TODO: add a column to the right of the table. Data are in 'd'.
}

void
DBTable::delRow(int c)
{
  _table.erase(_table.begin()+c);
   // TODO: delete row #c. Note #0 is the first row.
}

void
DBTable::delCol(int c)
{
   // delete col #c. Note #0 is the first row.
   for (size_t i = 0, n = _table.size(); i < n; ++i)
      _table[i].removeCell(c);
}

// For the following getXXX() functions...  (except for getCount())
// - Ignore null cells
// - If all the cells in column #c are null, return NAN
// - Return "float" because NAN is a float.
float
DBTable::getMax(size_t c) const
{
  float MAX=INT_MIN;
  for(size_t i=0;i<_table.size();i++){
    if(_table[i][c]!=INT_MAX&&_table[i][c]>MAX){
      MAX=_table[i][c];
      }
  }
  if(MAX==INT_MIN)return NAN;
  return MAX;
   // TODO: get the max data in column #c
}

float
DBTable::getMin(size_t c) const
{
  float MIN=INT_MAX-1;
  for(size_t i=0;i<_table.size();i++){
    if(_table[i][c]!=INT_MAX&&_table[i][c]<MIN){
      MIN=_table[i][c];
      }
  }
  if(MIN==INT_MAX-1)return NAN;
  return MIN;
   // TODO: get the min data in column #c
}

float 
DBTable::getSum(size_t c) const
{
  float sum=0;
  size_t null=0;
  for(size_t i=0;i<_table.size();i++){
    if(_table[i][c]!=INT_MAX){
    sum=sum+_table[i][c];
      }
    else{
    null++;
    }
  }

  if(null==this->nRows())return NAN;
  return sum;
   // TODO: compute the sum of data in column #c
}

int
DBTable::getCount(size_t c) const
{
  int DIST=0;
  vector<int> temp;
  temp.push_back(INT_MAX);
  for(size_t i=0;i<_table.size();i++)
  {
     size_t num=0;
     for(size_t j=0;j<temp.size();j++)
     {
       if(_table[i][c]!=temp[j])
       {
       num++;
       }
       if(num==temp.size())
       {
       temp.push_back(_table[i][c]);
       DIST++;
       }
     }  
  }
  return DIST;
   // TODO: compute the number of distinct data in column #c
   // - Ignore null cells
}

float
DBTable::getAve(size_t c) const
{
  float sum=0;
  float num=0;
  size_t null=0;
  for(size_t i=0;i<_table.size();i++){
    if(_table[i][c]!=INT_MAX){
    sum=sum+_table[i][c];
      num++;
      }
    else null++;
  }
  if(null==this->nRows())return NAN;
  return sum/num;
   // TODO: compute the average of data in column #c
}

void
DBTable::sort(const struct DBSort& s)
{
  ::sort(_table.begin(),_table.end(),s);
   // TODO: sort the data according to the order of columns in 's'
}

void
DBTable::printCol(size_t c) const
{
  for(size_t i=0;i<nRows();i++)
  {
    if(i>0)
    {
      cout<<" ";
    }
    if(_table[i][c]==INT_MAX)
    {
      cout<<".";
    }
    else
    {
      cout<<_table[i][c];
    }

  }
   // TODO: to print out a column.
   // - Data are seperated by a space. No trailing space at the end.
   // - Null cells are printed as '.'
}

void
DBTable::printSummary() const
{
   size_t nr = nRows(), nc = nCols(), nv = 0;
   for (size_t i = 0; i < nr; ++i)
      for (size_t j = 0; j < nc; ++j)
         if (_table[i][j] != INT_MAX) ++nv;
   cout << "(#rows, #cols, #data) = (" << nr << ", " << nc << ", "
        << nv << ")" << endl;
}


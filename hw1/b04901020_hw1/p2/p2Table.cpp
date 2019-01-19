/****************************************************************************
  FileName     [ p2Table.cpp ]
  PackageName  [ p2 ]
  Synopsis     [ Define member functions of class Row and Table ]
  Author       [ Chung-Yang (Ric) Huang ]
  Copyright    [ Copyleft(c) 2016-present LaDs(III), GIEE, NTU, Taiwan ]
****************************************************************************/
#include "p2Table.h"
#include  <fstream> 
#include  <sstream>



using namespace std;

// Implement member functions of class Row and Table here
bool
Table::read(const string& csvFile)
{

  fstream file;
  file.open(csvFile.c_str());  
  if(file.is_open()){	
  	string line;
  	getline(file,line); 
  	istringstream templine(line); 
  	string data;
	while (getline(templine,data,',')){
	_nCols++;
	}
	if(data.empty())_nCols++;
	
  	file.close();
 
  	fstream file;
  	file.open(csvFile.c_str());
  	while (getline(file,line))  
	{
	  istringstream templine(line); 
	  string data;
	  Row row(_nCols);
	  size_t num = 0;
	  
	  while(num<_nCols)
	  {
        if(num == _nCols-1){
		getline(templine,data,'\r');	
		}
		else{
        getline(templine,data,',');
		}
		if(data.empty()){
		row[num]= INT_MAX;
		}
		else{
		row[num]=(atoi(data.c_str()));	
		}
		num++;		
	  }
	  this->add(row);
	}
  	file.close();
  	return true;
  }
  else {return false;}
   // TODO
}
void Table::add(Row i){
	 _rows.push_back(i);   
}
void Table::print(){
	for(size_t i=0;i<_rows.size();i++){
      for(size_t j=0;j<_nCols;j++){
        if(_rows[i][j]==INT_MAX){
         cout<<right<<setw(4)<<".";
        }
        else{
         cout<<right<<setw(4)<<_rows[i][j];       
        }
      }
      cout<<endl;
    }
}

bool Table::empty(size_t col){
	size_t num=0;
	for(size_t i=0;i<_rows.size();i++){
		if(_rows[i][col]==INT_MAX){
		 num++;
		}
	}
	if(num==_rows.size()){
	   return true;
	}
	return false;
}


float Table::AVE(size_t col){
	float sum=0;
	float num=0;
	for(size_t i=0;i<_rows.size();i++){
		if(_rows[i][col]!=INT_MAX){
		sum=sum+_rows[i][col];
	    num++;
	    }
	}
	return sum/num;
}
int Table::SUM(size_t col){
	long long sum=0;
	for(size_t i=0;i<_rows.size();i++){
		if(_rows[i][col]!=INT_MAX){
		sum=sum+_rows[i][col];
	    }
	}
	return sum;
}
int Table::MAX(size_t col){
	int MAX=INT_MIN;
	for(size_t i=0;i<_rows.size();i++){
		if(_rows[i][col]!=INT_MAX&&_rows[i][col]>MAX){
			MAX=_rows[i][col];
	    }
	}return MAX;
}
int Table::MIN(size_t col){
	int MIN=INT_MAX-1;
	for(size_t i=0;i<_rows.size();i++){
		if(_rows[i][col]!=INT_MAX&&_rows[i][col]<MIN){
			MIN=_rows[i][col];
	    }
	}return MIN;
}

int Table::DIST(size_t col){
	int DIST=0;
	vector<int> temp;
	temp.push_back(INT_MAX);
	for(size_t i=0;i<_rows.size();i++){
	   size_t num=0;
	   for(size_t j=0;j<temp.size();j++){
	     if(_rows[i][col]!=temp[j]){
 			num++;}
 		 if(num==temp.size()){
			temp.push_back(_rows[i][col]);
	        DIST++;}
	      }  
	   }
	return DIST;
}
void Table::ADD(){
	 Row row(_nCols);
	 string temp;
	 for(size_t i=0;i<_nCols;i++){
       		cin>>temp;
       		if(temp=="."){
       		 row[i]=INT_MAX;
       		}
       		else{
       		 row[i]=(atoi(temp.c_str()));
       		}
        }
	 this->add(row);
}

void Table::EXIT(){
}
/****************************************************************************
  FileName     [ p2Main.cpp ]
  PackageName  [ p2 ]
  Synopsis     [ Define main() function ]
  Author       [ Chung-Yang (Ric) Huang ]
  Copyright    [ Copyleft(c) 2016-present LaDs(III), GIEE, NTU, Taiwan ]
****************************************************************************/
#include <iostream>
#include <string>
#include "p2Table.h"

using namespace std;

int main()
{
   Table table;

   // Read in the csv file. Do NOT change this part of code.
   string csvFile;
   cout << "Please enter the file name: ";
   cin >> csvFile;
   if (table.read(csvFile)){
      cout << "File \"" << csvFile << "\" was read in successfully." << endl;
   }
   else {
      cerr << "Failed to read in file \"" << csvFile << "\"!" << endl;
      exit(-1); // csvFile does not exist.
   }
   
   string command;
   size_t num;
  // TODO read and execute commands  
     while(cin>>command){   
     if(command=="EXIT"){break;}
     else if(command=="PRINT"){table.print();}
     else if(command=="ADD"){table.ADD();}
     else if(command=="AVE"||command=="MAX"||command=="MIN"||command=="SUM"||command=="DIST"){
     cin>>num;
     if(table.empty(num)){cout<<"Error: This is a NULL column!!"<<endl;}	
     else if(command=="AVE"){cout<<"The average of data in column #"<<num<<" is "<<fixed<<setprecision(1)<<table.AVE(num)<<"."<<endl;}
     else if(command=="MAX"){cout<<"The maximum of data in column #"<<num<<" is "<<table.MAX(num)<<"."<<endl;}
     else if(command=="MIN"){cout<<"The minimum of data in column #"<<num<<" is "<<table.MIN(num)<<"."<<endl;}
     else if(command=="SUM"){cout<<"The summation of data in column #"<<num<<" is "<<table.SUM(num)<<"."<<endl;}
     else if(command=="DIST"){cout<<"The distinct count of data in column #"<<num<<" is "<<table.DIST(num)<<"."<<endl;}
     }
     }
   return 0;
}

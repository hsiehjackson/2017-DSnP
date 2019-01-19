/****************************************************************************
  FileName     [ cirMgr.cpp ]
  PackageName  [ cir ]
  Synopsis     [ Define cir manager functions ]
  Author       [ Chung-Yang (Ric) Huang ]
  Copyright    [ Copyleft(c) 2008-present LaDs(III), GIEE, NTU, Taiwan ]
****************************************************************************/

#include <iostream>
#include <iomanip>
#include <cstdio>
#include <ctype.h>
#include <cassert>
#include <cstring>
#include "cirMgr.h"
#include "cirGate.h"
#include "util.h"

using namespace std;

// TODO: Implement memeber functions for class CirMgr

/*******************************/
/*   Global variable and enum  */
/*******************************/
CirMgr* cirMgr = 0;

bool lexOptions
(const string&, vector<string>&, size_t); 


enum CirParseError {
   EXTRA_SPACE,
   MISSING_SPACE,
   ILLEGAL_WSPACE,
   ILLEGAL_NUM,
   ILLEGAL_IDENTIFIER,
   ILLEGAL_SYMBOL_TYPE,
   ILLEGAL_SYMBOL_NAME,
   MISSING_NUM,
   MISSING_IDENTIFIER,
   MISSING_NEWLINE,
   MISSING_DEF,
   CANNOT_INVERTED,
   MAX_LIT_ID,
   REDEF_GATE,
   REDEF_SYMBOLIC_NAME,
   REDEF_CONST,
   NUM_TOO_SMALL,
   NUM_TOO_BIG,

   DUMMY_END
};

/**************************************/
/*   Static varaibles and functions   */
/**************************************/
static unsigned lineNo = 1;  // in printint, lineNo needs to ++
static unsigned colNo  = 1;  // in printing, colNo needs to ++
static char buf[1024];
static string errMsg;
static int errInt;
static CirGate *errGate;

static bool
parseError(CirParseError err)
{
   switch (err) {
      case EXTRA_SPACE:
         cerr << "[ERROR] Line " << lineNo+1 << ", Col " << colNo+1
              << ": Extra space character is detected!!" << endl;
         break;
      case MISSING_SPACE:
         cerr << "[ERROR] Line " << lineNo+1 << ", Col " << colNo+1
              << ": Missing space character!!" << endl;
         break;
      case ILLEGAL_WSPACE: // for non-space white space character
         cerr << "[ERROR] Line " << lineNo+1 << ", Col " << colNo+1
              << ": Illegal white space char(" << errInt
              << ") is detected!!" << endl;
         break;
      case ILLEGAL_NUM:
         cerr << "[ERROR] Line " << lineNo+1 << ": Illegal "
              << errMsg << "!!" << endl;
         break;
      case ILLEGAL_IDENTIFIER:
         cerr << "[ERROR] Line " << lineNo+1 << ": Illegal identifier \""
              << errMsg << "\"!!" << endl;
         break;
      case ILLEGAL_SYMBOL_TYPE:
         cerr << "[ERROR] Line " << lineNo+1 << ", Col " << colNo+1
              << ": Illegal symbol type (" << errMsg << ")!!" << endl;
         break;
      case ILLEGAL_SYMBOL_NAME:
         cerr << "[ERROR] Line " << lineNo+1 << ", Col " << colNo+1
              << ": Symbolic name contains un-printable char(" << errInt
              << ")!!" << endl;
         break;
      case MISSING_NUM:
         cerr << "[ERROR] Line " << lineNo+1 << ", Col " << colNo+1
              << ": Missing " << errMsg << "!!" << endl;
         break;
      case MISSING_IDENTIFIER:
         cerr << "[ERROR] Line " << lineNo+1 << ": Missing \""
              << errMsg << "\"!!" << endl;
         break;
      case MISSING_NEWLINE:
         cerr << "[ERROR] Line " << lineNo+1 << ", Col " << colNo+1
              << ": A new line is expected here!!" << endl;
         break;
      case MISSING_DEF:
         cerr << "[ERROR] Line " << lineNo+1 << ": Missing " << errMsg
              << " definition!!" << endl;
         break;
      case CANNOT_INVERTED:
         cerr << "[ERROR] Line " << lineNo+1 << ", Col " << colNo+1
              << ": " << errMsg << " " << errInt << "(" << errInt/2
              << ") cannot be inverted!!" << endl;
         break;
      case MAX_LIT_ID:
         cerr << "[ERROR] Line " << lineNo+1 << ", Col " << colNo+1
              << ": Literal \"" << errInt << "\" exceeds maximum valid ID!!"
              << endl;
         break;
      case REDEF_GATE:
         cerr << "[ERROR] Line " << lineNo+1 << ": Literal \"" << errInt
              << "\" is redefined, previously defined as "
              << errGate->getTypeStr() << " in line " << errGate->getLineNo()
              << "!!" << endl;
         break;
      case REDEF_SYMBOLIC_NAME:
         cerr << "[ERROR] Line " << lineNo+1 << ": Symbolic name for \""
              << errMsg << errInt << "\" is redefined!!" << endl;
         break;
      case REDEF_CONST:
         cerr << "[ERROR] Line " << lineNo+1 << ", Col " << colNo+1
              << ": Cannot redefine constant (" << errInt << ")!!" << endl;
         break;
      case NUM_TOO_SMALL:
         cerr << "[ERROR] Line " << lineNo+1 << ": " << errMsg
              << " is too small (" << errInt << ")!!" << endl;
         break;
      case NUM_TOO_BIG:
         cerr << "[ERROR] Line " << lineNo+1 << ": " << errMsg
              << " is too big (" << errInt << ")!!" << endl;
         break;
      default: break;
   }
   return false;
}

/**************************************************************/
/*   class CirMgr member functions for circuit construction   */
/**************************************************************/
bool
CirMgr::readCircuit(const string& fileName)
{
  ifstream file;
  file.open(fileName);
  if(file.is_open())
  {
    // construct 
    string line;
    int tempI=0;
    int tempM=0;
    unsigned linenum=1;




    while(getline(file,line))
    {
      string option = line;
      string optionback = line;



      if(option[0]=='i'||option[0]=='o')
      {
        while(option[0]!=' ')
        {
          option.erase(option.begin());
        }
      }


      option.erase(option.begin());
      vector<string> options;
      lexOptions(line,options,0);

      if(lineNo==1&&options[0]!="aag")
      {

        errMsg = options[0];
        parseError(MISSING_IDENTIFIER);
        return false;
      }


      if(options[0][0]=='i'||options[0][0]=='o')
      {
        if(options[0][0]=='i')
        {
          options[0].erase(options[0].begin());  
          int num; 
          myStr2Int(options[0],num);
          static_cast <CirPiGate*>(_totallist[_pilist[num]])->_name = option;
        }
        else if(options[0][0]=='o')
        {
          options[0].erase(options[0].begin());
          int num; 
          myStr2Int(options[0],num);
          static_cast <CirPoGate*>(_totallist[_polist[num]])->_name = option;
        }
      } 

      else if(options.size()==6)
      {
        myStr2Int(options[1],M);
        myStr2Int(options[2],I);
        myStr2Int(options[3],L);
        myStr2Int(options[4],O);
        myStr2Int(options[5],A);
        tempM=M;
        vector<CirGate*> v(M+O+1);
        _totallist = v;
        _totallist[0] = new CirConstGate(0,0);
      }

      else if(options.size()==1)
      {
       if(options[0]=="c") break;
       int num;
       myStr2Int(options[0],num);
       num = unsigned(num);
       if(tempI<I)
       {
        _totallist[num/2] = new CirPiGate(num/2,linenum);
        _pilist.push_back(num/2);
        tempI++;
       }
       else
       {
        tempM++;
        _totallist[tempM] = new CirPoGate(tempM,linenum);
        _totallist[tempM]->_faninID.push_back(num);
        _polist.push_back(tempM);
       }
      }
      else if(options.size()==3)
      {
        int  num0,num1,num2;
        myStr2Int(options[0],num0);
        myStr2Int(options[1],num1);
        myStr2Int(options[2],num2);
        num0 = unsigned(num0);
        num1 = unsigned(num1);
        num2 = unsigned(num2);
        _totallist[num0/2] = new CirAigGate(num0/2,linenum);
        _totallist[num0/2]->_faninID.push_back(num1);
        _totallist[num0/2]->_faninID.push_back(num2);
      }
      else
      { 
        return false;
      }
      linenum++;
      lineNo++;
    }


    //connect 
    for(int i=0;i<M+O+1;i++)
    {
      if(_totallist[i]!=NULL)
      {
        int temp=0;
        if(_totallist[i]->_faninID.size()!=0)
        {
          while(temp<2)
          {

            CirGate* gate = _totallist[(_totallist[i]->_faninID[temp])/2];
            if(gate!=NULL)
            {
              _totallist[i]->_fanin.push_back(CirGateV(gate,(_totallist[i]->_faninID[temp])%2));
              gate->_fanout.push_back(CirGateV(_totallist[i],(_totallist[i]->_faninID[temp])%2));
            }
            else
            {
              gate = new CirUndefGate((_totallist[i]->_faninID[temp])/2);
             _totallist[(_totallist[i]->_faninID[temp])/2]=gate;
              _totallist[i]->_fanin.push_back(CirGateV(gate,(_totallist[i]->_faninID[temp])%2));
              gate->_fanout.push_back(CirGateV(_totallist[i],(_totallist[i]->_faninID[temp])%2));
            }
            if(_totallist[i]->_faninID.size()==1) break;
            temp++;
          }
        }
      }
    }
  
    CirGate::setGlobalRef();
    for(unsigned i=0;i<_polist.size();i++)
    {
      _totallist[_polist[i]]->dfsTraversal(dfslist);
    }
    unsigned temp=0;
    while(temp<dfslist.size())
    {
      if(dfslist[temp]->getTypeStr()=="UNDEF")
        dfslist.erase(dfslist.begin()+temp); 
      else
        temp++;
    }

    file.close();
    return true;
  }
  else
  {
    cerr<<"Cannot open design \""<<fileName<<"\"!!"<<endl;
    return false;
  }
}

/**********************************************************/
/*   class CirMgr member functions for circuit printing   */
/**********************************************************/
/*********************
Circuit Statistics
==================
  PI          20
  PO          12
  AIG        130
------------------
  Total      162
*********************/
void
CirMgr::printSummary() const
{
cout<<endl;
cout<<"Circuit Statistics"<<endl;
cout<<"=================="<<endl;
cout<<"  PI"<<setw(12)<<right<<I<<endl;
cout<<"  PO"<<setw(12)<<right<<O<<endl;
cout<<"  AIG"<<setw(11)<<right<<A<<endl;
cout<<"------------------"<<endl;
cout<<"  Total"<<setw(9)<<right<<I+O+A<<endl;
}

void
CirMgr::printNetlist() const
{
  cout<<endl;

  for(unsigned i=0;i<dfslist.size();i++)
  {
    stringstream ss1,ss2;

    ss1<<setw(4)<<left<<dfslist[i]->getTypeStr();
    ss2<<dfslist[i]->getID();

    cout<<"["<<i<<"] "<<ss1.str()<<ss2.str();

    if(dfslist[i]->getTypeStr()=="PI")
    {
      string name = static_cast<CirPiGate*>(dfslist[i])->_name;
      if(name.size()!=0)
        cout<<" "<<"("<<name<<")";
    }
    else if(dfslist[i]->getTypeStr()=="PO")
    {
      stringstream ss3;

      if(dfslist[i]->_fanin[0].isInv())
        ss3<<"!"<<dfslist[i]->_fanin[0].gate()->getID();
      else
        ss3<<dfslist[i]->_fanin[0].gate()->getID();

      if(dfslist[i]->_fanin[0].gate()->getTypeStr()=="UNDEF")
        cout<<" "<<"*"<<ss3.str();
      else
        cout<<" "<<ss3.str();

      string name = static_cast<CirPoGate*>(dfslist[i])->_name;
      if(name.size()!=0)
        cout<<" "<<"("<<name<<")";

    }
    else if(dfslist[i]->getTypeStr()=="AIG")  
    {
      stringstream ss3,ss4;

      if(dfslist[i]->_fanin[0].isInv())
        ss3<<"!"<<dfslist[i]->_fanin[0].gate()->getID();
      else
        ss3<<dfslist[i]->_fanin[0].gate()->getID();

      if(dfslist[i]->_fanin[1].isInv())
        ss4<<"!"<<dfslist[i]->_fanin[1].gate()->getID();
      else
        ss4<<dfslist[i]->_fanin[1].gate()->getID();

      if(dfslist[i]->_fanin[0].gate()->getTypeStr()=="UNDEF")
        cout<<" "<<"*"<<ss3.str();
      else
        cout<<" "<<ss3.str();

      if(dfslist[i]->_fanin[1].gate()->getTypeStr()=="UNDEF")
        cout<<" "<<"*"<<ss4.str();
      else
        cout<<" "<<ss4.str();  
    }
    cout<<endl;
    }
  }


void
CirMgr::printPIs() const
{
   unsigned temp=0;
   cout << "PIs of the circuit:";
   while(temp!=_pilist.size())
   {
    cout << " " <<_pilist[temp];
    temp++;
   }
   cout << endl;
}

void
CirMgr::printPOs() const
{
   unsigned temp=0;
   cout << "POs of the circuit:";
   while(temp!=_polist.size())
   {
    cout << " " <<_polist[temp];
    temp++;
   }
   cout << endl;
}

void
CirMgr::printFloatGates() const
{
  vector<CirGate*>::const_iterator it = _totallist.begin();
  IdList _floatID;
  IdList _unuseID;
  for(unsigned i=1;i<_totallist.size();i++)
  {
    if(_totallist[i]!=NULL)
    {
      if(_totallist[i]->_fanin.size()!=0)
      {
        if(_totallist[i]->_fanin[0].gate()->getTypeStr()=="UNDEF")
        {
          _floatID.push_back(i);
        }
        
        if(_totallist[i]->_fanin.size()==2)
        {
          
          if(_totallist[i]->_fanin[1].gate()->getTypeStr()=="UNDEF")
          {
            if(_floatID.size()==0||_floatID[_floatID.size()-1]!=i)
            {
              _floatID.push_back(i);
            }
          }
        }
      }

      if(_totallist[i]->_fanout.size()==0)
      {
        if(_totallist[i]->getTypeStr()!="PO")
        {
        _unuseID.push_back(i);
        }
      }
    }
  }


  unsigned temp1=0,temp2=0;

  if(_floatID.size()!=0)
  {
    cout<<"Gates with floating fanin(s):";
    while(temp1!=_floatID.size())
    {
      cout << " " <<_floatID[temp1];
      temp1++;
    }
    cout<<endl;
  }
  
  if(_unuseID.size()!=0)
  {
    cout<<"Gates defined but not used  :";
    while(temp2!=_unuseID.size())
    {
      cout << " " <<_unuseID[temp2];
      temp2++;
    }
    cout<<endl;
  }
}

void
CirMgr::writeAag(ostream& outfile) const
{
  unsigned count=0;
  for(unsigned i=0;i!=dfslist.size();i++)
  {
    if(dfslist[i]->getTypeStr()=="AIG")
    {
      count++;
    }
  }

 outfile<<"aag "<<M<<" "<<I<<" "<<L<<" "<<O<<" "<<count<<endl;
 for(unsigned i=0;i<_pilist.size();i++)
 {
  outfile<<_pilist[i]*2<<endl;
 }
 for(unsigned i=0;i<_polist.size();i++)
 {
  outfile<<_totallist[_polist[i]]->_faninID[0]<<endl;
 }
 for(unsigned i=0;i<dfslist.size();i++)
 {
  if(dfslist[i]->getTypeStr()=="AIG")
  {
    outfile<<(dfslist[i]->getID())*2<<" "<<dfslist[i]->_faninID[0]<<" "<<dfslist[i]->_faninID[1]<<endl;
  }
 }
 

 for(unsigned i=0;i<_pilist.size();i++)
 {
  string ss = static_cast<CirPiGate*>(_totallist[_pilist[i]])->_name;
  if(ss!="")
  outfile<<"i"<<i<<" "<<ss<<endl;
 }

 for(unsigned i=0;i<_polist.size();i++)
 {
  string ss = static_cast<CirPoGate*>(_totallist[_polist[i]])->_name;
  if(ss!="")
  outfile<<"o"<<i<<" "<<ss<<endl;
 }

 outfile<<"c"<<endl;
 outfile<<"AAG output by Pig and Koala"<<endl;

}



bool lexOptions
(const string& option, vector<string>& tokens, size_t nOpts) 
{
   string token;
   size_t n = myStrGetTok(option, token);
   while (token.size()) {
      tokens.push_back(token);
      n = myStrGetTok(option, token, n);
   }
   if (nOpts != 0) {
      if (tokens.size() < nOpts) {
         return false;
      }
      if (tokens.size() > nOpts) {
         return false;
      }
   }
   return true;
}
/****************************************************************************
  FileName     [ cirGate.cpp ]
  PackageName  [ cir ]
  Synopsis     [ Define class CirAigGate member functions ]
  Author       [ Chung-Yang (Ric) Huang ]
  Copyright    [ Copyleft(c) 2008-present LaDs(III), GIEE, NTU, Taiwan ]
****************************************************************************/

#include <iostream>
#include <iomanip>
#include <sstream>
#include <stdarg.h>
#include <cassert>
#include <algorithm>
#include "cirGate.h"
#include "cirMgr.h"
#include "util.h"
#include "bitset"

using namespace std;

// TODO: Keep "CirGate::reportGate()", "CirGate::reportFanin()" and
//       "CirGate::reportFanout()" for cir cmds. Feel free to define
//       your own variables and functions.

extern CirMgr *cirMgr;

unsigned CirGate::_globalRef = 0 ;

/**************************************/
/*   class CirGate member functions   */
/**************************************/
void
CirGate::reportGate() const
{
	for(int i=0;i<80;i++)
	{cout<<"=";}
	cout<<endl;

	cout<<"= ";
	printGate();
	cout<<endl;
	
	cout<<"= FECs:";
  printFec();
	cout<<endl;
	
	cout<<"= Value: ";
  printSim();
  cout<<endl;
	
	for(int i=0;i<80;i++)
	{cout<<"=";}
	cout<<endl;
}


void 
CirGate::printFec() const
{
  vector<FecGrp> temp = cirMgr->getGrps();

  if(temp.size()==0) return;
  
  if(fecpos==-1) return;
  
  for(unsigned i=0; i < temp[fecpos].size(); i++)
  {
    if(temp[fecpos][i]!=_id)
    {
      if(cirMgr->getGate(temp[fecpos][i])->_sim!=_sim)
          cout<<" !";
        else
          cout<<" ";
        cout << temp[fecpos][i];
    }
  }
}

void 
CirGate::printSim() const
{
  bitset<64> temp(_sim);
  for(int i=63; i>0; --i)
  {
    cout<<temp[i];
    if(i%8==0)cout<<"_";
  }
  cout<<temp[0];
}



void
CirGate::reportFanin(int level) const
{
   assert (level >= 0);
   setGlobalRef();
   recursFanin("",level+1,false);      
   
}

void
CirGate::recursFanin(string t, int level,bool inv) const
{
  if(level==0) return;

  cout<<t<<(inv?"!":"")<<getTypeStr()<<" "<<_id;
  
  if(isGlobalRef() && level>1 && _fanin.size()) 
  {
    cout<<" (*)"<<endl; 
    return;
  }
  else 
  {
    if(!isGlobalRef())
      _ref=_globalRef;
      cout<<endl;
  } 
  for(unsigned i=0;i<_fanin.size();i++)
    _fanin[i].gate()->recursFanin(t+"  ",level-1,_fanin[i].isInv());
}




void
CirGate::reportFanout(int level) const
{
   assert (level >= 0);
   setGlobalRef();
   recursFanout("",level+1,false); 
}



void
CirGate::recursFanout(string t, int level,bool inv) const
{
  if(level==0) return;

  cout<<t<<(inv?"!":"")<<getTypeStr()<<" "<<_id;
  
  if(isGlobalRef() && level>1 && _fanout.size()) 
  {
    cout<<" (*)"<<endl; 
    return;
  }
  else 
  {
    if(!isGlobalRef())
    _ref=_globalRef;
    cout<<endl;
  } 

  for(unsigned i=0;i<_fanout.size();i++)
    _fanout[i].gate()->recursFanout(t+"  ",level-1,_fanout[i].isInv());
}




void 
CirGate::dfsTraversal(GateList &dfsList, vector<bool> &dfs) 
{
  for(unsigned i=0;i<_fanin.size();i++)
  {
    if(!_fanin[i].gate()->isGlobalRef())
    {
      _fanin[i].gate()->setToGlobalRef();
      _fanin[i].gate()->dfsTraversal(dfsList,dfs);
    }
  }
  dfsList.push_back(this);
  dfs[this->getID()]=true;
}

void
CirGate::gensim()
{
  if(getTypeStr()=="AIG")
  {
    size_t in0=(_fanin[0].isInv())?(~(_fanin[0].gate()->_sim)):(_fanin[0].gate()->_sim);
    size_t in1=(_fanin[1].isInv())?(~(_fanin[1].gate()->_sim)):(_fanin[1].gate()->_sim);
    _sim = in0&in1;
  }
  if(getTypeStr()=="PO")
  {
    _sim = (_fanin[0].isInv())?(~(_fanin[0].gate()->_sim)):(_fanin[0].gate()->_sim);
  }
}
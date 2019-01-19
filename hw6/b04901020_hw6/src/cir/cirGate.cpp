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

using namespace std;

extern CirMgr *cirMgr;

unsigned CirGate::_globalRef = 0 ;


// TODO: Implement memeber functions for class(es) in cirGate.h

/**************************************/
/*   class CirGate member functions   */
/**************************************/

void
CirGate::reportGate() const
{
	cout<<"=================================================="<<endl;
	cout<<"= ";
	printGate();
	cout<<"="<<endl;
	cout<<"=================================================="<<endl;
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
CirGate::dfsTraversal(GateList &dfsList) 
{
  for(unsigned i=0;i<_fanin.size();i++)
  {
    if(!_fanin[i].gate()->isGlobalRef())
    {
      _fanin[i].gate()->setToGlobalRef();
      _fanin[i].gate()->dfsTraversal(dfsList);
    }
  }
  dfsList.push_back(this);
}
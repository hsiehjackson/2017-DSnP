/****************************************************************************
  FileName     [ cirSim.cpp ]
  PackageName  [ cir ]
  Synopsis     [ Define cir optimization functions ]
  Author       [ Chung-Yang (Ric) Huang ]
  Copyright    [ Copyleft(c) 2008-present LaDs(III), GIEE, NTU, Taiwan ]
****************************************************************************/

#include <cassert>
#include "cirMgr.h"
#include "cirGate.h"
#include "util.h"

using namespace std;

// TODO: Please keep "CirMgr::sweep()" and "CirMgr::optimize()" for cir cmd.
//       Feel free to define your own variables or functions

/*******************************/
/*   Global variable and enum  */
/*******************************/

/**************************************/
/*   Static varaibles and functions   */
/**************************************/

/**************************************************/
/*   Public member functions about optimization   */
/**************************************************/
// Remove unused gates
// DFS list should NOT be changed
// UNDEF, float and unused list may be changed
void
CirMgr::sweep()
{
	for(size_t i=1;i<_totallist.size();i++)
	{
		if(_totallist[i]!=0)
		{
			if(_totallist[i]->getTypeStr()!="PI")
			{
				if(booldfs[i]!=true)
				{	
					if(_totallist[i]->getTypeStr()=="AIG")A--;
					cout<<"Sweeping: "<<_totallist[i]->getTypeStr()<<"("<<_totallist[i]->getID()<<")";
					cout<<" removed..."<<endl;
					delete _totallist[i];
					_totallist[i] =NULL; 
				}
			}
		}
	}
	
	for(size_t i=1;i<_totallist.size();i++)
	{
		if(_totallist[i]!=0)
		{
			for(size_t j=0;j<_totallist[i]->_fanout.size();j++)
			{
				if(_totallist[(_totallist[i]->_fanout[j].gate()->getID())]==NULL)
				{
					_totallist[i]->_fanout.erase(_totallist[i]->_fanout.begin()+j);		
				}
			}
			
		}
	}	
}

// Recursively simplifying from POs;
// _dfsList needs to be reconstructed afterwards
// UNDEF gates may be delete if its fanout becomes empty...
void
CirMgr::optimize()
{
	for(size_t i=0;i<dfslist.size();i++)
	{
		if(dfslist[i]->getTypeStr()=="AIG")
		{
			CirGateV f0 = dfslist[i]->_fanin[0];
			CirGateV f1 = dfslist[i]->_fanin[1];
			bool dirty = false;

			if(f0.gate()->getID()==0)
			{
				dirty = true;
				if(f0.isInv())
					oneinput(dfslist[i]);
				else
					zeroinput(dfslist[i]);
			}
			else if(f1.gate()->getID()==0)
			{
				dirty = true;
				if(f1.isInv())
					oneinput(dfslist[i]);
				else
					zeroinput(dfslist[i]);
			}
			else if(f0.gate()->getID()==f1.gate()->getID())	
			{
				dirty = true;
				if(f0.isInv()==f1.isInv())
					oneinput(dfslist[i]);
				else
					zeroinput(dfslist[i]);
			}
			if(dirty)
			{
				delete _totallist[dfslist[i]->getID()];
				_totallist[dfslist[i]->getID()]=NULL;
				A--;
			}
		}
	}
	genDFS();
}

/***************************************************/
/*   Private member functions about optimization   */
/***************************************************/


void 
CirMgr::oneinput(CirGate* gate)
{

	removein(gate);
	CirGateV fin = (gate->_fanin[0].gate()->getID()!=0)?gate->_fanin[0]:gate->_fanin[1];
	cout<<"Simplifying: "<<fin.gate()->getID()<<" merging ";
	cout<<(fin.isInv()?"!":"")<<gate->getID()<<"..."<<endl;


	for(size_t i=0; i < gate->_fanout.size();i++)
	{
		CirGateV out = gate->_fanout[i];
		CirGateV temp = CirGateV(out.gate(),(out.isInv()!=fin.isInv()));
		fin.gate()->_fanout.push_back(temp);// in_fanout

		for(size_t j=0;j<out.gate()->_fanin.size();j++)
		{
			if(out.gate()->_fanin[j].gate()->getID()==gate->getID())
			{
				CirGateV temp2 = CirGateV(fin.gate(),(out.isInv()!=fin.isInv()));
				out.gate()->_fanin[j] = temp2;//out_fanin
 			}
		}
	}

}
void 
CirMgr::zeroinput(CirGate* gate)
{
	removein(gate);
	cout<<"Simplifying: 0 merging "<<gate->getID()<<"..."<<endl;
	for(size_t i=0; i < gate->_fanout.size();i++)
	{
		CirGateV out = gate->_fanout[i];
		CirGateV temp = CirGateV(out.gate(),out.isInv());
		_totallist[0]->_fanout.push_back(temp);//in_fanout

		for(size_t j=0;j<out.gate()->_fanin.size();j++)
		{
			if(out.gate()->_fanin[j].gate()->getID()==gate->getID())
			{
				CirGateV temp2 = CirGateV(_totallist[0],out.isInv());
				out.gate()->_fanin[j] = temp2;//out_fanin
 			}
		}
	}
}

void 
CirMgr::removein(CirGate* gate)
{
	for(size_t i=0; i < gate->_fanin.size();i++)
	{
		CirGate* Ingate = gate->_fanin[i].gate();
		for(size_t j=0; j< Ingate->_fanout.size();j++)
		{
			if(Ingate->_fanout[j].gate()->getID()==gate->getID())
			{
				Ingate->_fanout.erase(Ingate->_fanout.begin()+j);
				break;
			}
		}
	}
}


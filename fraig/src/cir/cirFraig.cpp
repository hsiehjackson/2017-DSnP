/****************************************************************************
  FileName     [ cirFraig.cpp ]
  PackageName  [ cir ]
  Synopsis     [ Define cir FRAIG functions ]
  Author       [ Chung-Yang (Ric) Huang ]
  Copyright    [ Copyleft(c) 2012-present LaDs(III), GIEE, NTU, Taiwan ]
****************************************************************************/

#include <cassert>
#include "cirMgr.h"
#include "cirGate.h"
#include "sat.h"
#include "myHashMap.h"
#include "util.h"

using namespace std;

// TODO: Please keep "CirMgr::strash()" and "CirMgr::fraig()" for cir cmd.
//       Feel free to define your own variables or functions

/*******************************/
/*   Global variable and enum  */
/*******************************/

/**************************************/
/*   Static varaibles and functions   */
/**************************************/
static SatSolver solver;
typedef pair<CirGate*, CirGate*> mergeGate;
unsigned CirGate::_globalRef2 = 0 ;
/*******************************************/
/*   Public member functions about fraig   */
/*******************************************/
class HashKey
{
public:
   HashKey(CirGate* gate) 
   {
   	size_t temp0=gate->getfainin(0)._gateV ;
   	size_t temp1=gate->getfainin(1)._gateV ;
   	if(temp0 > temp1)
   	{
   		_faninhash[0]=temp0;
   		_faninhash[1]=temp1;
   	}
   	else
   	{
   		_faninhash[0]=temp1;
   		_faninhash[1]=temp0;
    }
   }
   size_t operator() () const 
   { 
   	//return _faninhash[0]+_faninhash[1]+_faninhash[0]%256*_faninhash[1]%256;
   	//return _faninhash[0]+_faninhash[1]<<5;
   	return (_faninhash[0]>>6&0xfff)*(_faninhash[1]>>6&0xfff);  
   }
   void operator = (const HashKey& k)
   {
   	_faninhash[0]=k._faninhash[0];
   	_faninhash[1]=k._faninhash[1];
   } 
   bool operator == (const HashKey& k) const 
   { 
   	return (_faninhash[0]==k._faninhash[0]&&_faninhash[1]==k._faninhash[1]); 
   }
 
private:
	size_t _faninhash[2];
};

// _floatList may be changed.
// _unusedList and _undefList won't be changed
void
CirMgr::strash()
{
	HashMap<HashKey,unsigned> _CirHash(dfslist.size());
	for(size_t i=0; i < dfslist.size();i++)
	{
		if(dfslist[i]->getTypeStr()=="AIG")
		{
			HashKey k(dfslist[i]);
			unsigned id;
			if(_CirHash.query(k,id))
			{
				cout<<"Strashing: "<<id<<" merging ";
				cout<<dfslist[i]->getID()<<"..."<<endl;

				merge(_totallist[id],dfslist[i]);
				delete _totallist[dfslist[i]->getID()];
				_totallist[dfslist[i]->getID()]=NULL;
				A--;
			}
			else
			_CirHash.insert(k, dfslist[i]->getID());
		}
	}  
	genDFS();
}

void
CirMgr::fraig()
{
	
	runfraig(solver);
	solver.reset();
	cout<<"\r\033[K";
	strash();
	_FecGrps.clear();
}

/********************************************/
/*   Private member functions about fraig   */
/********************************************/

void
CirMgr::runfraig(SatSolver& s)
{
	vector<mergeGate> res;
	unsigned countCEX=0;
	fraigInit(s);
	CirGate::setGlobalRef();
	CirGate::setGlobalRef2();
	
	for(unsigned i=0; i<dfslist.size(); i++)
	{
		if(dfslist[i]->isAig()==0)
			continue;
		if((dfslist[i]->fecpos)==-1)
			continue;
		if((dfslist[i]->fecpos)==0)
		{
			if(dfslist[i]->_fanin[0].gate()->isGlobalRef2()||dfslist[i]->_fanin[1].gate()->isGlobalRef2())
			{
				if(dfslist[i]->_sim==0)
				{
					mergeGate temp(_totallist[0],dfslist[i]);
					res.push_back(temp);
					continue;
				}
			}
			else if(!(SATsolve(s,dfslist[i],_totallist[0],(dfslist[i]->getsim()!=_totallist[0]->getsim()))))
			{
				mergeGate temp(_totallist[0],dfslist[i]);
				res.push_back(temp);
				dfslist[i]->setToGlobalRef2();
			}
			else
			{
				storeCEX(s);
				countCEX++;
			}
		}
		else
		{
			for(unsigned j=0; j<_FecGrps[dfslist[i]->fecpos].size(); j++)
			{
				unsigned id = _FecGrps[dfslist[i]->fecpos][j];
				bool inv = (dfslist[i]->getsim()!=_totallist[id]->getsim());
				if(dfslist[i]->getID()==id)
					continue;
				if(!(_totallist[id]->isGlobalRef())&&!(_totallist[id]->isGlobalRef2()))
				{
					if(!(SATsolve(s,dfslist[i],_totallist[id],inv)))
					{
						_totallist[id]->setToGlobalRef2();
						mergeGate temp(dfslist[i],_totallist[id]);
						res.push_back(temp);
					}
					else
					{
						storeCEX(s);
						countCEX++;
					}

				}
				if(countCEX==64) break;
			}
			dfslist[i]->setToGlobalRef();
		}
		if(countCEX==64)
		{
			cout<<"\r\033[KUpdating by SAT... Total #FEC Group = "<<_FecGrps.size()<<endl;
			fraigmerge(res);
			genDFS();
			setallsim();
			setFecGrps();
			updateFecGrps();
			i=0;
			res.clear();
			countCEX=0;
			fraigInit(s);
			CirGate::setGlobalRef();
			CirGate::setGlobalRef2();
		}
	/*
		if(res.size()>500)
		{
			fraigmerge(res);
			genDFS();
			i=0;
			res.clear();
			fraigInit(s);
			CirGate::setGlobalRef();
			CirGate::setGlobalRef2();
		}	
	*/	
	}
	fraigmerge(res);
	genDFS();
	res.clear();
}























/*
	for(unsigned i=0; i<dfslist.size(); i++)
	{	
		if(dfslist[i]->isAig()!=true)
			continue;
		if((dfslist[i]->fecpos)==-1)
			continue;
		for(int j=0; j<_FecGrps[dfslist[i]->fecpos].size(); j++)
		{

			unsigned id = _FecGrps[dfslist[i]->fecpos][j];
			bool inv = (dfslist[i]->getsim()!=_totallist[id]->getsim());
				//cout<<"("<<dfslist[i]->getID()<<" "<<id<<")"<<endl;
				if(dfslist[i]->getID()==id)
					continue;
				if(!(_totallist[id]->isGlobalRef())&&!(_totallist[id]->isGlobalRef2()))
				{
					//cout<<endl<<dfslist[i]->getID()<<" "<<id<<endl;
					if(!(SATsolve(s,dfslist[i],_totallist[id],inv)))
					{	
						if(id==0)
						{
							mergeGate temp(_totallist[id],dfslist[i]);
							res.push_back(temp);
						}
						else
						{
							_totallist[id]->setToGlobalRef2();
							mergeGate temp(dfslist[i],_totallist[id]);
							res.push_back(temp);
						}
					}
					else
					{
						storeCEX(s);
						countCEX++;
					}
				}
				if(id==0)
					break;
		}
		dfslist[i]->setToGlobalRef();
		if(countCEX>=64)
		{
			//cout<<"\r\033[KUpdating by SAT... Total #FEC Group = "<<_FecGrps.size()<<endl;
			fraigmerge(res);
			genDFS();
			setallsim();
			setFecGrps();
			updateFecGrps();
			i=0;
			res.clear();
			countCEX=0;
			fraigInit(s);
			CirGate::setGlobalRef();
			CirGate::setGlobalRef2();
		}
	}

	fraigmerge(res);
	genDFS();
}
	
	
	for(unsigned i=0; i<dfslist.size(); i++)
	{	
		if(dfslist[i]->isAig()!=true)
			continue;
		if((dfslist[i]->fecpos)==-1)
			continue;
		for(int j=0; j<_FecGrps[dfslist[i]->fecpos].size(); j++)
		{

			unsigned id = _FecGrps[dfslist[i]->fecpos][j];
			bool inv = (dfslist[i]->getsim()!=_totallist[id]->getsim());
				//cout<<"("<<dfslist[i]->getID()<<" "<<id<<")"<<endl;
				if(dfslist[i]->getID()==id)
					continue;
				if(!(_totallist[id]->isGlobalRef()))
				{

					//cout<<endl<<dfslist[i]->getID()<<" "<<id<<endl;
					if(!(SATsolve(s,dfslist[i],_totallist[id],inv)))
					{	
						if(id==0)
						{
							mergeGate temp(_totallist[id],dfslist[i]);
							res.push_back(temp);
							
							for(int k=0; k<_FecGrps[dfslist[i]->fecpos].size(); k++)
							{
								if(dfslist[i]->getID()==_FecGrps[dfslist[i]->fecpos][k])
								{
									_FecGrps[dfslist[i]->fecpos].erase(_FecGrps[dfslist[i]->fecpos].begin()+k);
									dfslist[i]->setpos(-1);
									break;
								}
							}
							j--;
						}
						else
						{
							mergeGate temp(dfslist[i],_totallist[id]);
							res.push_back(temp);
							_totallist[id]->setpos(-1);
							_FecGrps[dfslist[i]->fecpos].erase(_FecGrps[dfslist[i]->fecpos].begin()+j);
							j--;
						}
					}
					else
					{
						storeCEX(s);
						countCEX++;
					}
				}
				if(id==0)
					break;
		}
		dfslist[i]->setToGlobalRef();
		
		if(countCEX>=64)
		{
			cout<<"\r\033[KUpdating by SAT... Total #FEC Group = "<<_FecGrps.size()<<endl;
			fraigmerge(res);
			genDFS();
			setallsim();
			setFecGrps();
			updateFecGrps();
			i=0;
			res.clear();
			countCEX=0;
			fraigInit(s);
			CirGate::setGlobalRef();
		}

		
		if(res.size()>370)
		{
			fraigmerge(res);
			genDFS();
			i=0;
			res.clear();
			fraigInit(s);
			CirGate::setGlobalRef();
		}
		
			
	}
	fraigmerge(res);
	genDFS();
	
}
*/
void 
CirMgr::fraigInit(SatSolver& s)
{	
	s.reset();
	s.initialize();

	_totallist[0]->setVar(s.newVar());

	for(unsigned i = 0; i < _pilist.size(); i++)
	{
		Var v = s.newVar();
		_totallist[_pilist[i]]->setVar(v);
	}	

	for(unsigned i = 0; i < dfslist.size(); i++)
	{
		if(dfslist[i]->isAig())
		{
			Var v =s.newVar();
			dfslist[i]->setVar(v);
		}
	}


	for(unsigned i = 0; i < dfslist.size(); i++)
	{
		if(dfslist[i]->isAig())
		{
			Var var0 = dfslist[i]->getVar();
			Var var1 = dfslist[i]->_fanin[0].gate()->getVar();
			Var var2 = dfslist[i]->_fanin[1].gate()->getVar();
			bool in1 = dfslist[i]->_fanin[0].isInv();
			bool in2 = dfslist[i]->_fanin[1].isInv();
			s.addAigCNF(var0, var1, in1, var2, in2);
		}
	}
}


bool
CirMgr::SATsolve(SatSolver& s, CirGate* g1, CirGate*g2, bool inv)
{	
	cout<<"\r\033[K"<<"Proving ("<<g1->getID()<<", "<<((g1->_sim==g2->_sim)?"":"!")<<g2->getID()<<")..."<<flush;
	Var v = s.newVar();
	s.addXorCNF(v, g1->getVar(), inv, g2->getVar(),false);
	s.assumeRelease();
	s.assumeProperty(_totallist[0]->getVar(), false);
	s.assumeProperty(v,true);
	return s.assumpSolve();
}

void 
CirMgr::storeCEX(SatSolver& s)
{
	for(unsigned i=0; i<_pilist.size(); i++)
	{
		CirGate* g =_totallist[_pilist[i]];
		size_t p = ((g->getsim()<<1)+s.getValue(g->getVar()));
		g->setsim(p);
	}
}

void
CirMgr::fraigmerge(vector<mergeGate> rec)
{
	for(unsigned i=0; i<rec.size(); i++)
	{
		CirGate* g1=rec[i].first;
		CirGate* g2=rec[i].second;
		cout<<"\r\033[K"<<"Fraig: "<<g1->getID()<<" merging "<<((g1->_sim==g2->_sim)?"":"!")<<g2->getID()<<"..."<<endl;
		merge(g1,g2);
		
		for(unsigned j=0; j<_FecGrps[g2->fecpos].size(); j++)
		{
			if(_FecGrps[g2->fecpos][j]==g2->getID())
			{
				_FecGrps[g2->fecpos].erase(_FecGrps[g2->fecpos].begin()+j);
				break;
			}
		}
		
		_totallist[g2->getID()]=NULL;
		delete g2;
		g2 = NULL;
		A--;		
	}
}



void 
CirMgr::merge(CirGate* g1,CirGate* g2)
{
	removein(g2);
	bool inv = ((g1->_sim)!=(g2->_sim));				
	for(size_t j=0; j < g2->_fanout.size(); j++)
	{
		CirGateV out = g2->_fanout[j];
		g1->_fanout.push_back(out);// in_fanout
		for(size_t k=0; k < out.gate()->_fanin.size(); k++)
		{
			if(out.gate()->_fanin[k].gate()->getID()==g2->getID())
			{
				CirGateV temp2 = CirGateV(g1,out.isInv()!=inv);
				out.gate()-> _fanin[k] = temp2;//out_fanin
 			}
		}
	}

}
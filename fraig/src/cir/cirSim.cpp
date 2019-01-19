/****************************************************************************
  FileName     [ cirSim.cpp ]
  PackageName  [ cir ]
  Synopsis     [ Define cir simulation functions ]
  Author       [ Chung-Yang (Ric) Huang ]
  Copyright    [ Copyleft(c) 2008-present LaDs(III), GIEE, NTU, Taiwan ]
****************************************************************************/

#include <fstream>
#include <iostream>
#include <iomanip>
#include <algorithm>
#include <cassert>
#include <cmath>
#include "cirMgr.h"
#include "cirGate.h"
#include "myHashMap.h"
#include "util.h"
#include "bitset"

using namespace std;

// TODO: Keep "CirMgr::randimSim()" and "CirMgr::fileSim()" for cir cmd.
//       Feel free to define your own variables or functions
MyUsage use;

/*******************************/
/*   Global variable and enum  */
/*******************************/
class simkey
{
public:
	simkey(){}
	simkey(const size_t s):_sim(s){}

	size_t operator() () const
	{
		return (_sim > (~_sim))?(_sim):(~_sim);
	}

	bool operator == (const simkey& k) const
	{
		return (_sim==k._sim||~(_sim)==k._sim);
	}

private:
	size_t _sim;
};
/**************************************/
/*   Static varaibles and functions   */
/**************************************/
static bool compare(FecGrp gp1, FecGrp gp2)
{
	return gp1[0]<gp2[0];
}

bool 
CirMgr::checkpattern(string &line)
{
	if(line.size()!=I)
	{
		cout<<"Error: Pattern("<<line<<") length("<<line.size()<<")does not match the number of inputs("<<I<<") in a circuit!!"<<endl;
		return false;
	}
	for(size_t i=0; i<line.size();i++)
	{
		if(line[i]!='0'&&line[i]!='1')
		{
			cout<<"Error: Pattern("<<line<<") contains a non-0/1 character('"<<line[i]<<"')."<<endl;
			return false;
		}
	}
	return true;
}


/************************************************/
/*   Public member functions about Simulation   */
/************************************************/
void
CirMgr::randomSim()
{
	unsigned presize = _FecGrps.size();
	unsigned fail = 0;
	unsigned num = 0;
	unsigned maxfail = size_t(pow(I,0.5)*5);

	do
	{
		for(int i=0; i<I; i++)
		{
			size_t temp = ((((size_t)(rnGen(INT_MAX))<<32)|(size_t)(rnGen(INT_MAX))));
			_totallist[_pilist[i]]->setsim(temp);
		}
		setallsim();
		simlog(64);
		setFecGrps();


		//double time = use.clk(1);
		//double standard = (abs(presize - _FecGrps.size())/time);
		//if(standard < 100.0) ++fail;
		if((presize==_FecGrps.size()))++fail;
		presize = _FecGrps.size();
		num++;
	}
	while(fail<maxfail);
	cout<<"\r\033[K"<<num*64<<" patterns simulated."<<endl;
	updateFecGrps();
}

void
CirMgr::fileSim(ifstream& patternFile)
{
	string line;
	unsigned num=0,count=0;
	
	vector<size_t> pattern(I,0);
	while(getline(patternFile, line))
	{
		vector<string> options;
		lexOptions(line, options, 0);

		for(size_t i=0; i<options.size();i++)
		{
			if(checkpattern(options[i])==0)
			{
				cout<<num<<" patterns simulated."<<endl;
				return;
			}
			for(int j=0;j<I;j++)
			{
				pattern[j]=pattern[j]+((size_t(options[i][j]-48))<<count);

			}
			if(++count==64)
			{
				for(int j=0; j<I; j++)
				{
					//printSim(pattern[j]);cout<<endl;
					_totallist[_pilist[j]]->setsim(pattern[j]);
					pattern[j]=0;
				}
				num=num+64;
				count=0;
				setallsim();
				simlog(64);
				setFecGrps();
				
			}
		} 
	}

	if(count!=0)
	{
		for(int j=0; j<I; j++)
		{
			_totallist[_pilist[j]]->setsim(pattern[j]);
			pattern[j]=0;
		}

		setallsim();
		simlog(count);
		setFecGrps();

	}

	cout<<"\r\033[K"<<num+count<<" patterns simulated."<<endl;
	updateFecGrps();

}



void 
CirMgr::setallsim()
{
	for(unsigned  i=0; i < dfslist.size(); i++)
	{
		dfslist[i]->gensim();
	}
}




void 
CirMgr::setFecGrps()
{

	if(_FecGrps.size()==0)//init
	{
		FecGrp gp;
		gp.push_back(0);

		for(unsigned  i=0; i < dfslist.size(); i++)
		{
			if(dfslist[i]->isAig())
			gp.push_back(dfslist[i]->getID());
		}
		_FecGrps.push_back(gp);
	}


	vector<FecGrp> temp;

	for(int i=_FecGrps.size()-1;i>=0;--i)
	{
		int s = _FecGrps[i].size();

		HashMap<simkey,FecGrp*> _FecMap(s);
		
		for(int j=0; j<s; j++)
		{

			simkey k(_totallist[_FecGrps[i][j]]->_sim);
			FecGrp *gp;
			if(_FecMap.query(k,gp))
			{
				gp->push_back(_FecGrps[i][j]);// setMap
			}
			else
			{
				FecGrp* temp = new FecGrp;
				temp->push_back(_FecGrps[i][j]);
				_FecMap.insert(k,temp);// setMap 
			}

		}

		HashMap<simkey,FecGrp*>::iterator it = _FecMap.begin();
		for(;it != _FecMap.end(); it++)
		{
			if(it!=0)
			{	
				if((*it)->size()>1)
				{
					sortFecGrp(*(*it));
					temp.push_back(*(*it));
				}
			}
			delete *it;
		}
	}

	_FecGrps.clear();
	_FecGrps = temp;
	temp.clear();

	cout<<"\r\033[K"<<"Total #FEC Group = "<<_FecGrps.size()<<flush; 
}



void 
CirMgr::resetFecGrps()
{
	for(unsigned  i=0; i < dfslist.size();i++)
	{
		if(dfslist[i])
		dfslist[i]->setpos(-1);
	}
}


void 
CirMgr::updateFecGrps()
{
	sortFecGrps();
	resetFecGrps();
	connectgateGrp();
}

void 
CirMgr::connectgateGrp()
{

	for(unsigned i=0; i < _FecGrps.size(); i++)
	{
		for(unsigned j=0; j < _FecGrps[i].size(); j++)
		{
			_totallist[_FecGrps[i][j]]->setpos(i);
		}
	}
}


void 
CirMgr::simlog(unsigned num)
{
	unsigned count=0;
	if(_simLog==0)return;
	while(count<num)
	{
		for(unsigned i=0; i<_pilist.size();i++)
		{
			bitset<64> temp(_totallist[_pilist[i]]->_sim);
			*_simLog<<temp[count];
		}
		*_simLog<<" ";
		for(unsigned i=0; i<_polist.size();i++)
		{
			bitset<64> temp(_totallist[_polist[i]]->_sim);
			*_simLog<<temp[count];
		}
		*_simLog<<endl;
		count++;
	}
}
/*************************************************/
/*   Private member functions about Simulation   */
/*************************************************/


void 
CirMgr::sortFecGrps()
{
std::sort(_FecGrps.begin(),_FecGrps.end(),compare);
}

void 
CirMgr::sortFecGrp(FecGrp &gp)
{
std::sort(gp.begin(),gp.end());
}


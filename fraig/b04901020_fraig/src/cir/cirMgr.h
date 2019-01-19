/****************************************************************************
  FileName     [ cirMgr.h ]
  PackageName  [ cir ]
  Synopsis     [ Define circuit manager ]
  Author       [ Chung-Yang (Ric) Huang ]
  Copyright    [ Copyleft(c) 2008-present LaDs(III), GIEE, NTU, Taiwan ]
****************************************************************************/

#ifndef CIR_MGR_H
#define CIR_MGR_H

#include <vector>
#include <string>
#include <fstream>
#include <iostream>
#include <map>
#include "sat.h"

using namespace std;

// TODO: Feel free to define your own classes, variables, or functions.

#include "cirDef.h"

extern CirMgr *cirMgr;
typedef pair<CirGate*, CirGate*> mergeGate;

class simkey;

class CirMgr
{
public:
   CirMgr() {}
   ~CirMgr() {} 

   // Access functions
   // return '0' if "gid" corresponds to an undefined gate.
   CirGate* getGate(unsigned gid) const 
   { 
      if(gid>=_totallist.size())
        return NULL;
      else 
        return _totallist[gid]; 
   }

   // Member functions about circuit construction
   bool readCircuit(const string&);
   bool lexOptions(const string&, vector<string>&, size_t); 

   // Member functions about circuit optimization
   void sweep();
   void optimize();

   // Member functions about simulation
   void randomSim();
   void fileSim(ifstream&);
   void setSimLog(ofstream *logFile) { _simLog = logFile; }

   // Member functions about fraig
   void strash();
   void printFEC() const;
   void fraig();

   // Member functions about circuit reporting
   void printSummary() const;
   void printNetlist() const;
   void printPIs() const;
   void printPOs() const;
   void printFloatGates() const;
   void printFECPairs() const;
   void writeAag(ostream&) const;
   void writeGate(ostream&, CirGate*) const;

   //optimize
   void oneinput(CirGate*);
   void zeroinput(CirGate*); 
   void removein(CirGate*);
   
   void genDFS();
   void merge(CirGate*,CirGate*);

   //sim
   FecGrp getFecGrp(unsigned num) const {return _FecGrps[num];}
   void setallsim();
   void setFecGrps();
   
   void resetFecGrps();
   void updateFecGrps();
   void connectgateGrp();
   void simlog(unsigned);

   void sortFecGrps();
   void sortFecGrp(FecGrp&);
   bool checkpattern(string &);
   vector<FecGrp> getGrps(){return _FecGrps;}



   //fraig
   void fraigInit(SatSolver&);
   bool SATsolve(SatSolver&, CirGate*, CirGate*, bool);
   void storeCEX(SatSolver&);
   void fraigmerge(vector<mergeGate>);
   void runfraig(SatSolver&);


private:
   //sim
   ofstream           *_simLog;
   vector<FecGrp>     _FecGrps;


   IdList _pilist;
   IdList _polist;
   GateList _totallist;
   mutable GateList dfslist;

   vector<bool> booldfs; // for sweep 
   int M,I,L,O,A;


  

};

#endif // CIR_MGR_H

/****************************************************************************
  FileName     [ cirGate.h ]
  PackageName  [ cir ]
  Synopsis     [ Define basic gate data structures ]
  Author       [ Chung-Yang (Ric) Huang ]
  Copyright    [ Copyleft(c) 2008-present LaDs(III), GIEE, NTU, Taiwan ]
****************************************************************************/

#ifndef CIR_GATE_H
#define CIR_GATE_H

#include <string>
#include <vector>
#include <map>
#include <iostream>
#include "cirDef.h"
#include <sstream>
#include <iomanip>
#include "sat.h"


using namespace std;

// TODO: Feel free to define your own classes, variables, or functions.

class CirGate;
class CirGateV;
//------------------------------------------------------------------------
//   Define classes
//------------------------------------------------------------------------

class CirGateV
{
public: 
  #define NEG 0x1
  CirGateV(CirGate* g, size_t phase):
    _gateV(size_t(g) + phase){}
  
  CirGate* gate() const{
      return (CirGate*) (_gateV & ~size_t(NEG));}
  
  bool isInv() const{return (_gateV & NEG);}

  size_t  _gateV;
};



class CirGate
{
public:
   friend class CirMgr;
   CirGate(): _linenum(-1),_id(-1),_ref(0),fecpos(-1),_sim(0),_var(0){};
   virtual ~CirGate() {}

   // Basic access methods
   string getTypeStr() const 
   {
      switch(this->_type)
      {
        case 0:
          return "UNDEF";
          break;
        case 1:
          return "PI";
          break;
        case 2:
          return "PO";
          break;
        case 3:
          return "AIG";
          break;
        case 4:
          return "CONST";
          break;
        default:
          return "";
          break;
      }  
   }
   unsigned getLineNo() const { return _linenum; }
   unsigned getID() const {return _id;}
   CirGateV getfainin(size_t i) const {return _fanin[i];}
   CirGateV getfainout(size_t i) const {return _fanout[i];}

   //*************************************************//
   virtual bool isAig() const { if(_type==AIG_GATE) return true; return false; }
   // Printing functions
   virtual void printGate() const {}
   void printFec() const;
   void printSim() const;
   void reportGate() const;
   //*************************************************//
   void reportFanin(int level) const;
   void recursFanin(string t, int ,bool) const;
   void reportFanout(int level) const;
   void recursFanout(string t, int ,bool) const;

   void dfsTraversal(GateList &dfsList, vector<bool> &dfs);
   bool isGlobalRef() const{return (_ref==_globalRef);}
   void setToGlobalRef(){_ref = _globalRef;}
   static void setGlobalRef(){_globalRef++;}

   bool isGlobalRef2() const{return (_ref2==_globalRef2);}
   void setToGlobalRef2(){_ref2 = _globalRef2;}
   static void setGlobalRef2(){_globalRef2++;}

   //sim
   void gensim();
   size_t getsim(){return _sim;}
   void setsim(size_t num){_sim = num;}
   void setpos(int num){fecpos = num;}
   //fraig
   Var getVar() const{return _var;}
   void setVar(const Var &v){_var = v;}

protected:
  vector<CirGateV>  _fanin;
  vector<CirGateV>  _fanout;
  IdList _faninID;
  unsigned  _linenum;
  unsigned  _id;
  GateType _type;
  static  unsigned _globalRef;
  mutable unsigned _ref;
  int fecpos;  //sim
  size_t _sim;  //sim
  Var _var; //fraig
  static  unsigned _globalRef2; //fraig
  mutable unsigned _ref2; //fraig
};



class CirAigGate: public CirGate
{
public:
  friend class CirMgr;
  CirAigGate(unsigned id, unsigned linenum)
  {_type = AIG_GATE;  _id=id;  _linenum=linenum;}
  ~CirAigGate(){}
  virtual void printGate() const
  {
    stringstream ss;
    ss<<getTypeStr()<<"("<<_id<<"), line "<<_linenum;
    cout<<ss.str();
  }
};


class CirPiGate: public CirGate
{
public:
  friend class CirMgr;
  CirPiGate(unsigned id, unsigned linenum)
  {_type = PI_GATE; _id=id;  _linenum=linenum;}
  ~CirPiGate(){} 
  virtual void printGate() const
  {
    stringstream ss;
    if(_name.size()!=0)
      ss<<getTypeStr()<<"("<<_id<<")\""<<_name<<"\", line "<<_linenum;
    else
      ss<<getTypeStr()<<"("<<_id<<"), line "<<_linenum;
    cout<<ss.str();
  }
protected:
  string _name;

};


class CirPoGate: public CirGate
{
public:
  friend class CirMgr;
  CirPoGate(unsigned id, unsigned linenum)
  {_type = PO_GATE; _id=id;  _linenum=linenum;}
  ~CirPoGate(){}
  virtual void printGate() const
  {
    stringstream ss;
    if(_name.size()!=0)
      ss<<getTypeStr()<<"("<<_id<<")\""<<_name<<"\", line "<<_linenum;
    else
      ss<<getTypeStr()<<"("<<_id<<"), line "<<_linenum;
    cout<<ss.str();
  }
protected:
  string _name;
};


class CirConstGate: public CirGate
{
public:
  friend class CirMgr;
  CirConstGate(unsigned id=0, unsigned linenum=0)
  {_type = CONST_GATE; _id=id;  _linenum=linenum;}
  ~CirConstGate(){}
  virtual void printGate() const
  {
    stringstream ss;
    ss<<getTypeStr()<<"("<<_id<<"), line "<<_linenum;
    cout<<ss.str();
  }
};


class CirUndefGate: public CirGate
{
public:
  friend class CirMgr;
  CirUndefGate(unsigned id, unsigned linenum=0)
  {_type = UNDEF_GATE; _id=id;  _linenum=linenum;}
  ~CirUndefGate(){}
  virtual void printGate() const
  {
    stringstream ss;
    ss<<getTypeStr()<<"("<<_id<<"), line "<<_linenum;
    cout<<ss.str();
  }
};
#endif // CIR_GATE_H


/****************************************************************************
  FileName     [ memCmd.cpp ]
  PackageName  [ mem ]
  Synopsis     [ Define memory test commands ]
  Author       [ Chung-Yang (Ric) Huang ]
  Copyright    [ Copyleft(c) 2007-present LaDs(III), GIEE, NTU, Taiwan ]
****************************************************************************/
#include <iostream>
#include <iomanip>
#include "memCmd.h"
#include "memTest.h"
#include "cmdParser.h"
#include "util.h"

using namespace std;

extern MemTest mtest;  // defined in memTest.cpp

bool
initMemCmd()
{
   if (!(cmdMgr->regCmd("MTReset", 3, new MTResetCmd) &&
         cmdMgr->regCmd("MTNew", 3, new MTNewCmd) &&
         cmdMgr->regCmd("MTDelete", 3, new MTDeleteCmd) &&
         cmdMgr->regCmd("MTPrint", 3, new MTPrintCmd)
      )) {
      cerr << "Registering \"mem\" commands fails... exiting" << endl;
      return false;
   }
   return true;
}


//----------------------------------------------------------------------
//    MTReset [(size_t blockSize)]
//----------------------------------------------------------------------
CmdExecStatus
MTResetCmd::exec(const string& option)
{
   // check option
   string token;
   if (!CmdExec::lexSingleOption(option, token))
      return CMD_EXEC_ERROR;
   if (token.size()) {
      int b;
      if (!myStr2Int(token, b) || b < int(toSizeT(sizeof(MemTestObj)))) {
         cerr << "Illegal block size (" << token << ")!!" << endl;
         return CmdExec::errorOption(CMD_OPT_ILLEGAL, token);
      }
      #ifdef MEM_MGR_H
      mtest.reset(toSizeT(b));
      #else
      mtest.reset();
      #endif // MEM_MGR_H
   }
   else
      mtest.reset();
   return CMD_EXEC_DONE;
}

void
MTResetCmd::usage(ostream& os) const
{  
   os << "Usage: MTReset [(size_t blockSize)]" << endl;
}

void
MTResetCmd::help() const
{  
   cout << setw(15) << left << "MTReset: " 
        << "(memory test) reset memory manager" << endl;
}


//----------------------------------------------------------------------
//    MTNew <(size_t numObjects)> [-Array (size_t arraySize)]
//----------------------------------------------------------------------
CmdExecStatus
MTNewCmd::exec(const string& option)
{
   // TODO
   vector<string> options;
   CmdExec::lexOptions(option,options,0); 
   int num;
   int num1,num2;


   if(options.size()==0)
      return CmdExec::errorOption(CMD_OPT_MISSING, ""); 
   
   if(myStrNCmp("-Array", options[0], 2) == 0) 
   {
    if(options.size()==1) return CmdExec::errorOption(CMD_OPT_MISSING, options[0]);
    else if(options.size()>3) return CmdExec::errorOption(CMD_OPT_EXTRA,options[3]);
    else if(options.size()==2)
    {
      if(myStr2Int(options[1],num)) 
      {
        if(num<=0) CmdExec::errorOption(CMD_OPT_ILLEGAL,options[1]);
        return CmdExec::errorOption(CMD_OPT_MISSING, options[1]);
      }
      return CmdExec::errorOption(CMD_OPT_ILLEGAL,options[1]);
    }
    else
    {
      if((!myStr2Int(options[1],num1))||num1<=0) return CmdExec::errorOption(CMD_OPT_ILLEGAL,options[1]);
      else if((!myStr2Int(options[2],num2))||num2<=0) return CmdExec::errorOption(CMD_OPT_ILLEGAL,options[2]);
      else
      {
        mtest.newArrs(num2,num1);
        return CMD_EXEC_DONE;
      }
    }
   }
   else if(myStr2Int(options[0],num)) 
   {
      if(num<=0) 
      {
        return CmdExec::errorOption(CMD_OPT_ILLEGAL,options[1]);
      }

      else if(options.size()==1)
      {
        mtest.newObjs(num);
        return CMD_EXEC_DONE;
      }

      else if(myStrNCmp("-Array", options[1], 2) == 0)
      {
        if(myStr2Int(options[2],num2))
        {
          if(num2<=0) return CmdExec::errorOption(CMD_OPT_ILLEGAL,options[2]);
          else if(options.size()>3) return CmdExec::errorOption(CMD_OPT_EXTRA,options[3]);          
          else
          {
            mtest.newArrs(num,num2);
            return CMD_EXEC_DONE;
          }
        }
        return CmdExec::errorOption(CMD_OPT_ILLEGAL,options[2]);
      }
      else
      {
        return CmdExec::errorOption(CMD_OPT_EXTRA,options[1]);
      }
   }

   else
   {
      return CmdExec::errorOption(CMD_OPT_ILLEGAL,options[0]);
   }

}

void
MTNewCmd::usage(ostream& os) const
{  
   os << "Usage: MTNew <(size_t numObjects)> [-Array (size_t arraySize)]\n";
}

void
MTNewCmd::help() const
{  
   cout << setw(15) << left << "MTNew: " 
        << "(memory test) new objects" << endl;
}


//----------------------------------------------------------------------
//    MTDelete <-Index (size_t objId) | -Random (size_t numRandId)> [-Array]
//----------------------------------------------------------------------
CmdExecStatus
MTDeleteCmd::exec(const string& option)
{
   // TODO
   int num1,num2;
   vector<string> options;
   CmdExec::lexOptions(option,options,0); 

   if(options.size()==0)
      return CmdExec::errorOption(CMD_OPT_MISSING, ""); 
   


   if(myStrNCmp("-Index", options[0], 2) == 0)
   {
      if(options.size()==1)
      {
        return CmdExec::errorOption(CMD_OPT_MISSING,options[0]);
      }
    
      else if(myStr2Int(options[1],num1))
      {
        if(num1<0) return CmdExec::errorOption(CMD_OPT_ILLEGAL,options[0]);
        else if(options.size()==2)
        {
          if(num1>=mtest.getObjListSize())
          {
            cerr << "Size of object list ("<<mtest.getObjListSize()<<") is <= "<<options[1]<<"!!"<<endl;
            return CmdExec::errorOption(CMD_OPT_ILLEGAL,options[1]);
          }
          else
          {
            mtest.deleteObj(num1);
            return CMD_EXEC_DONE; 
          }
        }
        else if(options.size()>2)
        {
          if(myStrNCmp("-Array", options[2], 2)!=0)
          {
            return CmdExec::errorOption(CMD_OPT_ILLEGAL,options[2]);
          } 
          else
          {
            if(options.size()>3) return CmdExec::errorOption(CMD_OPT_ILLEGAL,options[3]);
            else if(num1>=mtest.getArrListSize())
            {
              cerr << "Size of array list ("<<mtest.getArrListSize()<<") is <= "<<options[1]<<"!!"<<endl;
              return CmdExec::errorOption(CMD_OPT_ILLEGAL,options[1]);
            } 
            else
            {
              mtest.deleteArr(num1);
              return CMD_EXEC_DONE;
            }
          }
        }
      }
      else
        return CmdExec::errorOption(CMD_OPT_ILLEGAL,options[1]); 
   }




   else if(myStrNCmp("-Random", options[0], 2) == 0)
   {
      if(options.size()==1)
      {
        return CmdExec::errorOption(CMD_OPT_MISSING,options[0]);
      }
    
      else if(myStr2Int(options[1],num1))
      {
        if(num1<=0) return CmdExec::errorOption(CMD_OPT_ILLEGAL,options[0]);
        else if(options.size()==2)
        {
          if(mtest.getObjListSize()==0)
          {
            cerr << "Size of object list is 0!!"<<endl;
            return CmdExec::errorOption(CMD_OPT_ILLEGAL,options[0]);
          }
          else
          {
            for(int i=0; i<num1;i++)
            {
              mtest.deleteObj(rnGen(mtest.getObjListSize()));
            } 
            return CMD_EXEC_DONE; 
          }
        }
        else if(options.size()>2)
        {
          if(myStrNCmp("-Array", options[2], 2)!=0)
          {
            return CmdExec::errorOption(CMD_OPT_ILLEGAL,options[2]);
          } 
          else
          {
            if(options.size()>3) return CmdExec::errorOption(CMD_OPT_ILLEGAL,options[3]);
            else if(mtest.getArrListSize()==0)
            {
              cerr << "Size of array list is 0!!"<<endl;
              return CmdExec::errorOption(CMD_OPT_ILLEGAL,options[0]);
            }  
            else
            {
              for(int i=0; i<num1; i++)
              {
                mtest.deleteArr(rnGen(mtest.getArrListSize()));
              } 
            return CMD_EXEC_DONE;
            }
          }
        }
      }
      else
        return CmdExec::errorOption(CMD_OPT_ILLEGAL,options[1]); 
   }







  else if(myStrNCmp("-Array", options[0], 2) == 0)
  {
    if(options.size()==1) return CmdExec::errorOption(CMD_OPT_MISSING,"");
    else if(myStrNCmp("-Index", options[1], 2)==0)
    {

      if(myStr2Int(options[2],num2))
      {
        if(num2<0) return CmdExec::errorOption(CMD_OPT_ILLEGAL,options[2]);
        else if(options.size()>3) return CmdExec::errorOption(CMD_OPT_ILLEGAL,options[3]);
        else
        {
          if(num2>=mtest.getArrListSize())
          {
            cerr << "Size of array list ("<<mtest.getArrListSize()<<") is <= "<<options[2]<<"!!"<<endl;
            return CmdExec::errorOption(CMD_OPT_ILLEGAL,options[2]);
          } 
          else
          {
            mtest.deleteArr(num2);
            return CMD_EXEC_DONE;
          }
        }
      }
      else
        return CmdExec::errorOption(CMD_OPT_ILLEGAL,options[2]);
    }
    else if(myStrNCmp("-Random", options[1], 2)==0) 
    {
      if(myStr2Int(options[2],num2))
      {
        if(num2<=0) return CmdExec::errorOption(CMD_OPT_ILLEGAL,options[2]);
        else if(options.size()>3) return CmdExec::errorOption(CMD_OPT_ILLEGAL,options[3]);
        else
        {
          if(mtest.getArrListSize()==0)
          {
            cerr << "Size of array list is 0!!"<<endl;
            return CmdExec::errorOption(CMD_OPT_ILLEGAL,options[1]);
          } 
          else
          {
           for(int i=0; i<num2;i++)
           {
            mtest.deleteArr(rnGen(mtest.getArrListSize()));
           } 
           return CMD_EXEC_DONE;
          }
        }
      }
      else
        return CmdExec::errorOption(CMD_OPT_ILLEGAL,options[2]);
    }
    else
      return CmdExec::errorOption(CMD_OPT_ILLEGAL,options[1]);
  }
  else
     return CmdExec::errorOption(CMD_OPT_ILLEGAL,options[0]); 

   
}

void
MTDeleteCmd::usage(ostream& os) const
{  
   os << "Usage: MTDelete <-Index (size_t objId) | "
      << "-Random (size_t numRandId)> [-Array]" << endl;
}

void
MTDeleteCmd::help() const
{  
   cout << setw(15) << left << "MTDelete: " 
        << "(memory test) delete objects" << endl;
}


//----------------------------------------------------------------------
//    MTPrint
//----------------------------------------------------------------------
CmdExecStatus
MTPrintCmd::exec(const string& option)
{
   // check option
   if (option.size())
      return CmdExec::errorOption(CMD_OPT_EXTRA, option);
   mtest.print();

   return CMD_EXEC_DONE;
}

void
MTPrintCmd::usage(ostream& os) const
{  
   os << "Usage: MTPrint" << endl;
}

void
MTPrintCmd::help() const
{  
   cout << setw(15) << left << "MTPrint: " 
        << "(memory test) print memory manager info" << endl;
}



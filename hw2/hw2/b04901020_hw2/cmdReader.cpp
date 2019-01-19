/****************************************************************************
  FileName     [ cmdReader.cpp ]
  PackageName  [ cmd ]
  Synopsis     [ Define command line reader member functions ]
  Author       [ Chung-Yang (Ric) Huang ]
  Copyright    [ Copyleft(c) 2007-present LaDs(III), GIEE, NTU, Taiwan ]
****************************************************************************/
#include <cassert>
#include <cstring>
#include "cmdParser.h"

using namespace std;

//----------------------------------------------------------------------
//    Extrenal funcitons
//----------------------------------------------------------------------
void mybeep();
char mygetc(istream&);
ParseChar getChar(istream&);


//----------------------------------------------------------------------
//    Member Function for class Parser
//----------------------------------------------------------------------
void
CmdParser::readCmd()
{
   if (_dofile.is_open()) {
      readCmdInt(_dofile);
      _dofile.close();
   }
   else
      readCmdInt(cin);
}

void
CmdParser::readCmdInt(istream& istr)
{
   resetBufAndPrintPrompt();

   while (1) {
      ParseChar pch = getChar(istr);
      if (pch == INPUT_END_KEY) break;
      switch (pch) {
         case LINE_BEGIN_KEY : moveBufPtr(_readBuf); break;
         case HOME_KEY       : moveBufPtr(_readBuf); break;
         case LINE_END_KEY   : moveBufPtr(_readBufEnd); break;
         case END_KEY        : moveBufPtr(_readBufEnd); break;
         case BACK_SPACE_KEY : /* TODO */ if(moveBufPtr(_readBufPtr - 1)==true){deleteChar();}break;
         case DELETE_KEY     : deleteChar(); break;
         case NEWLINE_KEY    : cout << char(NEWLINE_KEY);
         					   addHistory(); 
         					   deleteLine();
							   break;
         case ARROW_UP_KEY   : moveToHistory(_historyIdx - 1); break;
         case ARROW_DOWN_KEY : moveToHistory(_historyIdx + 1); break;
         case ARROW_RIGHT_KEY: /* TODO */moveBufPtr(_readBufPtr + 1);  break;
         case ARROW_LEFT_KEY : /* TODO */moveBufPtr(_readBufPtr - 1);  break;
         case PG_UP_KEY      : moveToHistory(_historyIdx - PG_OFFSET); break;
         case PG_DOWN_KEY    : moveToHistory(_historyIdx + PG_OFFSET); break;
         case TAB_KEY        : /* TODO */
         int i;
         int num;
         i=_readBufPtr-&(_readBuf[0]);
         num=i/8;
         insertChar(' ',(num+1)*8-i);
         break;
         case INSERT_KEY     : // not yet supported; fall through to UNDEFINE
         case UNDEFINED_KEY:   mybeep(); break;
         default:  // printable character
            insertChar(char(pch)); break;
      }
      #ifdef TA_KB_SETTING
      taTestOnly();
      #endif
   }
}


// This function moves _readBufPtr to the "ptr" pointer
// It is used by left/right arrowkeys, home/end, etc.
//
// Suggested steps:
// 1. Make sure ptr is within [_readBuf, _readBufEnd].
//    If not, make a beep sound and return false. (DON'T MOVE)
// 2. Move the cursor to the left or right, depending on ptr
// 3. Update _readBufPtr accordingly. The content of the _readBuf[] will
//    not be changed
//
// [Note] This function can also be called by other member functions below
//        to move the _readBufPtr to proper position.
bool
CmdParser::moveBufPtr(char* const ptr)
{
   // TODO...   				
  if((ptr>=_readBufEnd&&_readBufPtr==_readBufEnd)||(ptr<=_readBuf&&_readBufPtr==_readBuf))
  {

    mybeep();
    return false;
  }
  while(_readBufPtr>ptr)
  {
    cout <<"\b";
    _readBufPtr--;
  }
  while(_readBufPtr<ptr)
  {
    cout << *_readBufPtr;
    _readBufPtr++;
  }
   return true;
}


// [Notes]
// 1. Delete the char at _readBufPtr
// 2. mybeep() and return false if at _readBufEnd
// 3. Move the remaining string left for one character
// 4. The cursor should stay at the same position
// 5. Remember to update _readBufEnd accordingly.
// 6. Don't leave the tailing character.
// 7. Call "moveBufPtr(...)" if needed.
//
// For example,
//
// cmd> This is the command
//              ^                (^ is the cursor position)
//
// After calling deleteChar()---
//
// cmd> This is he command
//              ^
//
bool
CmdParser::deleteChar()
{
   int ptr=0;
   int fornum=0;
   int backnum=0;

   if(_readBufPtr==_readBufEnd)
   {
   	mybeep(); return false;
   }



   while(&(_readBuf[ptr])!=_readBufPtr)
   {
    ptr++;
    fornum++;
   }
   while(&(_readBuf[ptr])!=_readBufEnd)
   {
    ptr++;
    backnum++;
   }

   for(int i=fornum;i<fornum+backnum-1;i++)
   {
    _readBuf[i]=_readBuf[i+1];
   }
   
   _readBuf[fornum+backnum-1]=' ';
   _readBufEnd--;
   

   for(int i=fornum;i<fornum+backnum;i++)
   {
    cout<<_readBuf[i];
   }

   *_readBufEnd='\0';

   for(int i=0;i<backnum;i++)
   {
    cout<<"\b";
   }	
   // TODO...
   return true;
}


// 1. Insert character 'ch' for "repeat" times at _readBufPtr
// 2. Move the remaining string right for "repeat" characters
// 3. The cursor should move right for "repeats" positions afterwards
// 4. Default value for "repeat" is 1. You should assert that (repeat >= 1).
//
// For example,
//
// cmd> This is the command
//              ^                (^ is the cursor position)
//
// After calling insertChar('k', 3) ---
//
// cmd> This is kkkthe command
//                 ^
//
void
CmdParser::insertChar(char ch, int repeat)
{
   // TODO...
   assert(repeat >= 1);
   int ptr=0;
   int fornum=0;
   int backnum=0;
   while(&(_readBuf[ptr])!=_readBufPtr)
   {
    ptr++;
    fornum++;
   }
   while(&(_readBuf[ptr])!=_readBufEnd)
   {
    ptr++;
    backnum++;
   }

   for(int i=fornum+backnum+repeat-1;i>fornum+repeat-1;i--)
   {
    _readBuf[i]=_readBuf[i-repeat];
   }
   
   for(int i=0;i<repeat;i++)
   {
   _readBuf[fornum+i]=ch;
   _readBufEnd++;
   _readBufPtr++;
   }
   *_readBufEnd='\0';

   for(int i=fornum;i<fornum+backnum+repeat;i++)
   {
    cout<<_readBuf[i];
   }   

   for(int i=0;i<backnum;i++)
   {
    cout<<"\b";
   }
}

// 1. Delete the line that is currently shown on the screen
// 2. Reset _readBufPtr and _readBufEnd to _readBuf
// 3. Make sure *_readBufEnd = 0
//
// For example,
//
// cmd> This is the command
//              ^                (^ is the cursor position)
//
// After calling deleteLine() ---
//
// cmd>
//      ^
//
void
CmdParser::deleteLine()
{	
	cout<<"\033[2K\r";
	resetBufAndPrintPrompt();
   // TODO...
}


// This functions moves _historyIdx to index and display _history[index]
// on the screen.
//
// Need to consider:
// If moving up... (i.e. index < _historyIdx)
// 1. If already at top (i.e. _historyIdx == 0), beep and do nothing.
// 2. If at bottom, temporarily record _readBuf to history.
//    (Do not remove spaces, and set _tempCmdStored to "true")
// 3. If index < 0, let index = 0.
//
// If moving down... (i.e. index > _historyIdx)
// 1. If already at bottom, beep and do nothing
// 2. If index >= _history.size(), let index = _history.size() - 1.
//
// Assign  index to _historyIdx at the end.
//
// [Note] index should not = _historyIdx
//
void
CmdParser::moveToHistory(int index)
{
// TODO...
	if(_history.size()==0)
	{
	 mybeep();
	 return;
	}

	else if(index < _historyIdx) //up
	{   
		if(_historyIdx==0){mybeep();return;}
		if(index==_history.size()-1||((index==_history.size()-10)&&_historyIdx!=index+1))
		{
			_tempCmdStored=true;
			addHistory();			
		}
		if(index<0&&_historyIdx>=0)index=0;
	}
	

	else if(index > _historyIdx) //down
	{	

		if(_historyIdx==_history.size()){mybeep();return;}
		if(index>=_history.size()&&_historyIdx<_history.size()){index=_history.size()-1;}
		if(index==_history.size()-1)
		{	
			_historyIdx=index;
			retrieveHistory();
			_history.pop_back();
			_tempCmdStored=false;
			return;
		}
	}
	_historyIdx=index;
	retrieveHistory();
}


// This function adds the string in _readBuf to the _history.
// The size of _history may or may not change. Depending on whether 
// there is a temp history string.
//
// 1. Remove ' ' at the beginning and end of _readBuf
// 2. If not a null string, add string to _history.
//    Be sure you are adding to the right entry of _history.
// 3. If it is a null string, don't add anything to _history.
// 4. Make sure to clean up "temp recorded string" (added earlier by up/pgUp,
//    and reset _tempCmdStored to false
// 5. Reset _historyIdx to _history.size() // for future insertion
//
void
CmdParser::addHistory()
{	
	if(!(_historyIdx==_history.size()&&_tempCmdStored==true))
	{
		if(_readBuf!=_readBufPtr)
		{	
		 bool key=1;
		 while(key==1)
		 {
			moveBufPtr(_readBuf);
			while(_readBufPtr!=_readBufEnd)  
			{
				if(*_readBufPtr!=' ')
				{
					break;
				}
				deleteChar();
			}
			if(&(_readBuf[0])==_readBufEnd){break;}
			moveBufPtr(_readBufEnd);
			while(_readBufPtr!=_readBuf)
			{	
				if(*(_readBufPtr-1)!=' ')
				{	   
					break;
				}
				moveBufPtr(_readBufPtr - 1);
				deleteChar();
			}
			key=0;
		 }

 		}
 	}

	string temp(_readBuf);

	if(_tempCmdStored==true)
	{
		if(_historyIdx==_history.size())
		{
			_history.push_back(temp);
		}
		else
		{
			_history.pop_back();
			if(temp==""){_historyIdx=_history.size();return;}
			_history.push_back(temp);
			_tempCmdStored=false;
			_historyIdx=_history.size();
			return;
		}
	}
	else
	{
		if(temp=="")
		{
			_historyIdx=_history.size();
			return;
		}
		else
		{
			_history.push_back(temp);
			_tempCmdStored=false;
		}
	}
	_historyIdx=_history.size();
 }
   // TODO...


// 1. Replace current line with _history[_historyIdx] on the screen
// 2. Set _readBufPtr and _readBufEnd to end of line
//
// [Note] Do not change _history.size().
//
void
CmdParser::retrieveHistory()
{
   deleteLine();
   strcpy(_readBuf, _history[_historyIdx].c_str());
   cout << _readBuf;
   _readBufPtr = _readBufEnd = _readBuf + _history[_historyIdx].size();

}

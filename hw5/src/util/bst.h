/****************************************************************************
  FileName     [ bst.h ]
  PackageName  [ util ]
  Synopsis     [ Define binary search tree package ]
  Author       [ Chung-Yang (Ric) Huang ]
  Copyright    [ Copyleft(c) 2005-present LaDs(III), GIEE, NTU, Taiwan ]
****************************************************************************/

#ifndef BST_H
#define BST_H

#include <cassert>

using namespace std;

template <class T> class BSTree;

// BSTreeNode is supposed to be a private class. User don't need to see it.
// Only BSTree and BSTree::iterator can access it.
//
// DO NOT add any public data member or function to this class!!
//
template <class T>
class BSTreeNode
{
	friend class BSTree<T>;
	friend class BSTree<T>::iterator;
   	// TODO: design your own class!!
	BSTreeNode(const T& d, BSTreeNode* p=0, BSTreeNode* l=0, BSTreeNode* r=0 ):
			_data(d),_parent(p),_left(l),_right(r){}

	T              _data;
	BSTreeNode<T>* _parent; 
	BSTreeNode<T>* _left;
	BSTreeNode<T>* _right;
	// update in insert and erase and deleting min()/successor()
};


template <class T>
class BSTree
{
   // TODO: design your own class!!

public:
	BSTree(){
		_tail = new BSTreeNode<T>(T(""));
	}
	~BSTree(){clear();  delete _tail;}



   class iterator 
   { 
   		friend class BSTree;

   	public:
      iterator(BSTreeNode<T>* n=0):  _node(n) {}
      iterator(const iterator& i) :  _node(i._node) {}
      ~iterator() {}

      const T& operator * () const { return _node->_data; }
      T& operator * () { return _node->_data; }
      
      iterator& operator ++ () 
      { 
      	BSTreeNode<T>* temp=_node;
      	if(_node->_right)
      	{
			_node=min(_node->_right);      		
      		return *this;
      	}
      	else 
      	{
      		while(_node->_parent->_left!=_node)
      		{
      			_node=_node->_parent;
      			if(_node==NULL)      
      			{
      				_node=temp;
      				break;
      			}			
      		}
      		if(_node->_parent->_left==_node)
   		   		_node=_node->_parent;
      		return *this;
      	}
      }
      
      iterator& operator -- () 
      {
      	BSTreeNode<T>* temp=_node;
      	if(_node->_left)
      	{
      		_node=max(_node->_left);
      		return *this;
      	}
      	else
      	{
      		if(_node->_parent==NULL)
      		{	
      			return *this;
      		}
      		while(_node->_parent->_right!=_node)
      		{
      			_node=_node->_parent;
      			if(_node->_parent==NULL)
      			{
      				_node=temp;
      				break;
      			}
      		}
      		if(_node->_parent->_right==_node)
      			_node=_node->_parent;
      		return *this;
      	}
      } //bring back to the last element
      iterator operator ++ (int) {iterator temp(*this); operator++(); return temp; }
      iterator operator -- (int) {iterator temp(*this); operator--(); return temp; }

      iterator& operator = (const iterator& i) { _node=i._node; return *this;}
      bool operator != (const iterator& i) const { return i._node!=_node; }
      bool operator == (const iterator& i) const { return i._node==_node; }

     	BSTreeNode<T>* min (BSTreeNode<T>* node) const
 	   	{
   		if(node->_left==NULL)
   			return node;
   		else
   			return min(node->_left);
   		}

   		BSTreeNode<T>* max (BSTreeNode<T>* node) const
   		{
   		if(node->_right==NULL)
   			return node;
   		else
   			return max(node->_right);
   		}

   		BSTreeNode<T>* successor (BSTreeNode<T>* node) const // the minimum node of the right tree
   		{
   		if(node->_right==NULL)
   			return NULL;
   		else
   			return min(node->_right);
   		}

   	private:
   	  BSTreeNode<T>* _node;
   };




   iterator begin() const
   {
   	if(empty()) return end();
   	iterator temp(min(_root));
   	return temp;
   } // leftmost element smallest update after insert and erase find min!
   
   iterator end() const
   {
   	iterator temp(_tail);
   	return temp;
   } // design on our own(--operator bring the iterator to the last element)
   
   bool empty() const{return !_root;} 
   size_t size() const{return size(_root)-1;}
   void insert(const T& x)
   {
   	if(empty())
   	{
   		_root = new BSTreeNode<T>(x,NULL,NULL,_tail);
   		_tail->_parent=_root;
   	}	
   	else
   	{
   		insert(_root,x);
   	}


   }// maintain the order 

   void pop_back() 
   {
   	erase(--end());
   } //remove the rightmost

   void pop_front() 
   {
   	erase(begin());
   } //remove the leftmost 

   bool erase(iterator pos)
   {

   	if(empty())
  	 	return false;

   	if(_tail)
   	{
   		_tail->_parent->_right=NULL;
   		delete _tail;
   		_tail=NULL;
   	}


  	if(successor(pos._node)==NULL)
  	{
  	 	if(pos._node->_left==NULL)
  	 	{
  	 		if(pos._node==_root)
  	 		{
  	 			//cout<<"11"<<endl;
  	 			clear();
  	 			return true;
  	 		}
  	 		else if(pos._node->_parent->_left==pos._node)
  	 		{
  	 			//cout<<"22"<<endl;
  	 			pos._node->_parent->_left=NULL;	
  	 		}
  	 		else
  	 		{
  	 			//cout<<"33"<<endl;
  	 			pos._node->_parent->_right=NULL;
  	 		}
  	 		delete pos._node;
  	 		pos._node=NULL;
  	 	}
  	 	else
  	 	{
  	 		if(pos._node==_root)
  	 		{
  	 			//cout<<"44"<<endl;
  	 			BSTreeNode<T>* temp=_root;
				_root=_root->_left;
				delete temp;
  	 		}
  	 		else
  	 		{

  	 			if(pos._node->_parent->_left==pos._node)
  	 			{
  	 				//cout<<"55"<<endl;
  	 				pos._node->_parent->_left=pos._node->_left;
  	 				pos._node->_left->_parent=pos._node->_parent;
  	 			}
  	 			else
  	 			{
  	 				//cout<<"66"<<endl;
  	 				pos._node->_parent->_right=pos._node->_left;
  	 				pos._node->_left->_parent=pos._node->_parent;
  	 			}
  	 			delete pos._node;
  	 			pos._node=NULL;
  	 		}
  	 	}
  	 }
  	 else
  	 {
  	 	pos._node->_data=successor(pos._node)->_data;
  	 	erase(successor(pos._node));
  	 	
  	 	if(_tail)
   		{
   			_tail->_parent->_right=NULL;
   			delete _tail;
   			_tail=NULL;
   		}
  	 }
	 

  	 if(!_tail)
  	 {
  	 	_tail=new BSTreeNode<T>(T(""),max(_root));
  	 	max(_root)->_right=_tail;
  	 }

  	 return true;
   } // return false if empty 

   bool erase(const T& x) 
   {
   	for(iterator li(begin());li!=end();li++)
   	{
   		if(li._node->_data==x)
   		{
   			erase(li);
   			return true;
   		}
   	}
   	return false;
   } //return false if not exist
   

   void clear()
   {
   	
   	if(empty())return;
   	clean(_root);
   	_tail = new BSTreeNode<T>(T(""));
   	
   }  //delete all BSTreeNode but not dummy BSTreeNode


   void sort() const {}

   void print() const { debug(" ",_root);}

   void debug(string t,BSTreeNode<T>* n) const
   {
    	if(n!=_tail){
     		cerr<<t<<n->_data<<endl;
     		if(n->_left!=0 && n->_left!=_tail)  debug(t+"  ",n->_left);
     		else cerr<<(t+"  ")<<"[0]"<<endl;
     		if(n->_right!=0 && n->_right!=_tail)  debug(t+"  ",n->_right);
     		else cerr<<(t+"  ")<<"[0]"<<endl;
   		}
   		else cerr<<t<<"[0]"<<endl;
   	}


   BSTreeNode<T>* min (BSTreeNode<T>* node) const
   {
   	if(node->_left==NULL)
   		return node;
   	else
   		return min(node->_left);
   }

   BSTreeNode<T>* max (BSTreeNode<T>* node) const
   {
   	if(node->_right==NULL)
   		return node;
   	else
   		return max(node->_right);
   }

   BSTreeNode<T>* successor (BSTreeNode<T>* node) const // the minimum node of the right tree
   {
   	if(node->_right==NULL)
   		return NULL;
   	else
   		return min(node->_right);
   }


private:
	size_t size (BSTreeNode<T>* node) const
	{
		if(node==NULL)
			return 0;
		return size(node->_left)+size(node->_right)+1;
	}
	void clean(BSTreeNode<T>*& node)
	{
		if(node->_left!=NULL)
			clean(node->_left);
		if(node->_right!=NULL)
			clean(node->_right);
		delete node;
		node=NULL;
	}
	void insert(BSTreeNode<T>* node, const T& x)
	{
		if(x<=node->_data)
		{
			if(node->_left==NULL)
			{
			  node->_left = new BSTreeNode<T>(x,node);
			}
			else
			  insert(node->_left,x);
		}
		else
		{
			if(node->_right==NULL)
			  node->_right = new BSTreeNode<T>(x,node);
			else if(node->_right==_tail)
			{
			  node->_right = new BSTreeNode<T>(x,node,NULL,_tail);
			  _tail->_parent = node->_right;
			}
			else
			  insert(node->_right,x);
		}
	}

	BSTreeNode<T>*	_root;
  BSTreeNode<T>*  _tail;   // update in insert and erase  //dummy node
	

   
};

#endif // BST_H

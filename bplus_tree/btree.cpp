// with keys and pointers stored seperatley
#include<iostream>
#include <list>
#include <algorithm>
#include <cmath>
#include <fstream>
#include <string>
#include <string.h>
using namespace std;
 
// A BTree node
class BTreeNode
{
public:
    list<long long> keys;  // An array of keys
    list<BTreeNode*> childPtrs; // An array of child pointers
    bool leaf; // Is true when node is leaf. Otherwise false
    BTreeNode *leafNxtPtr;
    BTreeNode *parentPtr;
    
};

BTreeNode *root = NULL;
unsigned ovf = 0;

BTreeNode* search(long long key) {

	// begin from root node
	BTreeNode *curr;
	curr = root;
	list<long long>::iterator it;

	while(!curr->leaf) {
		// get the child to be traversed
		it = upper_bound (curr->keys.begin(), curr->keys.end(), key);
		int id = distance(curr->keys.begin(), it);	

		// set curr to child
		int i=0;
		for (auto &x : curr->childPtrs) {
			if(i==id) { curr = x; break;}
			i++;
		}
	}

	return curr;

}

BTreeNode* search_cnt(long long key) {

	// begin from root node
	BTreeNode *curr;
	curr = root;
	list<long long>::iterator it;

	while(!curr->leaf) {
		// get the child to be traversed
		it = lower_bound (curr->keys.begin(), curr->keys.end(), key);
		int id = distance(curr->keys.begin(), it);	
		
		// set curr to child
		int i=0;
		for (auto &x : curr->childPtrs) {
			if(i==id) { curr = x; break;}
			i++;
		}
	}

	return curr;

}


void splitNonLeafNode(BTreeNode *curr) {

	// cout << "splitting non-leaf node" << endl;
	// create a new nodes
	BTreeNode *newNode = new BTreeNode();

	// splice the key list 
	int mid_pt = curr->keys.size()/2;
	std::list<long long>::iterator it = curr->keys.begin();
	std::advance(it,mid_pt); long long mid_val = *it;
	newNode->keys.splice(newNode->keys.begin(), curr->keys, it, curr->keys.end());
	
	// splice the childPtrs list 
	std::list<BTreeNode*>::iterator itc = curr->childPtrs.begin();
	std::advance(itc,mid_pt+1);
	newNode->childPtrs.splice(newNode->childPtrs.begin(), curr->childPtrs, itc, curr->childPtrs.end());
	newNode->childPtrs.push_front (curr->childPtrs.back()); // both the pointers pointing to the same node
	
	newNode->leaf = false;  ; curr->leafNxtPtr = NULL;

	//updating the childPtrs->parentPtr
	itc = newNode->childPtrs.begin();
	advance(itc,1);
	BTreeNode *temp;
	for (list<BTreeNode*>::iterator it = itc; it!=newNode->childPtrs.end(); ++it) {
    	temp = *it;
    	temp->parentPtr = newNode;
	}


	// splitting root node
	if (curr->parentPtr == NULL) {
		BTreeNode *rootNode = new BTreeNode();
		rootNode->keys.push_back(mid_val);	
		rootNode->childPtrs.push_back(curr);
		rootNode->childPtrs.push_back(newNode);
		rootNode->leaf = false;
		rootNode->leafNxtPtr = NULL;
    	rootNode->parentPtr = NULL;
    	curr->parentPtr = newNode->parentPtr = rootNode;	
    	root = rootNode;
	} else {

		// assigning the parent of the newly created node
		newNode->parentPtr = curr->parentPtr;

		// find the lowerbound position to insert in the keys
		it = upper_bound (curr->parentPtr->keys.begin(), curr->parentPtr->keys.end(), mid_val);
		int id = distance(curr->parentPtr->keys.begin(), it);
		curr->parentPtr->keys.insert(it,mid_val);

		// insert the address of new node in the i+1 position of childptrs
		list<BTreeNode*>::iterator it2 = curr->parentPtr->childPtrs.begin();
		advance(it2,id+1);
		curr->parentPtr->childPtrs.insert(it2,newNode);

		if(curr->parentPtr->keys.size() >= ovf)
			splitNonLeafNode(curr->parentPtr);
		
	}

	
}
// if, there is no pointer to parent node then create a new node obj
// copy the median into the obj and assign the key value and pointers
// assign the parentPtr accordingly, exit
// implies this is the first split, so curr node should be set as leaf node
// and the childPtrs should be set to null
// else, check for ovf in parent node and linke the new node to curr nodes parent

void splitLeafNode(BTreeNode *curr) {

	// cout << "splitting leaf node" << endl;
	// create a new nodes
	BTreeNode *newNode = new BTreeNode();

	// splice the keys and cptrs list accordingly
	int mid_pt = curr->keys.size()/2;
	std::list<long long>::iterator it = curr->keys.begin();
	std::advance(it,mid_pt); long long mid_val = *it;
	newNode->keys.splice(newNode->keys.begin(), curr->keys, it, curr->keys.end());
	newNode->leaf = true; newNode->childPtrs.push_back(NULL); newNode->leafNxtPtr = curr->leafNxtPtr; curr->leafNxtPtr = newNode; 

	// splitting root node which is a leaf node as well
	if (curr->parentPtr == NULL) {
		BTreeNode *rootNode = new BTreeNode();
		rootNode->keys.push_back(mid_val);	
		rootNode->childPtrs.push_back(curr);
		rootNode->childPtrs.push_back(newNode);
		rootNode->leaf = false;
		rootNode->leafNxtPtr = NULL;
    	rootNode->parentPtr = NULL;
    	curr->parentPtr = newNode->parentPtr = rootNode;
    	root = rootNode;
	} else {

		// assigning the parent of the newly created node
		newNode->parentPtr = curr->parentPtr;

		// find the lowerbound position to insert in the keys
		list<long long>::iterator it = upper_bound (curr->parentPtr->keys.begin(), curr->parentPtr->keys.end(), mid_val);
		int id = distance(curr->parentPtr->keys.begin(), it);
		curr->parentPtr->keys.insert(it,mid_val);

		// insert the address of new node in the id+1 position of childptrs
		list<BTreeNode*>::iterator it2 = curr->parentPtr->childPtrs.begin();
		advance(it2,id+1);
		curr->parentPtr->childPtrs.insert(it2,newNode);

		if (curr->parentPtr->keys.size() >= ovf) {
			splitNonLeafNode(curr->parentPtr);
		}
	}

}

void createRoot(long long k) {
	// cout << "Creating root node with : " << k <<endl;
	BTreeNode *newNode = new BTreeNode();
	newNode->keys.push_back(k);
	newNode->childPtrs.push_back(NULL);
    newNode->leaf = true;
    newNode->leafNxtPtr = NULL;
    newNode->parentPtr = NULL;
    root = newNode;
}

int insertKey(long long k) {

	// cout << "Inserting key "<< k<< endl;
	
	// if root is not present then create root node
	if(root == NULL) {
		createRoot(k);
		return 0;
	}

	// traverse to the corresponding leaf node and add an entry into the leaf node
	BTreeNode* node = search(k);
	// printf("inserting key %d at node containg elemts: ",k);
	// for (auto &x : node->keys) {
	// 	cout << x << ", ";
	// }cout <<endl;
	std::list<long long>::iterator it = upper_bound (node->keys.begin(), node->keys.end(), k);
	node->keys.insert(it,k);

	// if ovf=5(deg) then split node (recursivley uptill the root node if necessary)
	if (node->keys.size() >= ovf) {
		splitLeafNode(node);
	}

	return 0;
}





void dispNode(BTreeNode *node) {

	for (auto &x : node->keys) {
    	cout << "|" << x << "|";
	}

}

void dispTree() {

	list<BTreeNode*> temp, temp2;
	

	dispNode(root);
	cout << endl << "----------------------" << endl;
	if(root->childPtrs.front() == NULL)
		return;

	for (auto &x : root->childPtrs) {
			temp.push_back(x);
	}
	
	while(1) {

		if (!temp.empty()) {
			BTreeNode *node = temp.front();
			temp.pop_front();
			dispNode(node);	
			for (auto &x : node->childPtrs) {
				if (x != NULL)
					temp2.push_back(x);	
			}
	
		} else {
			if(!temp2.empty()) {
				temp.splice(temp.end(), temp2);
				cout << endl << "----------------------" << endl;	
			} else {
				break;
			}
			
		}
		
	}

}


void find_elem(long long key) {
	
	if(root == NULL) {cout << "NO" << endl; return;}

	BTreeNode *child = search(key);

	for(auto &x : child->keys) {
		if(x == key) {
			cout << "YES"<< endl;
			return;
		}
	}

	cout << "NO"<< endl;

}

void count_elem (long long key){

	if(root == NULL) {cout << 0<< endl; return;}
	
	BTreeNode *child = search_cnt(key);

	int cnt=0;
	
	while(1) {

		for(auto &x : child->keys) {
			// cout << x<<", ";
			if (x <= key) {
				if(x == key)
					cnt++;
			} else {
				cout << cnt<< endl;
				return;
			}
		}
		
		child = child->leafNxtPtr;
		if(child == NULL){cout << cnt << endl; break;}
		
	}

}




void find_range (long long strt, long long end) {

	if(root == NULL) {cout << 0; return;}

	BTreeNode *child = search(strt);
	int cnt=0;
	while(1) {

		for(auto &x : child->keys) {
			
			if ( x >= strt && x <= end) {
				cnt++;
			} else if(x>end){
				cout << cnt<< endl;
				return;
			}
		}

		child = child->leafNxtPtr;
		if(child == NULL){cout <<cnt<< endl; break;}
	}


}

int main(int argc, char **argv) {

	// int mem_sz = atoi(argv[1]);

	int blk_sz = atoi(argv[2]); // 40 => ovf/degree=3

	//formula modified since we are using long long
	int node_sz = floor((blk_sz - 8)/16.0);
	
	if (node_sz == 0 || argc < 2) {
		printf("Node size is invalid, unable to form tree. \n");
		exit(0);
	}
		
	
	ovf = node_sz + 1;

	std::ifstream infile("input3.txt");

	string line;

	while (getline(infile, line))
	{
	    cout << line << ": ";
  		if (line.find("FIND")!=string::npos) {

  			long long temp = stoll(line.substr(4));
  			find_elem(temp);	

  		}
  		else if(line.find("RANGE")!=string::npos) {
  			
  			int ws = line.find(" ", 6);
  			long long strt = stoll(line.substr(6,ws));
  			long long end = stoll(line.substr(ws));
  			find_range(strt, end);
			
			
  		} else if(line.find("INSERT")!=string::npos) {
  			long long temp = stoll(line.substr(6));
  			insertKey(temp);
  			cout << "OK"<<endl;

  		} else if(line.find("COUNT")!=string::npos) {
  			long long temp = stoll(line.substr(5));
  			count_elem(temp);
  			
  		}
	}

	// dispTree();

	return 0;
}


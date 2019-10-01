#ifndef TRIE_INCLUDED
#define TRIE_INCLUDED

#include <string>
#include <vector>
#include <iostream>
using namespace std; 

template<typename ValueType>

class Trie
{
public:
	Trie() {
		root = new Node; 
	}
	~Trie() { //use a post order traversal
		forestFire(root); 
		delete root; 
	}

	struct Node; //forward class declaration! 


	void reset() //call destructor here
	{
		~Trie(); 
		root = new Node; 
	}
	void insert(const std::string& key, const ValueType& value) {
		Node* ptr = root; 
		
		for (int i = 0; i < key.size(); i++) {
			//if ptr children contains first letter in string
			bool contains = false;
			for (typename vector<Node*>::iterator it = ptr->children.begin(); it != ptr->children.end(); ++it) {
				if ((*it)->label == key[i]) { //go down that path
					ptr = *it; 
					contains = true; 
					break;
				}
			}

			if (!contains) {
				//else, make a new node 
				Node* additional = new Node;
				additional->label = key[i];
				ptr->children.push_back(additional); //invalidates all iterators 
				ptr = additional;
			}
		}
		//now we've traversed through the string, add the value
		ptr->values.push_back(value); 
	}

	vector<ValueType> find(const string& key, bool exactMatchOnly) const {
		vector<ValueType> results; 
		Node* ptr = nullptr; 
		for (typename vector<Node*>::iterator it = root->children.begin(); it != root->children.end(); ++it) {
			if ((*it)->label == key[0] && key.size() == 1)
				return (*it)->values;
			else if ((*it)->label == key[0]) {
				ptr = *it; 
			}
		}
		if (ptr == nullptr){ //aka first char isn't there
			return results;
		}
		return findHelper(key.substr(1), exactMatchOnly, ptr);
	}

	
	
	


	// C++11 syntax for preventing copying and assignment
	Trie(const Trie&) = default;
	Trie& operator=(const Trie&) = default;
private:
	void forestFire(Node* cur) { //post order traversal of trie
	//TODO: IMPLEMENT DESTRUCTOR
		if (cur->children.empty())
			return; //INVALIDATES THE DESTRUCTOR ITERATOR!!!


		typename vector<Node*>::iterator it = cur->children.begin(); 
		while (it != cur->children.end()) {
			forestFire(*it); 
			delete *it; 
			it = cur->children.erase(it); 
		}
	}

	vector<ValueType> findHelper(const string key, bool exactMatchOnly, Node* leaf) const {
		vector<ValueType> numbers;
		if (key.size() == 1) { //base case
			for (typename vector<Node*>::iterator it = leaf->children.begin(); it != leaf->children.end(); ++it) {
				if (exactMatchOnly) {
					if ((*it)->label == key[0]) {

						numbers.insert(numbers.end(), (*it)->values.begin(), (*it)->values.end());
					}
				}
				else
					numbers.insert(numbers.end(), (*it)->values.begin(), (*it)->values.end());
			}
		}
		else {
			//find key[0]
			for (typename vector<Node*>::iterator it = leaf->children.begin(); it != leaf->children.end(); ++it) { //we always run this loop
				if (!exactMatchOnly && (*it)->label != key[0]) { //this only runs if we're feeling lucky
					vector<ValueType> temp = findHelper(key.substr(1), true, *it); //might end after 1 loopage 
					numbers.insert(numbers.end(), temp.begin(), temp.end());

				}
				else if ((*it)->label == key[0]) { //we run this regardless of bool, since we have no choice
					vector<ValueType> temp = findHelper(key.substr(1), exactMatchOnly, *it);
					numbers.insert(numbers.end(), temp.begin(), temp.end());
				}

			}
		}
		return numbers;
	}
	struct Node {
		char label;
		vector<ValueType> values; 	//vector will hold the values 
		vector<Node*> children;//vector of Node* pointers will hold the pointers, accordingly 
	};
	Node* root; //root node of the Trie 

};



#endif // TRIE_INCLUDED

#ifndef SET_H
#define SET_H //include guards 




#include <string>

using ItemType = std::string;

class Set
{
public:
	Set();
	bool empty() const;
	int size() const;
	bool insert(const ItemType& value);
	// Insert value into the set if it is not already present.  Return
  // true if the value was actually inserted.  Leave the set unchanged
  // and return false if the value was not inserted (perhaps because it
  // was already in the set or because the set has a fixed capacity and
  // is full).
	bool erase(const ItemType& value);
	// Remove the value from the set if present.  Return true if the
// value was removed; otherwise, leave the set unchanged and
// return false.
	bool contains(const ItemType& value) const;
	// Return true if the value is in the set, otherwise false.

	bool get(int pos, ItemType& value) const;
	// If 0 <= i < size(), copy into value the item in the set that is
// strictly greater than exactly i items in the set and return true.
// Otherwise, leave value unchanged and return false.
	void swap(Set& other);
	// Exchange the contents of this set with the other one.

	//void dump() const; //prints the list 

	//'housekeeping' functions, as Smallberg would say
	~Set(); //destructor 
	//When a Set is destroyed, the nodes in the linked list must be deallocated.

	Set(const Set& other); //copy constructor 
	//When a brand new Set is created as a copy of an existing Set, enough new nodes must be allocated to hold a duplicate of the original list.

	Set& operator=(const Set& rhs); //assignment operator
	/*When an existing Set (the left-hand side) is assigned the value of another Set (the right-hand side), 
	the result must be that the left-hand side object is a duplicate of the right-hand side object, with no 
	memory leak of list nodes (i.e. no list node from the old value of the left-hand side should be still
	allocated yet inaccessible).*/

private: 
	struct Node { //public by default 
		ItemType m_value;
		Node* m_next;
		Node* m_prev;
	};
	int m_size; //size of LinkedList 
	Node* head; //dummy pointer, always points to the top
	Node* tail; //second dummy node 
};

//non-member functions 
void unite(const Set& s1, const Set& s2, Set& result); 
void subtract(const Set& s1, const Set& s2, Set& result); 

#endif // !SET_H

//implementation of the set interface

#include "Set.h"
#include <iostream> //string already included thru "Set.h"

Set::Set() { //constructor
	head = new Node;
	tail = new Node;
	head->m_next = tail; //head is a dummy node
	//small comment: i had undefined behavior bug for about 2 hours until I realized head and tail must BOTH point to each other -- NO NULL PTR!
	tail->m_prev = head; //tail is also a dummy node, POINTS TO HEAD!!! LIGHTBULB!!
	m_size = 0;
}

bool Set::empty() const {
	return m_size == 0;
}

int Set::size() const {
	return m_size; //we have this instance var for a reason, after all
}

bool Set::insert(const ItemType& value) {
	//insert at the end of the list, if the value is not already present

	//first, check if the item is already in the list by traversing with a ptr using contains

	if (contains(value))
		return false;

	//now, add item to the end
	Node* add = new Node; //add is the new Node we add at the back
	add->m_value = value;

	add->m_prev = tail->m_prev; //set previous of add to (old) previous of tail
	add->m_next = tail; //set next of add to tail (the end)
	(tail->m_prev)->m_next = add; //set next to the old previous of tail to add
	tail->m_prev = add; //set previous of tail to add

	m_size++;
	return true;
}

bool Set::erase(const ItemType& value) {
	//check if the item is in the list; using contains is unnecessary because we have to find the value by traversing anyways
	Node *p = head->m_next;
	while (p != tail) {
		if (p->m_value == value) { //we find the value
			p->m_prev->m_next = p->m_next;
			p->m_next->m_prev = p->m_prev;
			delete p; //prevents memory leak?
			m_size--;
			return true;
		}
		p = p->m_next;
	}

	return false;
}

bool Set::contains(const ItemType& value) const {
	Node *p = head->m_next; //p points to the first item in the list
	while (p != tail) {
		if (p->m_value == value) { //if the value is already in the list
			return true;
		}
		p = p->m_next; //set p equal to the next element in the list
	}
	return false;
}

bool Set::get(int pos, ItemType& value) const {
	if (0 > pos || pos >= size()) {
		return false;
	}
	//find value in set that is strictly greater than EXACTLY pos items in the set
	Node *p = head->m_next;
	for (int i = 0; i < size(); i++) {
		int counter = 0;
		Node *temp = head->m_next;
		for (int j = 0; j < size(); j++) {
			if (p->m_value > temp->m_value) {
				counter++;
			}
			temp = temp->m_next;
		}
		if (counter == pos) {
			value = p->m_value; //nothing about the function is changed
			return true;
		}
		p = p->m_next;
	}

	return false; //should never get to this part
}

Set::~Set() { //destructor, since compiler generated one doesn't work
	Node *p = head;
	while (p != tail) {
		Node *temp = p->m_next; //this temp pointer should delete when we go out of scope, since we didn't dynamically allocate
		delete p;
		p = temp;
	}
	delete tail;
}

Set::Set(const Set& other) { //copy constructor, e.g. when you say Set a = b; a is the NEW set
	head = new Node; //same code as in constructor
	tail = new Node;
	head->m_next = tail;
	tail->m_prev = head;
	for (int i = 0; i < other.size(); i++) { //use get, won't be in the same order but that doesn't matter
		ItemType value;
		other.get(i, value); //doesn't modify original
		insert(value);
	}
	m_size = other.size();
}

Set& Set::operator= (const Set& rhs) { //assignment operator
	if (&rhs == this) //check for aliasing
		return *this;

	//if they aren't the same, use swap and return *this thru copy constructing
	Set temp = rhs; //copy constructing
	swap(temp); //swap with temp, temp should delete using the destructor
	return *this;
}

void Set::swap(Set& other) {
	//swap head and tail pointers using a temp pointer
	Node *temp = head;
	head = other.head;
	other.head = temp;

	Node *temp2 = tail;
	//^^these are dangling pointers that should delete as we go out of scope, since we didn't dynamically allocate
	tail = other.tail;
	other.tail = temp2;

	//swap sizes of the 2 linked lists
	int temp_size = m_size;
	m_size = other.size();
	other.m_size = temp_size;
}

/*void Set::dump() const{
	Node *p = head->m_next;
	while (p != tail) {
		std::cerr << p->m_value << " ";
		p = p->m_next;
	}
}*/

void unite(const Set& s1, const Set& s2, Set& result) {
	//use assignment operator for result
	result = s1;
	for (int i = 0; i < s2.size(); i++) {
		ItemType value;
		if (s2.get(i, value)) { //if we successfully get the value, add it to result
			result.insert(value);
		}
	}
}

void subtract(const Set& s1, const Set& s2, Set& result) {
	result = s1; //assignment operator for result, compiler knows you use assignment operator b/c u don't say "Set"
	for (int i = 0; i < s2.size(); i++) { //loop through all the elements of s2 using get
		ItemType value;
		if (s2.get(i, value) && result.contains(value)) { //if result has an element of s2 in it, erase it from result
			result.erase(value);
			i--; //decrement so we don't skip any indexes
		}
	}
}
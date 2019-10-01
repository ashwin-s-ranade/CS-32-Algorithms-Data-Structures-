/*#include <iostream>
#include <string>
#include <cassert>
#include "Set.h"
using namespace std; 

int main() {

	//tests performed using ItemType = std::string; 
	Set ss; //default constructor 
	assert(ss.empty() && ss.size() == 0); //empty() and size() 
	assert(ss.insert("first")); //insert()
	assert(!ss.insert("first")); //inserting a duplicate shouldn't work 
	assert(ss.insert("second"));
	assert(ss.insert("fake third"));
	assert(ss.insert("third"));
	assert(ss.erase("fake third")); //erasing should return true 
	assert(!ss.erase("Second")); //erasing something not in the Set shouldn't work
	assert(ss.contains("third"));
	assert(!ss.contains("fake third")); //testing erase works correctly 

	ItemType x = "xxx";
	assert(!ss.get(5, x) && x == "xxx"); //get shouldn't change anything here, and it should return false
	assert(ss.get(0, x) && x == "first"); //testing get again
	assert(ss.get(2, x) && x == "third"); //ss should stay unchanged 
	assert(!ss.contains("xxx")); //ss should not be changed  
	assert(ss.contains(x)); //x is a reference, so it should be changed

	Set copy = ss; //testing copy constructor 
	//assert(copy.size() == ss.size()); //checking if the sizes are the same 
	assert(copy.contains("third") && copy.contains("first") && copy.contains("second") && copy.size() == 3); //check if copy holds the same values 

	Set b;
	b.swap(ss); //testing swap 
	assert(ss.size() == 0 && b.size() == 3); //checking if the sizes are swapped correctly


	Set hello = b; //copy constructor 
	b = hello; //checking aliasing in assignment operator 
	assert(b.contains("first"));
	ss = b; //testing assignment operator once again, but without aliasing
	assert(ss.size() == 3 && b.size() == 3); //the sizes should be identical 

	
	//now testing unite + subtract 
	Set smallberg;
	smallberg.insert("a");
	smallberg.insert("c");
	smallberg.insert("e");
	smallberg.insert("f");
	smallberg.insert("g");

	Set nachenberg;
	nachenberg.insert("a");
	nachenberg.insert("b");
	nachenberg.insert("d");
	nachenberg.insert("h");
	nachenberg.insert("g");
	Set resultant;
	resultant.insert("zzz"); //testing to see if unite works if resultant already has some stuff in it                                                                                                                                                                                                                                                                                                                

	unite(smallberg, nachenberg, resultant); //testing normal unite function
	assert(resultant.contains("h")); //unite function tested 

	subtract(smallberg, nachenberg, resultant); //testing subtract function 
	assert(!resultant.contains("a") && !resultant.contains("g")); //testing subtract function

	//testing unite/subtract when aliasing (the first 2 parameters SHOULD change because they are being modified THROUGH the third parameter) 
	Set ryan;
	ryan.insert("1");
	ryan.insert("2");
	ryan.insert("3");
	ryan.insert("4");
	ryan.insert("5");
	Set yunus;
	yunus.insert("a");
	yunus.insert("b");
	yunus.insert("c");
	yunus.insert("1");

	unite(ryan, yunus, ryan); //testing unite when multiple of one element 
	assert(ryan.contains("a") && ryan.contains("b") && ryan.contains("c") && ryan.size() == 8);
	subtract(ryan, yunus, ryan); //testing subtract when multiple of one element 
	assert(!ryan.contains("1")); //one should go away from ryan 
	assert(yunus.size() == 4);

	subtract(yunus, yunus, yunus); //yunus should be unchanged (testing subtract when they are all the same element) 
	assert(yunus.size() == 0);
	subtract(ryan, ryan, ryan); //ryan should now be 0 
	assert(ryan.size() == 0);
	Set lll;
	lll.insert("my man");
	unite(lll, lll, lll); //checking if unite works if all parameters refer to the same list 
	assert(lll.size() == 1); //checking if unite works when aliasing 

	//testing if swap works between 2 empty sets 
	Set qz; 
	Set zq; 
	qz.swap(zq); 
	zq.swap(zq); 

	//testing if swap works with same set 
	Set ashwin_r; 
	ashwin_r.insert("boolean"); 
	ashwin_r.swap(ashwin_r); 

	cerr << "All tests succeeded" << endl;
}*/
#include "Set.h"
#include <iostream>
#include <cassert>
using namespace std;

void test()
{
	Set ss;
	assert(ss.insert("roti"));
	assert(ss.insert("pita"));
	assert(ss.size() == 2);
	assert(ss.contains("pita"));
	ItemType x = "laobing";
	assert(ss.get(0, x) && x == "pita");
	assert(ss.get(1, x) && x == "roti");
}

int main()
{
	test();
	cout << "Passed all tests" << endl;
}
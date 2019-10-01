#include "provided.h"
#include "Trie.h"
#include <string>
#include <vector>
#include <iostream>
#include <istream>
using namespace std;

class GenomeImpl
{
public:
	GenomeImpl(const string& nm, const string& sequence);
	static bool load(istream& genomeSource, vector<Genome>& genomes);
	int length() const;
	string name() const;
	bool extract(int position, int length, string& fragment) const;
private:
	string dnaSequence; 
	int sizeOfSequence; 
	string nom; 
};

GenomeImpl::GenomeImpl(const string& nm, const string& sequence)
{
	dnaSequence = sequence; 
	sizeOfSequence = sequence.size(); //O(1) time?????
	nom = nm; 
}

bool GenomeImpl::load(istream& genomeSource, vector<Genome>& genomes)
{
	//extract genome name
	//extract DNA sequence after that, everything must be cacpitalized 
	//create genome object w/ name and DNA sequene, and add it to genomes

	//if properly formatted, clear vector, then add everything
	//if improperly formatted, then return false
	genomes.clear();//reset vector 
	string str; 
	string nombre;
	string dna; 
	bool firstLine = true; 
	int count = 5; 
	while (getline(genomeSource, str)) { //returns false if we reached the end of the file
		if (str.empty()) //no empty lines allowed
		{
			return false; 
		}
		if (firstLine && str[0] != '>') {
			return false; 
		}//we must start with a name line 

		//check if we start with name line
		for (char c : str) {
			if (c == '>') {
				if (!firstLine) { //append Genome to genomes 
					if (dna.empty()) {
						return false; 
					} //2 name lines next to each other 
					genomes.push_back(Genome(nombre, dna)); 
					dna = ""; //reset DNA string 
				}

				nombre = str.substr(1);
				if (nombre.empty()) {
					return false;
				}//greater than character < with nothing after it 
				firstLine = false; 
				break; //skip the next for loop 
			}
			if (!firstLine) {
				char x = toupper(c); 
				if (x != 'N' && x != 'A' && x != 'C' && x != 'G' && x != 'T') {
					return false; 
				} //improper formatting
				dna += x; 
			}
		}
	}
	//now we reached the end of the file, so we should have a name and a DNA sequence 
	if (dna.empty()) {
		return false; 
	}
	if (nombre.empty()) { return false; }//this isn't really necessary but it doesnt hurt to check 
	genomes.push_back(Genome(nombre, dna)); 
	return true; //the file is properly formatted   
}

int GenomeImpl::length() const
{
	return sizeOfSequence; 
}

string GenomeImpl::name() const
{
	return nom;  
}

bool GenomeImpl::extract(int position, int length, string& fragment) const
{
	if (position + length > sizeOfSequence)
		return false; 
	fragment = dnaSequence.substr(position, length); 
	return true; 
}


//******************** Genome functions ************************************

// These functions simply delegate to GenomeImpl's functions.
// You probably don't want to change any of this code.

Genome::Genome(const string& nm, const string& sequence)
{
	m_impl = new GenomeImpl(nm, sequence);
}

Genome::~Genome()
{
	delete m_impl;
}

Genome::Genome(const Genome& other)
{
	m_impl = new GenomeImpl(*other.m_impl);
}

Genome& Genome::operator=(const Genome& rhs)
{
	GenomeImpl* newImpl = new GenomeImpl(*rhs.m_impl);
	delete m_impl;
	m_impl = newImpl;
	return *this;
}

bool Genome::load(istream& genomeSource, vector<Genome>& genomes)
{
	return GenomeImpl::load(genomeSource, genomes);
}

int Genome::length() const
{
	return m_impl->length();
}

string Genome::name() const
{
	return m_impl->name();
}

bool Genome::extract(int position, int length, string& fragment) const
{
	return m_impl->extract(position, length, fragment);
}

#include "provided.h"
#include "Trie.h"
#include <string>
#include <vector>
#include <iostream>
#include <fstream>
#include <algorithm>
#include <map>
using namespace std;

class GenomeMatcherImpl
{
public:
	GenomeMatcherImpl(int minSearchLength);
	void addGenome(const Genome& genome);
	int minimumSearchLength() const;
	bool findGenomesWithThisDNA(const string& fragment, int minimumLength, bool exactMatchOnly, vector<DNAMatch>& matches) const;
	bool findRelatedGenomes(const Genome& query, int fragmentMatchLength, bool exactMatchOnly, double matchPercentThreshold, vector<GenomeMatch>& results) const;
private:

	int minSearch;
	vector<Genome> jeans; 
	map <string, int> columbus; 
	Trie<string> geneBlock; 
	int numberOfGenomes; 
	bool almostMatches(string str1, string str2) const{
		if (str1.size() != str2.size()) return false; 
		int sameChars = 0; 
		for (int i = 0; i < str1.length(); i++)
			if (str1[i] == str2[i]) 
				sameChars++; 
		return sameChars == str1.size() - 1; 
	}
	int validAdd(vector<DNAMatch> matches, DNAMatch d) const{ //checking if D should be added to matches 
		bool encounteredName = false; 
		for (int i = 0; i < matches.size(); i++) {
			if (matches[i].genomeName == d.genomeName) {
				encounteredName = true; 
				if (matches[i].length < d.length || (matches[i].length == matches[i].length && matches[i].position > d.position)) {
					return i; 
				}
			}
		}
		if (encounteredName)
			return -10; 
		//this means we didn't encounter the name
		return -1; 
	}
	void searchPlease(string str, string& name, string& position, string& gene) const{
		bool hasReachedComma = false;
		for (char c : str) {
			if (isdigit(c) && !hasReachedComma)
				gene += c; 
			if (c == ',')
				hasReachedComma = true;
			if (!hasReachedComma)
				name += c;
			if (hasReachedComma && isdigit(c))
				position += c;
		}
	}

	static bool greaterThan(const GenomeMatch& a, const GenomeMatch& b){
		if (a.percentMatch > b.percentMatch)
			return true; 
		if (a.percentMatch == b.percentMatch)
			if (a.genomeName < b.genomeName)
				return true; //less than dictionary wise 
		return false; 
	}

	void searchTillPos(string obiwan, string& anakin) const{ //sjllsajdf, position 1 
		//search till the next next character is 'p'
		for (int i = 0; i < obiwan.size(); i++) {
			if (obiwan[i + 1] == ' ' && obiwan[i+2] == 'p' && obiwan[i+3] == 'o' && obiwan[i+4] == 's' && obiwan[i+5] =='i' && obiwan[i+6] == 't')
				break;
			anakin += obiwan[i]; 
		}
	}


};

GenomeMatcherImpl::GenomeMatcherImpl(int minSearchLength)
{
	minSearch = minSearchLength;
	numberOfGenomes = 0; 
}

void GenomeMatcherImpl::addGenome(const Genome& genome)
{
	//add genome to collection of genomes held by GenomeMatcher object
	jeans.push_back(genome);
	numberOfGenomes++;
	columbus.emplace(genome.name(), numberOfGenomes); 
	string clutch = ""; 
	searchTillPos(genome.name(), clutch); 
	int i = 0; string str;
	while (genome.extract(i, minimumSearchLength(), str)) {
		string value = genome.name() + ", position " + to_string(i);
		geneBlock.insert(str, value); 
		i++; 
	}
}

int GenomeMatcherImpl::minimumSearchLength() const
{
	return minSearch; 
}

bool GenomeMatcherImpl::findGenomesWithThisDNA(const string& fragment, int minimumLength, bool exactMatchOnly, vector<DNAMatch>& matches) const
{
	if (fragment.size() < minimumLength || minimumLength < minimumSearchLength())
		return false; 

	//match exists when 
	//segment in genome identical to first N characters of fragment OR 
	//exactMatchOnly is false, and segment in genome is snip of first N characters of fragment

	//create a map?? 
	//start with exact matches only 
	for (int i = minimumLength; i <= fragment.size(); i++) {
		vector<string> temp; 
		if (i > minimumSearchLength()) {
			bool perfection = true; 
			temp = geneBlock.find(fragment.substr(0, minimumSearchLength()), true); //we are checking perfection in everything 
			if (temp.empty() && exactMatchOnly) //go to the next substring if temp is empty, since that means there are no strings to work with in temp 
			{
				continue; 
			}
			if (temp.empty() && !exactMatchOnly) {
				temp = geneBlock.find(fragment.substr(0, minimumSearchLength()), false); //we realize our problems
				if (temp.empty()) continue; //if it is STILL empty, then we have to move on to the next fragment 
				perfection = false; //there isn't a perfect match, BUT there are imperfect matches 
			}
			for (int j = 0; j < temp.size(); j++) { //check if each match is actually what u want 
				//extract the value from temp 
				string pos = ""; string tocomma = ""; string gene = "";//everything before the comma 
				searchPlease(temp[j], tocomma, pos,gene);
				string chosenone = ""; 
				searchTillPos(temp[j], chosenone); 
				//convert strings to ints 
				string remainFragment = "";
				map<string,int>::const_iterator p = columbus.find(chosenone); //e.g. yeti
				if (p == columbus.end()) {
				}
				bool jk = jeans[p->second -1].extract(stoi(pos),i,remainFragment);

				//bool jk = jeans[stoi(gene) - 1].extract(stoi(pos), i, remainFragment); 
				if (jk == false) { continue; } //move on to the next match 
				if (fragment.substr(minimumSearchLength(), i - minimumSearchLength()) == remainFragment.substr(minimumSearchLength(), i - minimumSearchLength())) { //an exact match is found 
					DNAMatch d; d.genomeName = chosenone; d.length = i; d.position = stoi(pos); 
					int x = validAdd(matches, d);
					if (x >= 0) {
						matches[x] = d;
					}//i think this is right?

					else if (x == -1) //we haven't encountered that genome before 
					{
						matches.push_back(d); 
					}
				}
				//check if it matches with fragment.substr(minimumSearchLength()+1)
				if (!exactMatchOnly){
					if (!perfection && fragment.substr(minimumSearchLength(), i - minimumSearchLength()) == remainFragment.substr(minimumSearchLength(), i - minimumSearchLength())) {
						DNAMatch d; d.genomeName = chosenone; d.length = i; d.position = stoi(pos);
						int x = validAdd(matches, d);
						if (x >= 0)
							matches[x] = d;
						else if (x == -1) //we haven't encountered that genome before 
						{
							matches.push_back(d);
						}
					}
					else if (perfection && almostMatches(fragment.substr(minimumSearchLength(), i - minimumSearchLength()),remainFragment.substr(minimumSearchLength(), i - minimumSearchLength()))) {
						DNAMatch d; d.genomeName = chosenone; d.length = i; d.position = stoi(pos);
						int x = validAdd(matches, d);
						if (x >= 0)
							matches[x] = d;
						else if (x == -1) //we haven't encountered that genome before 
						{
							matches.push_back(d);
						}
					}
				}
			}
		}
		else { //this means i = minimumSearchLength()
			temp = geneBlock.find(fragment.substr(0, i), exactMatchOnly);
			//go through these and add them to allMatches 
			for (int i = 0; i < temp.size(); i++) {
				string pos = ""; string tocomma = ""; string gene = ""; //everything before the comma 
				searchPlease(temp[i], tocomma, pos,gene); 
				string chosenone = "";
				searchTillPos(temp[i], chosenone);
				DNAMatch d; d.genomeName = chosenone; d.length = minimumSearchLength(); d.position = stoi(pos);
				int x = validAdd(matches, d);
				if (x >= 0)
					matches[x] = d;
				else if (x == -1) //we haven't encountered that genome before 
				{matches.push_back(d); 
				}
				
			} 
		}
	}

	return matches.size() > 0; //if no matches found, return false; 
}

bool GenomeMatcherImpl::findRelatedGenomes(const Genome& query, int fragmentMatchLength, bool exactMatchOnly, double matchPercentThreshold, vector<GenomeMatch>& results) const
{
	if (fragmentMatchLength < minimumSearchLength()) return false; 
	results.clear(); 
	vector<int> matchesPerGenome(numberOfGenomes, 0); //initialize to all 0 initially
	vector<string> genomeNames(numberOfGenomes, ""); //initialize all to the empty string 
	for (int i = 1; i <= query.length() - fragmentMatchLength; i = i * fragmentMatchLength) {
		if (i == 0)i++; //ghetto asf
		string frag; 
		query.extract(i, fragmentMatchLength, frag); //not necessary to use boolean property of extract here
		vector<DNAMatch> xd; 
		if (findGenomesWithThisDNA(frag, fragmentMatchLength, exactMatchOnly, xd)) {
			for (int j = 0; j < xd.size(); j++) {
				map<string, int>::const_iterator p = columbus.find(xd[j].genomeName); //e.g. yeti
				if (p == columbus.end()) continue;
				matchesPerGenome[p->second-1]++; //increments the corresponding gene number 
				genomeNames[p->second-1] = xd[j].genomeName; 
			}
		}
	}
	for (int i = 0; i < matchesPerGenome.size(); i++) {
		if (matchesPerGenome[i] > 0) {
			int s = (query.length() / fragmentMatchLength); //utilize integer division
			double p = (static_cast<double>(matchesPerGenome[i]) / s) * 100; //use static_cast to prevent integer division
			if (p >= matchPercentThreshold) { //matchesPerGenome[i] is a matching genome
				GenomeMatch g; 
				g.genomeName = genomeNames[i];  //make this name the correct one! 
				g.percentMatch = p; 
				results.push_back(g); 
			}
		}
	}
	
	//sort results 
	sort(results.begin(), results.end(), greaterThan); 
	return results.size() > 0; 
}

//******************** GenomeMatcher functions ********************************

// These functions simply delegate to GenomeMatcherImpl's functions.
// You probably don't want to change any of this code.

GenomeMatcher::GenomeMatcher(int minSearchLength)
{
	m_impl = new GenomeMatcherImpl(minSearchLength);
}

GenomeMatcher::~GenomeMatcher()
{
	delete m_impl;
}

void GenomeMatcher::addGenome(const Genome& genome)
{
	m_impl->addGenome(genome);
}

int GenomeMatcher::minimumSearchLength() const
{
	return m_impl->minimumSearchLength();
}

bool GenomeMatcher::findGenomesWithThisDNA(const string& fragment, int minimumLength, bool exactMatchOnly, vector<DNAMatch>& matches) const
{
	return m_impl->findGenomesWithThisDNA(fragment, minimumLength, exactMatchOnly, matches);
}

bool GenomeMatcher::findRelatedGenomes(const Genome& query, int fragmentMatchLength, bool exactMatchOnly, double matchPercentThreshold, vector<GenomeMatch>& results) const
{
	return m_impl->findRelatedGenomes(query, fragmentMatchLength, exactMatchOnly, matchPercentThreshold, results);
}

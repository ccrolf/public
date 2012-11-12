/* 
   This program solves the word puzzles. In the puzzle, n letters are given. 
   From these letters all possible words are to be found. Only "normal" letters appear, 
   i.e., no apostrophes or dashes. Also, proper nouns are not allowed. The input 
   to the program is a string representing the letters in the problem and an OpenOffice 
   dictionary file, you can find such files here: 
   http://wiki.services.openoffice.org/wiki/Dictionaries
*/

#include <iostream>
#include <fstream>
#include <string>
#include <hash_set.h>
#include <cctype>

using namespace std;

// fit whole alphabet in multiple of 2 (assumption), change to long in get_bits if >32
#define ALPHABET_LENGTH 32 


/* Calculate the hash code for a string */
struct hash_string
{
	size_t operator()(const string& s) const 
	{
		size_t res = 1;
		for (size_t i = 0, iEnd = s.length(); i < iEnd; ++i)
			res += s[i] << i;
		return res;
	}
};


/* Get the representation of a word as an integer. Each 1 represents
   that character is present at least once in the word */
unsigned int get_bits(const string& s)
{
	static const unsigned int shift_map[] = {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1, 2, 4, 8, 16, 32, 64, 128, 256, 512, 1024, 2048, 4096, 8192, 16384, 32768, 65536, 131072,
						262144, 524288, 1048576, 2097152, 4194304, 8388608, 16777216, 33554432, 67108864};
	unsigned int res = 0;
	for (int i = 0, iEnd = s.length(); i < iEnd; ++i)
	{
		if (isalpha(s[i])) // don't handle apostrophe etc
			res |= shift_map[s[i]]; // we assume all tokens fit in 32 bits
		else
			return 0;
	}		
	return res;
}


/* Calculate how many times each character appears in a word */
void get_counts(unsigned int* counts, const string& s)
{
	memset(counts, 0, ALPHABET_LENGTH * sizeof(int));
	for (unsigned int i = 0, iEnd = s.length(); i < iEnd; ++i) 
		++counts[s[i]-'a'];
}


/* Print the words that are possible to generate from the given tokens */
void print_words(const string& tokens, const hash_set<string, hash_string> words[])
{
	unsigned int token_counts[ALPHABET_LENGTH];
	unsigned int word_counts[ALPHABET_LENGTH];
	get_counts(token_counts, tokens);
	unsigned int matches = 0;
	
	for (unsigned int i = 0; i < tokens.length(); ++i)
	{
		for (hash_set<string, hash_string>::const_iterator it = words[i].begin(); it != words[i].end(); ++it) 
		{
			get_counts(word_counts, *it);
			bool match = true;
			for (unsigned int j = 0; match && j < ALPHABET_LENGTH; ++j) 
				if (word_counts[j] > token_counts[j])
					match = false;
			if (match) 
			{
				cout << *it << endl;
				++matches;
			}
		}
	}
	cout << "Total matches: " << matches << endl;
}



int main(int argc, char** argv) 
{
	if (argc < 3) 
	{
		cout << "Please include the characters in the problem and the dictionary file" << endl;
		return 1;
	}
	
	ifstream in(argv[2], ifstream::in);
	if (!in.good()) 
	{
		cout << "Error opening file" << endl;
		return 2;
	}
	
	string tokens(argv[1]);
	/* simplify, transform to lower case */
	transform(tokens.begin(), tokens.end(), tokens.begin(), ::tolower);
	const unsigned int length_to_match = tokens.length();
	const unsigned int bits_to_match = get_bits(tokens);
	
	hash_set<string, hash_string> *words = new hash_set<string, hash_string>[tokens.length()];
	//for (int i = 0; i < tokens.length(); ++i)
		//words[i](512); 

	/* Read through the file, adding words that may be a fit to the hash set */
	while (in.good())
	{
		string s;
		getline(in, s);
		if (isupper(s[0]) || isdigit(s[0])) // we don't allow proper nouns
			continue;
		
		/* strip off the extra information in the open office dictionary */
		size_t delim_pos = s.find_first_of('\'');
		if (delim_pos < s.length())
			s.erase(delim_pos);
		else if ((delim_pos = s.find_first_of('/')) < s.length())
			s.erase(delim_pos);
		
		if (s.length() > length_to_match)	// a word cannot match tokens if longer
			continue;
			
		/* simplify, transform to lower case */
		transform(s.begin(), s.end(), s.begin(), ::tolower);
		const unsigned int bits = get_bits(s);
		// if word contains any letter not in the given tokens, continue
		if (bits == 0 || (bits & bits_to_match) != bits) 
			continue;
		
		words[s.length()].insert(s);
	}
  	in.close();
	
	print_words(tokens, words);
	
	return 0;
}
	
	
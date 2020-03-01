#include <iostream>
#include <numeric>
#include <algorithm>
#include <vector>
#include <fstream>
#include <math.h>
#include <ctime>

#define SIZE 5000
#define STR_SIZE 100

using namespace std;

inline bool descending_pair(const pair<int, int> &a, const pair<int, int> &b) {
	return a.first > b.first;
}

enum probing {LINEAR_PROBING, QUADRATIC_PROBING};

class HashTable {
	private:
		probing _probing;
		string arr[SIZE];
		pair<int, int> counters[SIZE];

		int hash(string); //Hash input string into index
		int linear_probing(string, string); //return indexOfArray for inserting
		int quadratic_probing(string, string); //return indexOfArray for inserting
		bool is_desired(int, string);
		void reset_counters();
		void reset_array();

	public:
		HashTable(
			probing probing_alog // Algorithm for insertion and searching
		) {
			_probing = probing_alog;
			reset_counters();
		}
		void insert(string);
		bool search(string);
		void remove(string);
};

void HashTable::reset_counters() {
	for (int i = 0; i < SIZE; ++i) {
		counters[i].first = 0;
		counters[i].second = i;
	}
}

void HashTable::reset_array() {
	for (int i = 0; i < SIZE; ++i)
		arr[i] = "";
}

bool HashTable::is_desired(int index, string key) {
	return arr[index] == key;
}

int HashTable::hash(string key) {
    int ind = 0;
    int size = key.size();
    for(char &c : key) {
        int n = c - 'a' + 1;

        ind += ((unsigned long int) pow(27, --size) % SIZE);
    }

    return ind % SIZE;
}

int HashTable::linear_probing(string key, string search_term = "") {
	int ind = hash(key);

	if(!is_desired(ind, search_term)) {
		int old = ind++;
		while(old != ind && !is_desired(ind, search_term))
			ind = ++ind % SIZE;

		if(old == ind) return -1;
	}

	return ind % SIZE;
}

int HashTable::quadratic_probing(string key, string search_term = "") {
	int ind = hash(key);

	bool checked[SIZE];
	int base = 1;
	while(accumulate(begin(checked), end(checked), 0) != SIZE 
		&& !is_desired(ind, search_term)) {
			checked[ind] = true;
			ind = (ind + (unsigned long int)pow(base++, 2)) % SIZE;
		}

	if(accumulate(begin(checked), end(checked), 0) == SIZE) return -1;

	return ind % SIZE;
}

/*
* Insert into appropiate index based on algorithm
*/
void HashTable::insert(string s) {
	int ind;
	if(_probing == LINEAR_PROBING)
		ind = linear_probing(s);
	else
		ind = quadratic_probing(s);

	if(ind == -1)
		return;

	arr[ind] = s;
}

/*
* If found return true, else false
*/
bool HashTable::search(string s) {
	int ind;
	if(_probing == LINEAR_PROBING)
		ind = linear_probing(s, s);
	else
		ind = quadratic_probing(s, s);

	if (ind == -1)
		return false;

	counters[ind].first++;

	return arr[ind] == s;
}

void HashTable::remove(string s) {
	// cerr << endl << "Remove started: \n";
	int ind = linear_probing(s, s);

	if(ind == -1)
		return;

	arr[ind] = "";
	counters[ind].first = 0;
	sort(counters, counters + SIZE, descending_pair);

	string tmp[SIZE];
	copy(begin(arr), end(arr), tmp);
	reset_array();
	for (int i = 0; i < SIZE; ++i) {
		int ind = counters[i].second;
		if(tmp[ind] != "")
			insert(tmp[ind]);
	}

	// print_counters();

	reset_counters();
}

string random_word(int num_chars) {
	string str;
	str.reserve(num_chars);

	for (int i = 0; i < num_chars; ++i)
		str += rand() % (127 - 32) + 32;
	
	return str;
}

vector<string> test_insertion(HashTable *ht) {
	vector<string> v;
	v.reserve(SIZE);
	for (int i = 0; i < SIZE; ++i){
		string str = random_word(STR_SIZE);
		ht->insert(str);
		v.push_back(str);
	}

	return v;
}

void test_searching(HashTable *ht, vector<string> *v) {
	for (int i = 0; i < SIZE; ++i)
		ht->search((*v)[i]);
}

void test_deletion(HashTable *ht, vector<string> *v) {
	for (int i = 0; i < SIZE; ++i)
		ht->remove((*v)[i]);
}

void random_searching(HashTable *ht, vector<string> *v) {
	for (int i = 0; i < SIZE; ++i)
		ht->search((*v)[rand() % SIZE]);
}

int main()
{
	srand(time(0));

	HashTable hash_table(LINEAR_PROBING);

    auto start = chrono::high_resolution_clock::now();
    auto stop = chrono::high_resolution_clock::now();
    chrono::duration<double> result = stop - start;

	double ms = 0.0;
	for (int i = 0; i < 5; ++i)
	{
		auto v = test_insertion(&hash_table);
		random_searching(&hash_table, &v);
		hash_table.remove(v[rand() % SIZE]);
    	auto start = chrono::high_resolution_clock::now();
		test_searching(&hash_table, &v);
    	auto stop = chrono::high_resolution_clock::now();
   		chrono::duration<double> result = stop - start;

		ms += result.count() * 1000;
	}
	ms /= 5.0;
	
	ofstream write("result_probing.txt", ios::app);
	write << "Searching with words of size " << STR_SIZE 
		<< " and hash table of size " << SIZE << '\n';
	write << ms << " ms\n";
	write.close();

	cout << "DONE\n";

	return 0;
}


#define MIN_CHILD_INDEX 0
#define MAX_CHILD_INDEX 127
#define CHILD_INDEX_GAP 128

class TrieNode
{
public:
	char* key_value;
	char* value;
	TrieNode* children[CHILD_INDEX_GAP];

	TrieNode();
	~TrieNode();
};

class Trie
{
protected:
	int size;
	TrieNode* root;

public:
	Trie();
	virtual ~Trie();

	/**
	 * returns false if already contains!
	 */
	bool insert(char* key, char* value = 0);
	int getSize();
	bool contains(char* key);
	char* getValue(char* key);
};


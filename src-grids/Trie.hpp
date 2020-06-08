#ifndef TRIE_HPP__
#define TRIE_HPP__

#define MIN_CHILD_INDEX 0
#define MAX_CHILD_INDEX 127
#define CHILD_INDEX_GAP 128

#include <string.h>
#include <stdlib.h>
#include <stdio.h>




template <class Type>
class TrieNode
{
public:
	char* key_value;
	Type value;
	TrieNode<Type>* children[CHILD_INDEX_GAP];

	TrieNode()
	{
		this->key_value = 0;
		this->value = 0;
		bzero(this->children, CHILD_INDEX_GAP * sizeof(TrieNode<Type>*));
	}

	~TrieNode()
	{
	for ( int i = 0; i < CHILD_INDEX_GAP; i++ )
	{
		if ( this->children[i] != 0 )
		{
			delete this->children[i];
			this->children[i] = 0;
		}
	}

	if ( this->key_value != 0 )
	{
		free(this->key_value);
		this->key_value = 0;
	}
}

};


template <>
class TrieNode<char*>
{
public:
	char* key_value;
	char* value;
	TrieNode<char*>* children[CHILD_INDEX_GAP];

	TrieNode()
	{
		this->key_value = 0;
		this->value = 0;
		bzero(this->children, CHILD_INDEX_GAP * sizeof(TrieNode<char*>*));
	}

	~TrieNode()
	{
	for ( int i = 0; i < CHILD_INDEX_GAP; i++ )
	{
		if ( this->children[i] != 0 )
		{
			delete this->children[i];
			this->children[i] = 0;
		}
	}

	if ( this->key_value != 0 )
	{
		free(this->key_value);
		this->key_value = 0;
	}

	if ( this->value != 0 )
	{
		free(this->value);
		this->value = 0;
	}
}
};

template <class Type>
class Trie
{
protected:
	int size;
	TrieNode<Type>* root;

public:
	Trie()
	{
		this->size = 0;
		this->root = 0;
	}

	virtual ~Trie()
	{
		if ( this->root != 0 )
		{
			delete this->root;
			this->root = 0;
		}
		this->size = 0;
	}

	/**
	 * returns false if already contains!
	 */
	bool insert(char* key, Type value = 0)
	{
		printf("[Trie::insert]\n");

		if ( this->root == 0 )
		{
			this->root = new TrieNode<Type>();
			this->root->key_value = key;
			this->root->value = value;

			this->size = 1;

			fprintf(stdout, "[Trie::insert] Inserting string %s at root!\n", key);
			return true;
		}

		TrieNode<Type>* curnode = this->root;
		int depth = 0;
		int length = strlen(key);

		while ( depth <= length )
		{
			if ( strcmp(curnode->key_value, key) == 0 )
			{
				printf("[Trie::insert] Trying to insert %s when we already have it!\n", key);
				return false;
			}

			TrieNode<Type>* nextnode = curnode->children[key[depth] - MIN_CHILD_INDEX];

			if ( nextnode == 0 )
			{
				nextnode = new TrieNode<Type>();

				nextnode->key_value = key;
				nextnode->value = value;

				curnode->children[key[depth]-MIN_CHILD_INDEX] = nextnode;
				(this->size)++;

				fprintf(stdout, "[Trie::insert] Inserting string %s at depth %d\n", key, depth);
				return true;
			}

			// continue!
			curnode = nextnode;
			depth++;
		}

		fprintf(stdout, "[Trie::insert] Trying to insert %s but ran out of depth?\n", key);
		return false;
	}

	int getSize()
	{
		return this->size;
	}

	bool contains(char* key)
	{
		printf("[Trie::contains]\n");

		if ( key == 0 )
		{
			return false;
		}

		if ( this->root == 0 )
		{
			return false;
		}

		TrieNode<Type>* curnode = this->root;
		int depth = 0;
		int length = strlen(key);

		while ( curnode != 0 && depth < length )
		{
			if ( curnode->key_value == 0 )
			{
				return false;
			}

			if ( strcmp(curnode->key_value, key) == 0 )
			{
				return true;
			}

			TrieNode<Type>* nextnode = curnode->children[key[depth] - MIN_CHILD_INDEX];

			if ( nextnode == 0 )
			{
				return false;
			}

			// continue!
			curnode = nextnode;
			depth++;
		}

		return false;
	}

	Type getValue(const char* key)
	{
		if ( this->root == 0 || key == 0 )
		{
			return 0;
		}

		TrieNode<Type>* curnode = this->root;
		int depth = 0;
		int length = strlen(key);

		while ( depth < length )
		{
			if ( curnode->key_value == 0 )
			{
				return 0;
			}

			if ( strcmp(curnode->key_value, key) == 0 )
			{
				return curnode->value;
			}

			TrieNode<Type>* nextnode = curnode->children[key[depth] - MIN_CHILD_INDEX];

			if ( nextnode == 0 )
			{
				return 0;
			}

			// continue!
			curnode = nextnode;
			depth++;
		}

		return 0;
	}
};

#endif



#include "Trie.hpp"

#include <stdio.h>
#include <string.h>
#include <stdlib.h>



TrieNode::TrieNode()
{
	this->key_value = 0;
	this->value = 0;
	bzero(this->children, CHILD_INDEX_GAP*sizeof(TrieNode*));
}

TrieNode::~TrieNode()
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

Trie::Trie()
{
	this->size = 0;
	this->root = 0;
}
	
Trie::~Trie()
{
	if ( this->root != 0 )
	{
		delete this->root;
		this->root = 0;
	}
	this->size = 0;
}

int Trie::getSize()
{
	return this->size;
}

/**
 * returns false if already contains!
 */
bool Trie::insert(char* key, char* value)
{
	if ( this->root == 0 )
	{
		this->root = new TrieNode();
		this->root->key_value = key;
		this->root->value = value;

		this->size = 1;
		return true;
	}

	TrieNode* curnode = this->root;
	int depth = 0;
	int length = strlen(key);

	while ( depth <= length )
	{
		if ( strcmp(curnode->key_value, key) == 0 )
		{
			return false;
		}

		if ( depth == length )
		{
			// we cannot extend! We must switch the values... as we have a substring!
			char* old_key = curnode->key_value;
			char* old_value = curnode->value;

			curnode->key_value = key;
			curnode->value = value;

			key = old_key;
			value = old_value;
		}

		TrieNode* nextnode = curnode->children[key[depth] - MIN_CHILD_INDEX];

		if ( nextnode == 0 )
		{
			nextnode = new TrieNode();
			
			nextnode->key_value = key;
			nextnode->value = value;

			curnode->children[key[depth]-MIN_CHILD_INDEX] = nextnode;
			(this->size)++;

			return true;
		}

		// continue!
		curnode = nextnode;
		depth++;
	}

	return false;
}

/**
 * returns true if already contains!
 */
bool Trie::contains(char* key)
{
	if ( this->root == 0 )
	{
		return false;
	}

	TrieNode* curnode = this->root;
	int depth = 0;
	int length = strlen(key);

	bool free_value = false;

	while ( depth < length )
	{
		if ( curnode->key_value == 0 )
		{
			return false;
		}

		if ( strcmp(curnode->key_value, key) == 0 )
		{
			return true;
		}

		TrieNode* nextnode = curnode->children[key[depth] - MIN_CHILD_INDEX];

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


/**
 * returns true if already contains!
 */
char* Trie::getValue(char* key)
{
	if ( this->root == 0 )
	{
		return 0;
	}

	TrieNode* curnode = this->root;
	int depth = 0;
	int length = strlen(key);

	bool free_value = false;

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

		TrieNode* nextnode = curnode->children[key[depth] - MIN_CHILD_INDEX];

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
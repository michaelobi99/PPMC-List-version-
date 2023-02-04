#pragma once
#include <memory>
#include <iostream>
#include <stdexcept>

#define ESCAPE 257
#define END_OF_STREAM 256
#define SYMBOL_COUNT 257 //ascii 256 symbols + EOF symbol
#define MAX_SIZE  ((1 << 14) - (1))

using USHORT = std::uint16_t;


struct Trie {
	struct Node {
		Node* downPointer; //pointer to the first child
		Node* next; //next sibling of this node under the same parent
		Node* prev; //next sibling of this node under the same parent
		Node* vinePtr;
		int symbol;
		std::uint8_t contextCount;
		std::uint8_t depthInTrie;
		std::uint8_t noOfChildren;
		Node() : downPointer{ nullptr }, next{ nullptr }, prev{ nullptr }, vinePtr { nullptr }, contextCount{ 0 },
			depthInTrie{ 0 }, noOfChildren{ 0 }, symbol{ char() } {}
		Node* find(int index) {
			Node* cursor = downPointer;
			while (cursor && cursor->symbol != index) {
				cursor = cursor->next;
			}
			return cursor;
		}
		Node* insert(int symbol) {
			if (noOfChildren == 0) {
				downPointer = new Node();
				downPointer->symbol = symbol;
				downPointer->depthInTrie = depthInTrie + 1;
				downPointer->contextCount++;
				++noOfChildren;
				return downPointer;
			}
			else {
				Node* cursor = downPointer;
				while (cursor->symbol != symbol && cursor->next) {
					cursor = cursor->next;
				}
				if (cursor->symbol == symbol) {
					cursor->contextCount++;
					Node* tempCursor = cursor;
					while (tempCursor->prev && tempCursor->prev->contextCount < cursor->contextCount) {
						tempCursor = tempCursor->prev;
					}
					if (tempCursor->prev) {
						cursor->prev->next = cursor->next;
						if (cursor->next)
							cursor->next->prev = cursor->prev;
						cursor->next = tempCursor;
						cursor->prev = tempCursor->prev;
						tempCursor->prev = cursor;
					}
					else {
						if (cursor != tempCursor) {
							cursor->prev->next = cursor->next;
							if (cursor->next)
								cursor->next->prev = cursor->prev;
							cursor->next = tempCursor;
							cursor->prev = nullptr;
							tempCursor->prev = cursor;
							downPointer = cursor;
						}
					}
				}
				else {
					cursor->next = new Node;
					cursor->next->symbol = symbol;
					cursor->next->depthInTrie = depthInTrie + 1;
					cursor->next->contextCount++;
					cursor->next->prev = cursor;
				}
				++noOfChildren;
				return cursor->next;
			}
		}
		~Node() {
			if (downPointer) delete downPointer;
			if (next) delete next;
		}
	};
	std::unique_ptr<Node> root;
	uint8_t maxDepth{ 0 };
} trie;

Trie::Node* basePtr;//points to the most recent node of the Trie
Trie::Node* cursor;
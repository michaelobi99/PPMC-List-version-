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
		std::unique_ptr<Node> downPointer; //pointer to the first child
		std::unique_ptr<Node> next; //next sibling of this node under the same parent
		Node* vinePtr;
		int symbol;
		std::uint8_t contextCount;
		std::uint8_t depthInTrie;
		std::uint8_t noOfChildren;
		Node() : downPointer{ nullptr }, next{ nullptr }, vinePtr { nullptr }, contextCount{ 0 }, depthInTrie{ 0 }, noOfChildren{ 0 }, symbol{ char() } {}
		Node* find(int index) {
			Node* cursor = downPointer.get();
			while (cursor && cursor->symbol != index) {
				cursor = cursor->next.get();
			}
			return cursor;
		}
		Node* insert(int symbol) {
			if (noOfChildren == 0) {
				downPointer = std::make_unique<Node>();
				downPointer->symbol = symbol;
				downPointer->depthInTrie = depthInTrie + 1;
				downPointer->contextCount++;
				++noOfChildren;
				return downPointer.get();
			}
			else {
				Node* tempCursor = downPointer.get();
				while (tempCursor->next.get())
					tempCursor = tempCursor->next.get();
				tempCursor->next = std::make_unique<Node>();
				tempCursor->next->symbol = symbol;
				tempCursor->next->depthInTrie = depthInTrie + 1;
				tempCursor->next->contextCount++;
				++noOfChildren;
				return tempCursor->next.get();
			}
		}
	};
	std::unique_ptr<Node> root;
	uint8_t maxDepth{ 0 };
} trie;

Trie::Node* basePtr;//points to the most recent node of the Trie
Trie::Node* cursor;
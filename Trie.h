#pragma once
#include <memory>
#include <iostream>
#include <stdexcept>
#include "Arena.h"

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
		std::uint8_t contextCount;
		std::uint8_t depthInTrie;
		std::uint8_t noOfChildren;
		char symbol;
		Node() : downPointer{ nullptr }, next{ nullptr }, vinePtr { nullptr }, contextCount{ 0 }, depthInTrie{ 0 }, noOfChildren{ 0 }, symbol{ char() } {}
		Node* find(int index) {
			//printf("Entered find\n");
			if (index < 0 || index > 256)
				throw std::out_of_range("ERROR: index out of bound");
			Node* cursor = downPointer.get();
			while (cursor) {
				if ((int)cursor->symbol == index)
					break;
				cursor = cursor->next.get();
			}
			//printf("left find\n");
			return cursor;
		}
		Node* insert(int symbol) {
			//printf("Entered insert\n");
			if (noOfChildren == 0) {
				downPointer = std::make_unique<Node>();
				downPointer->symbol = (char)symbol;
				downPointer->depthInTrie = this->depthInTrie + 1;
				downPointer->contextCount = 1;
				++noOfChildren;
				return downPointer.get();
			}
			else {
				Node* tempCursor = downPointer.get();
				while (tempCursor->next.get())
					tempCursor = tempCursor->next.get();
				tempCursor->next = std::make_unique<Node>();
				tempCursor = tempCursor->next.get();
				tempCursor->symbol = (char)symbol;
				tempCursor->depthInTrie = this->depthInTrie + 1;
				tempCursor->contextCount = 1;
				++noOfChildren;
				return tempCursor;
			}
			//printf("left insert\n");
		}
	};
	std::unique_ptr<Node> root;
	uint8_t maxDepth{ 0 };
} trie;

Trie::Node* basePtr;//points to the most recent node of the Trie
Trie::Node* cursor;
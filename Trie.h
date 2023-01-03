#pragma once
#include <vector>
#include <ranges>
#include <memory>
#include <iostream>
#include <stdexcept>
#include "Arena.h"

#define ESCAPE 257
#define END_OF_STREAM 256
#define SYMBOL_COUNT 257 //ascii 256 symbols + EOF symbol
#define MAX_SIZE  ((1 << 14) - (1))

using USHORT = std::uint16_t;


//Arena<1024 * 1024 * 5> arena{};
struct Trie {
public:
	Trie() : root{ new Node } {}
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
			if (index < 0 || index > 256)
				throw std::out_of_range("ERROR: index out of bound");
			Node* cursor = downPointer.get();
			while (cursor) {
				if ((int)cursor->symbol == index)
					break;
				cursor = cursor->next.get();
			}
			return cursor;
		}
		void insert(char symbol) {
			if (noOfChildren == 0) {
				downPointer = std::make_unique<Node>();
				downPointer->symbol = symbol;
			}
			else {
				Node* tempCursor = downPointer.get();
				while (tempCursor->next.get())
					tempCursor = tempCursor->next.get();
				tempCursor->next = std::make_unique<Node>();
				tempCursor->next->symbol = symbol;
			}
			++noOfChildren;
		}
		/*void* operator new(std::size_t memSize) {
			return arena.allocate(memSize);
		}
		void operator delete(void* memory, std::size_t memSize) {
			arena.deallocate(static_cast<std::byte*>(memory), memSize);
		}*/
	};
	std::unique_ptr<Node> root;
	uint8_t maxDepth{ 0 };
} trie;

Trie::Node* basePtr;//points to the most recent node of the Trie
Trie::Node* cursor;
#pragma once
#include "Trie.h"
#include <vector>

struct Symbol {
	USHORT lowCount;
	USHORT highCount;
	USHORT scale;
};
std::uint16_t totals[SYMBOL_COUNT + 2]; //used to get the cumulative totals of any context
std::vector<uint8_t> negativeOneContextTable(SYMBOL_COUNT);
std::vector<std::uint8_t> excludedCharacters(SYMBOL_COUNT);
int escapeContext = 0;


void initializeModel(uint32_t order) {
	trie.root = std::make_unique<Trie::Node>();
	trie.maxDepth = order + 1;
	basePtr = trie.root.get();//points to the most recent node of the Trie
	cursor = basePtr;
	std::memset(excludedCharacters.data(), 0, std::size(excludedCharacters));
	std::memset(negativeOneContextTable.data(), 1, std::size(negativeOneContextTable));
}

void rescaleContextCount(Trie::Node* cursor) { 
	Trie::Node* children = cursor->downPointer.get();
	for (; children; children = children->next.get()) {
		children->contextCount = (children->contextCount + 1) / 2;
	}
}

void initializeTotalsToCurrentTable() {
	totals[0] = 0;
	int i = 0;
	uint16_t tempArray[SYMBOL_COUNT];
	std::memset(tempArray, 0, sizeof(tempArray));
	if (cursor) {//context exists
		Trie::Node* children = cursor->downPointer.get();
		for (; children; children = children->next.get()) {
			tempArray[(int)children->symbol] = children->contextCount;
		}
		for (i = 0; i < (SYMBOL_COUNT - 1); i += 8) {
			totals[i + 1] = totals[i] +
				((excludedCharacters[i]) ? 0 : tempArray[i]);
			totals[i + 2] = totals[i + 1] +
				((excludedCharacters[i + 1]) ? 0 : tempArray[i + 1]);
			totals[i + 3] = totals[i + 2] +
				((excludedCharacters[i + 2]) ? 0 : tempArray[i + 2]);
			totals[i + 4] = totals[i + 3] +
				((excludedCharacters[i + 3]) ? 0 : tempArray[i + 3]);
			totals[i + 5] = totals[i + 4] +
				((excludedCharacters[i + 4]) ? 0 : tempArray[i + 4]);
			totals[i + 6] = totals[i + 5] +
				((excludedCharacters[i + 5]) ? 0 : tempArray[i + 5]);
			totals[i + 7] = totals[i + 6] +
				((excludedCharacters[i + 6]) ? 0 : tempArray[i + 6]);
			totals[i + 8] = totals[i + 7] +
				((excludedCharacters[i + 7]) ? 0 : tempArray[i + 7]);
		}
		totals[SYMBOL_COUNT] = totals[SYMBOL_COUNT - 1] +
			((excludedCharacters[SYMBOL_COUNT - 1]) ? 0 : tempArray[SYMBOL_COUNT - 1]);
		totals[SYMBOL_COUNT + 1] = totals[SYMBOL_COUNT] + cursor->noOfChildren;
	}
	else {//cursor is at roots vinePtr, i.e negative one context
		for (i = 0; i < (SYMBOL_COUNT - 1); i += 8) {
			totals[i + 1] = totals[i] +
				((excludedCharacters[i]) ? 0 : negativeOneContextTable[i]);
			totals[i + 2] = totals[i + 1] +
				((excludedCharacters[i + 1]) ? 0 : negativeOneContextTable[i + 1]);
			totals[i + 3] = totals[i + 2] +
				((excludedCharacters[i + 2]) ? 0 : negativeOneContextTable[i + 2]);
			totals[i + 4] = totals[i + 3] +
				((excludedCharacters[i + 3]) ? 0 : negativeOneContextTable[i + 3]);
			totals[i + 5] = totals[i + 4] +
				((excludedCharacters[i + 4]) ? 0 : negativeOneContextTable[i + 4]);
			totals[i + 6] = totals[i + 5] +
				((excludedCharacters[i + 5]) ? 0 : negativeOneContextTable[i + 5]);
			totals[i + 7] = totals[i + 6] +
				((excludedCharacters[i + 6]) ? 0 : negativeOneContextTable[i + 6]);
			totals[i + 8] = totals[i + 7] +
				((excludedCharacters[i + 7]) ? 0 : negativeOneContextTable[i + 7]);
		}
		totals[SYMBOL_COUNT] = totals[SYMBOL_COUNT - 1] +
			((excludedCharacters[SYMBOL_COUNT - 1]) ? 0 : negativeOneContextTable[SYMBOL_COUNT - 1]);
		totals[SYMBOL_COUNT + 1] = totals[SYMBOL_COUNT] + 0;
	}
}

void getProbability() {
	initializeTotalsToCurrentTable();
}

void fillCharactersToBeExcluded() {
	Trie::Node* children = cursor->downPointer.get();
	for (; children; children = children->next.get()) {
		excludedCharacters[children->symbol] = 1;
	}
}

bool convertIntToSymbol(int c, Symbol& s) {
	bool escaped{};
	if (escapeContext >= 0) {
		for (; cursor; --escapeContext, cursor = cursor->vinePtr) {
			if (cursor->noOfChildren > 0) break;
		}
	}
	if (!cursor || cursor->find(c)) {//context doesn't exist, i.e cursor is at roots vinePtr
		getProbability();
		std::memset(excludedCharacters.data(), 0, std::size(excludedCharacters));
		s.highCount = totals[c + 1];
		s.lowCount = totals[c];
		escaped = false;
	}
	else { //current symbol doesnt exist in context, but context exists. avoid zero probability with escape
		getProbability();
		fillCharactersToBeExcluded();
		s.highCount = totals[ESCAPE + 1];
		s.lowCount = totals[ESCAPE];
		cursor = cursor->vinePtr;
		--escapeContext;
		escaped = true;
	}
	s.scale = totals[ESCAPE + 1];
	return escaped;
}

void getSymbolScale(Symbol& s) {
	while (cursor) {
		if (cursor->noOfChildren > 0) 
			break;
		cursor = cursor->vinePtr;
	}
	getProbability();
	s.scale = totals[ESCAPE + 1];
}

int convertSymbolToInt(long index, Symbol& s) {
	int c;
	for (c = ESCAPE; index < totals[c]; --c) {}
	s.highCount = totals[c + 1];
	s.lowCount = totals[c];
	if (c == ESCAPE) {
		fillCharactersToBeExcluded();
		cursor = cursor->vinePtr;
	}
	else {
		std::memset(excludedCharacters.data(), 0, std::size(excludedCharacters));
	}
	return c;
}


void updateModel(int c) {
	//printf("%c", c);
	Trie::Node* recentlyUpdatedNodePtr{ basePtr };
	Trie::Node* vineUpdater{ nullptr };
	if (recentlyUpdatedNodePtr->depthInTrie == trie.maxDepth) {
		recentlyUpdatedNodePtr = recentlyUpdatedNodePtr->vinePtr;
	}
	if (auto ptr = recentlyUpdatedNodePtr->find(c); ptr) {//if not nullptr, symbol is present
		ptr->contextCount++;
		if (ptr->contextCount == 255)
			rescaleContextCount(recentlyUpdatedNodePtr);
		basePtr = ptr;
	}
	else {
		basePtr = recentlyUpdatedNodePtr->insert(c);
	}
	vineUpdater = basePtr;

	while (recentlyUpdatedNodePtr->depthInTrie > 0) {//while not at root
		recentlyUpdatedNodePtr = recentlyUpdatedNodePtr->vinePtr;
		if (auto ptr = recentlyUpdatedNodePtr->find(c); ptr) {
			ptr->contextCount++;
			if (ptr->contextCount == 255)
				rescaleContextCount(recentlyUpdatedNodePtr);
			vineUpdater->vinePtr = ptr;
		}
		else {
			vineUpdater->vinePtr = recentlyUpdatedNodePtr->insert(c);
		}
		vineUpdater = vineUpdater->vinePtr;
	}
	//at this point recentlyUpdatedNodePtr will be pointing to the root. All order 1 context symbols
	//have their vine pointig to the root
	auto ptr = recentlyUpdatedNodePtr->find(c);
	ptr->vinePtr = recentlyUpdatedNodePtr;
	cursor = basePtr;
	escapeContext = basePtr->depthInTrie;
}
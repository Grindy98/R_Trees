#include "BTree.h"
#include <iostream>
#include <cassert>
#include <algorithm>
#include <sstream>
#include <queue>

BTree::BTree(IndexFile& idxf, DataFile& dataf)
	:
	idxf(idxf),
	dataf(dataf),
	conv(idxf)
{
	if(idxf.getHeader().entrySize != dataf.entrySize || idxf.getHeader().keySize != dataf.keySize) {
		throw exception("Tree index file doesn't match data file");
	}
	// Check for empty Tree
	if (idxf.getEOFOffset().getVal() <= idxf.getStartOffset().getVal()) {
		root = nullptr;
	}
	else {
		// Initialize the root from file
		root = conv.getNodeFromFile(idxf.getStartOffset());
	}
}

BTree::~BTree()
{
	// Always set the root before deleting
	if (root != nullptr) {
		conv.setNodeToFile(*root, idxf.getStartOffset());
	}
}

bool BTree::isKeyOffset(IndexKey key, Offset& result)
{
	if (root == nullptr) {
		return false;
	}
	shared_ptr<Node> curr = root;
	while (curr) {
		// Perform binary search to find the corresponding key value
		if (curr->search(key, result)) {
			// Found -- exit
			return true;
		}
		// Else check in between the last val and the next
		Offset temp = curr->getChildInBetween(key);
		curr = conv.getNodeFromFile(temp);
	}
	return false;
}

bool BTree::insert(pair<IndexKey, Offset> indexDataPair)
{
	if (root == nullptr) {
		// Special case of insertion
		root = make_shared<Node>(idxf.getHeader().degree, indexDataPair);
		// Also write to file to have the root space reserved
		conv.setNodeToFile(*root, idxf.getStartOffset());
		return true;
	}

	Offset parentOffset = Offset();
	shared_ptr<Node> parent = nullptr;
	// Initially null offset to prevent writes and reads to and from root
	Offset currOffset = Offset();
	shared_ptr<Node> curr = root;
	while(curr) {
		// While we haven't reached the last layer, just move down and split if necessary
		if (curr->isFull()) {
			// We have to split
			auto afterSplit = curr->split();
			if (parent) {
				// If not root -- easy :)
				// One becomes two -> left stays on offset and right goes to a new position
				Offset newNodeOffset = idxf.getEOFOffset();
				conv.setNodeToFile(*get<0>(afterSplit), currOffset);
				conv.setNodeToFile(*get<2>(afterSplit), newNodeOffset);

				// Insert middle node into the parent with the offsets -- if parent is not root also update in file
				parent->insert(get<1>(afterSplit), make_pair(currOffset, newNodeOffset));
				if (parent != root) {
					conv.setNodeToFile(*parent, parentOffset);
				}

				// Reassign curr node val based on the split index val
				if (indexDataPair.first < get<1>(afterSplit).first) {
					// Left side
					curr = get<0>(afterSplit);
				}
				else {
					// Right side
					curr = get<2>(afterSplit);
					currOffset = newNodeOffset;
				}
			}
			else {
				// Root split -- hard :(
				// We have to create a new node that becomes the root, which only has a single element
				// That element is the middle index pair "pushed up" by a split
				// We need to reassign everything, and allocate 2 new spaces for the split nodes instead of 1
				Offset leftNodeOffset = idxf.getEOFOffset();
				Offset rightNodeOffset = idxf.getEOFOffset() + idxf.getEntryByteSize();
				root = make_shared<Node>(idxf.getHeader().degree, get<1>(afterSplit));
				root->setChildren(get<1>(afterSplit).first, make_pair(leftNodeOffset, rightNodeOffset));

				conv.setNodeToFile(*get<0>(afterSplit), leftNodeOffset);
				conv.setNodeToFile(*get<2>(afterSplit), rightNodeOffset);

				// Reassign curr node val based on the split index val
				if (indexDataPair.first < get<1>(afterSplit).first) {
					// Left side
					curr = get<0>(afterSplit);
					currOffset = leftNodeOffset;
				}
				else {
					// Right side
					curr = get<2>(afterSplit);
					currOffset = rightNodeOffset;
				}
			}
		}
		parentOffset = currOffset;
		currOffset = curr->getChildInBetween(indexDataPair.first);

		parent = curr;
		curr = conv.getNodeFromFile(currOffset);
	}
	// Parent stores the leaf where we should insert this, check if unique
	Offset temp;
	if (!parent->search(indexDataPair.first, temp)) {
		parent->insert(indexDataPair);
	}
	else {
		return false;
	}
	// Update after insertions in file if not root
	if(parent != root){
		conv.setNodeToFile(*parent, parentOffset);
	}
	return true;
}

string BTree::toString(bool isHex)
{
	stringstream ret;
	ret << "Tree: \n";
	queue<shared_ptr<Node>> q = queue<shared_ptr<Node>>();
	if (root == nullptr) {
		ret << "Empty Tree" << endl;
		return ret.str();
	}
	q.push(root);
	int qSize = q.size();
	int layer = 1;
	while (!q.empty()) {
		ret << endl << "\tLayer " << layer << endl;
		for (int i = 0; i < qSize; i++) {
			auto n = q.front();
			q.pop();
			ret << n->toString(isHex) << endl;
			for (Offset off : n->getAllChildren()) {
				if (off.notNull()) {
					q.push(conv.getNodeFromFile(off));
				}
			}
		}
		layer++;
		qSize = q.size();
	}
	return ret.str();
}
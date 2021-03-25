#include "Tree.h"
#include <cassert>
#include <algorithm>

Tree::Tree(unique_ptr<DataFile> dataf, string idxFileNameToCreate, int degree)
	:
	dataf(move(dataf)),
	idxf(idxFileNameToCreate, degree, dataf->getDataFileType())
{
}

Tree::Tree(unique_ptr<DataFile> dataf, string indexFileName)
	:
	dataf(move(dataf)),
	idxf(indexFileName)
{
	if (idxf.getHeader().dataFileType != dataf->getDataFileType()) {
		throw exception("Input data file and index file don't match type!\n");
	}
}

void Tree::insert(pair<Rect, Offset> newEntry)
{
	// Read root from file 
	auto root = idxf.readRoot();
	if (root == nullptr) {
		// Empty tree, we have to create the node
		Node n = Node(idxf.getHeader().degree, true);
		n.insert(newEntry.first, newEntry.second);
		idxf.appendNewNode(n);
		return;
	}
	stack<PathIdentifier> insertPath;
	// Add root to the insert path
	insertPath.push(PathIdentifier(root, -1));
	insertWithoutSplit(newEntry, insertPath);

	// Now we have the insertion path, and we can recursively split if necessary
	while (!insertPath.empty()) {
		PathIdentifier currNodeid = insertPath.top();
		insertPath.pop();
		if (currNodeid.parent->isFull()) {
			auto splitRes = currNodeid.parent->split();
			if (insertPath.empty()) {
				// Edge case when the root splits
				// Create empty root in place -- root will NOT be a leaf
				Node newRoot = Node(idxf.getHeader().degree, false);

				// Create 2 new children and append both to the file
				Offset leftChildOff = idxf.appendNewNode(*splitRes[0].first);
				newRoot.insert(splitRes[0].second, leftChildOff);
				Offset rightChildOff = idxf.appendNewNode(*splitRes[1].first);
				newRoot.insert(splitRes[1].second, rightChildOff);

				// Overwrite root in file
				idxf.overwriteNode(newRoot, idxf.getRootOffset());
			}
			else {
				auto& parentid = insertPath.top();
				parentid.setModified();
				// Normal case when a node splits
				// Left node overwrites original
				auto leftChild = parentid.parent->getChild(parentid.childIndex);
				*leftChild.first = splitRes[0].second;
				idxf.overwriteNode(*splitRes[0].first, leftChild.second);
				// Right node gets appended
				Offset rightChildOff = idxf.appendNewNode(*splitRes[1].first);
				parentid.parent->insert(splitRes[1].second, rightChildOff);
			}
		}
		else {
			// Node is not full but we might need to update it regardless
			if (currNodeid.getModificationStatus()) {
				if (insertPath.empty()) {
					// Node that needs modification is root
					idxf.overwriteNode(*currNodeid.parent, idxf.getRootOffset());
				}
				else {
					// Find offset from parent
					assert(!insertPath.top().parent->IsLeaf);
					Offset off = insertPath.top().parent->getChild(insertPath.top().childIndex).second;
					idxf.overwriteNode(*currNodeid.parent, off);
				}
			}
		}
	}
}

vector<DataFile::Entry> Tree::search(Rect searchBox)
{
	vector<DataFile::Entry> found;
	stack<shared_ptr<Node>> nodeCheckStack;
	auto root = idxf.readRoot();
	if (root == nullptr) {
		// Empty tree
		return found;
	}
	nodeCheckStack.push(move(root));
	while (!nodeCheckStack.empty()) {
		shared_ptr<Node> curr = move(nodeCheckStack.top());
		nodeCheckStack.pop();
		// Check which rects overlap
		for (int i = 0; i < curr->getArrSize(); i++)
		{
			auto currChild = curr->getChild(i);
			if (currChild.first->intersects(searchBox)) {
				if (curr->IsLeaf) {
					// If leaf, just read entry from datafile and add it to found
					auto entry = dataf->getEntry(currChild.second);
					found.push_back(entry);
				}
				else {
					// If not leaf, read from file and push child to stack 
					auto newChild = idxf.readNode(currChild.second);
					nodeCheckStack.push(newChild);
				}
			}
		}
	}
	return found;
}

vector<DataFile::Entry> Tree::search(Point searchCenter, double searchRadius)
{
	vector<DataFile::Entry> found;
	stack<shared_ptr<Node>> nodeCheckStack;
	auto root = idxf.readRoot();
	if (root == nullptr) {
		// Empty tree
		return found;
	}
	nodeCheckStack.push(move(root));
	while (!nodeCheckStack.empty()) {
		shared_ptr<Node> curr = move(nodeCheckStack.top());
		nodeCheckStack.pop();
		// Check which rects overlap
		for (int i = 0; i < curr->getArrSize(); i++)
		{
			auto currChild = curr->getChild(i);
			// Check if the circle intersects the rectangle
			Point closest = Point();
			closest.x = std::clamp(searchCenter.x, currChild.first->getDownLeft().x,
				currChild.first->getUpRight().x);
			closest.y = std::clamp(searchCenter.y, currChild.first->getDownLeft().y,
				currChild.first->getUpRight().y);
			closest = closest - searchCenter;
			if (closest.x * closest.x + closest.y * closest.y <= searchRadius * searchRadius) {
				if (curr->IsLeaf) {
					// If leaf, just read entry from datafile and add it to found
					auto entry = dataf->getEntry(currChild.second);
					found.push_back(entry);
				}
				else {
					// If not leaf, read from file and push child to stack 
					auto newChild = idxf.readNode(currChild.second);
					nodeCheckStack.push(newChild);
				}
			}
		}
	}
	return found;
}

void Tree::insertWithoutSplit(pair<Rect, Offset> newEntry, stack<PathIdentifier>& insertPath)
{
	while (true) {
		// The front of the insertPath is the current node without the child index
		// The index has to be found by the current iteration before pushing a new value
		auto& pathid = insertPath.top();
		// If leaf node -- end of search, insert then return
		if (pathid.parent->IsLeaf) {
			pathid.parent->insert(newEntry.first, newEntry.second);
			pathid.setModified();
			return;
		}
		int insertI = pathid.parent->searchFirstIncluded(newEntry.first);
		if (insertI == -1) {
			// We cannot insert directly 
			// Search best node to extend and modify it
			insertI = pathid.parent->searchMinBBExtension(newEntry.first);
			Rect* toExtend = pathid.parent->getChild(insertI).first;
			*toExtend = toExtend->extend(newEntry.first);
			pathid.setModified();
		}
		pathid.childIndex = insertI;
		// Read child from file
		Offset childOff = pathid.parent->getChild(insertI).second;
		auto childNode = idxf.readNode(childOff);
		assert(childNode->getArrSize() >= (int)childNode->Degree);
		insertPath.push(PathIdentifier(childNode, -1));
	}
}

Tree::PathIdentifier::PathIdentifier(shared_ptr<Node> parent, int childIndex)
	:
	parent(parent),
	childIndex(childIndex),
	isModified(false)
{
}

bool Tree::PathIdentifier::getModificationStatus() const
{
	return isModified;
}

void Tree::PathIdentifier::setModified()
{
	isModified = true;
}

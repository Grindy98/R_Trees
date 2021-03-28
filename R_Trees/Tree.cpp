#include "Tree.h"
#include <cassert>
#include <algorithm>
#include <iostream>
#include <iomanip>
#include <cmath>

Tree::Tree(shared_ptr<DataFile> dataf, string idxFileNameToCreate, int degree)
	:
	// Create new tree
	dataf(dataf),
	idxf(idxFileNameToCreate, degree, dataf->getDataFileType())
{
	createTree();
}

Tree::Tree(shared_ptr<DataFile> dataf, string indexFileName)
	:
	// Open existing tree
	dataf(dataf),
	idxf(indexFileName)
{
	if (idxf.getHeader().dataFileType != dataf->getDataFileType()) {
		throw exception("Input data file and index file don't match type!\n");
	}
}

void Tree::insert(pair<Rect, Offset> newEntry)
{
	// Get root from file 
	auto root = idxf.getRoot();
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
				shared_ptr<Node> newRoot = make_shared<Node>(idxf.getHeader().degree, false);

				// Create 2 new children and append both to the file
				Offset leftChildOff = idxf.appendNewNode(*splitRes[0].first);
				newRoot->insert(splitRes[0].second, leftChildOff);
				Offset rightChildOff = idxf.appendNewNode(*splitRes[1].first);
				newRoot->insert(splitRes[1].second, rightChildOff);

				// Overwrite root in file
				idxf.setRoot(newRoot);
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
				if (!insertPath.empty()) {
					// Node that needs modification is not root, which is already modified
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
	auto root = idxf.getRoot();
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

vector<DataFile::Entry> Tree::search(Point searchCenter, double searchRadius, bool isDistanceKM)
{
	vector<DataFile::Entry> found;
	stack<shared_ptr<Node>> nodeCheckStack;
	auto root = idxf.getRoot();
	nodeCheckStack.push(move(root));
	while (!nodeCheckStack.empty()) {
		shared_ptr<Node> curr = move(nodeCheckStack.top());
		nodeCheckStack.pop();
		// Check which rects overlap
		for (int i = 0; i < curr->getArrSize(); i++)
		{
			auto currChild = curr->getChild(i);
			// Check if the circle intersects the rectangle
			if (doesCircleIntersect(*currChild.first, searchCenter, searchRadius, isDistanceKM)) {
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

vector<DataFile::Entry> Tree::search(Point searchCenter, double searchRadius, bool isDistanceKM, string tag)
{
	if (!idxf.doesTagExist(tag)) {
		throw exception("Tag does not exist!\n");
	}
	auto allTags = search(searchCenter, searchRadius, isDistanceKM);
	vector<DataFile::Entry> ret;
	for (const auto& entry : allTags) {
		if (entry.tag == tag) {
			ret.push_back(entry);
		}
	}
	return ret;
}

void Tree::createTree()
{
	int64_t size = dataf->getNumberOfElements().get();
	for (int64_t i = 0; i < size; i++)
	{
		auto entry = dataf->getEntry(Offset(i));
		// Add entry to tree
		insert(make_pair(entry.BB, Offset(i)));
		// Add tag if not already added 
		idxf.insertTagIfUnique(entry.tag);
		if (i % 5000 == 0) {
			cout << "Progress " << std::setw(10) << i << "/" << size << endl;
		}
	}
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

bool Tree::doesCircleIntersect(Rect rect, Point circleC, double circleR, bool isDistanceKM)
{
	static const double EarthRad = 6371.0;
	static const double pi = 2 * acos(0.0);

	Point closest = Point();
	closest.x = std::clamp(circleC.x, rect.getDownLeft().x,
		rect.getUpRight().x);
	closest.y = std::clamp(circleC.y, rect.getDownLeft().y,
		rect.getUpRight().y);
	
	if (!isDistanceKM) {
		closest = closest - circleC;
		return closest.x * closest.x + closest.y * closest.y <= circleR * circleR;
	}
	
	const double long2 =	circleC.x * pi / 180;
	const double long1 =	closest.x * pi / 180;
	const double lat2 =		circleC.y * pi / 180;
	const double lat1 =		closest.y * pi / 180;
	const double x = (long2 - long1) * cos((lat2 + lat1) / 2);
	const double y = (lat2 - lat1);
	const double d_sqr = (x * x + y * y) * EarthRad * EarthRad;
	// Find the ratio between d and circleR to get a multiplying factor
	closest = closest - circleC;
	const double distanceFactor_sqr = circleR * circleR / d_sqr;
	return 1.0 <= distanceFactor_sqr;
	
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
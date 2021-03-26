#pragma once
#include "IndexFile.h"
#include "DataFile.h"
#include <stack>
class Tree
{
public:
	Tree(shared_ptr<DataFile> dataf, string idxFileNameToCreate, int degree);
	Tree(shared_ptr<DataFile> dataf, string indexFileName);

	void insert(pair<Rect, Offset> newEntry);
	vector<DataFile::Entry> search(Rect searchBox);
	vector<DataFile::Entry> search(Point searchCenter, double searchRadius);
	vector<DataFile::Entry> search(Point searchCenter, double searchRadius, string tag);


private:
	IndexFile idxf;
	shared_ptr<DataFile> dataf;

	void createTree();

	struct PathIdentifier {
		PathIdentifier(shared_ptr<Node> parent, int childIndex);
		shared_ptr<Node> parent;
		int childIndex;
		bool getModificationStatus() const;
		void setModified();
	private:
		bool isModified;
	};
	void insertWithoutSplit(pair<Rect, Offset> newEntry, stack<PathIdentifier>& insertPath);
};


#pragma once
#include "IndexFile.h"
#include "DataFile.h"
#include <stack>
class Tree
{
public:
	Tree(unique_ptr<DataFile> dataf, string idxFileNameToCreate, int degree);
	Tree(unique_ptr<DataFile> dataf, string indexFileName);

	void insert(pair<Rect, Offset> newEntry);


private:
	unique_ptr<DataFile> dataf;
	IndexFile idxf;

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


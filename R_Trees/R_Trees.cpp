#include <iostream>
#include <chrono>
#include <random>
#include "Tree.h"
#include "ShapeFile.h"
#include "TextFile.h"

unique_ptr<Tree> createTree(string idxFilePath, string dbFilePath, bool isShapeFile) {
    auto start = std::chrono::high_resolution_clock::now();
    shared_ptr<DataFile> dbf; 
    unique_ptr<Tree> ptr;
    try
    {
        if (isShapeFile) {
            dbf = make_shared<ShapeFile>(dbFilePath);
        }
        else {
            dbf = make_shared<TextFile>(dbFilePath);
        }
        ptr = make_unique<Tree>(dbf, idxFilePath, 25);
    }
    catch (const std::exception& e)
    {
        cout << e.what();
        system("pause");
        exit(1);
    }
    auto finish = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double> elapsed = finish - start;
    cout << "Successfully created index file!" << endl;
    std::cout << "Elapsed time: " << elapsed.count() << " s" << endl;
    return ptr;
}

unique_ptr<Tree> readTree(string idxFilePath, string dbFilePath, bool isShapeFile) {
    shared_ptr<DataFile> dbf;
    unique_ptr<Tree> ptr;
    try
    {
        if (isShapeFile) {
            dbf = make_shared<ShapeFile>(dbFilePath);
        }
        else {
            dbf = make_shared<TextFile>(dbFilePath);
        }
        ptr = make_unique<Tree>(dbf, idxFilePath);
    }
    catch (const std::exception& e)
    {
        cout << e.what();
        system("pause");
        exit(1);
    }
    cout << "Successfully opened index file!" << endl;
    return ptr;
}

void findEntries(Tree& tree, Point coords, double range, bool isKM, string tag) {
    vector<DataFile::Entry> res;
    try {
        res = tree.search(coords, range, isKM, tag);
    }
    catch (const std::exception& e)
    {
        cout << e.what();
        system("pause");
        exit(1);
    }
    for (int i = 0; i < res.size(); i++) {
        std::cout << res[i].toString(i + 1) << endl;
    }
}

int main() {

    // Create / Read parameters
    bool willCreate = false;
    bool isShapeFileDir = true;
    string pathToDir = "D:\\Uni_Stuff\\Y2\\ADA\\ExtraProjects\\R_Trees\\romania-latest-free";
    string pathToIdx = "D:\\Uni_Stuff\\Y2\\ADA\\ExtraProjects\\R_Trees\\idxro";

    // Find parameters
    Point timisoaraPosition = { 21.2087, 45.7489 };
    bool isKM = true;
    double radius = 1;
    string tag = "pharmacy";

    unique_ptr<Tree> tree;
    if (willCreate) {
        tree = createTree(pathToIdx, pathToDir, isShapeFileDir);
    }
    else {
        tree = readTree(pathToIdx, pathToDir, isShapeFileDir);
    }

    system("pause");

    findEntries(*tree, timisoaraPosition, radius, isKM, tag);

    system("pause");

    return 0;
}
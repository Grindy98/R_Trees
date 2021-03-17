// B_Trees.cpp : This file contains the 'main' function. Program execution begins and ends there.
//
#include "BTree.h"
#include <iostream>
#include <chrono>

void createindex(string datafile, int recordsize, int keysize) {
    DataFile dataf(datafile, keysize, recordsize);
    IndexFile idx(datafile + ".ndx", keysize, recordsize);
    cout << endl << "BTree order for maximum disk read efficiency: " << idx.getHeader().degree << endl << endl;
    BTree tree(idx, dataf);

    for (Offset off = Offset(); off.getVal() < dataf.getEOFOffset().getVal(); off = off + dataf.entrySize) {
        auto entry = dataf.getEntryAtOffset(off);
        tree.insert(make_pair(entry.first, off));
    }
}

bool addrecord(string datafile, IndexKey key, BinaryStorage value) {
    IndexFile idx(datafile + ".ndx");
    DataFile dataf(datafile, idx);
    BTree tree(idx, dataf);
    
    // Check for uniqueness of key
    Offset temp;
    if (tree.isKeyOffset(key, temp)) {
        return false;
    }
    Offset newEntryOff = dataf.addNewEntry(make_pair(key, value));
    return tree.insert(make_pair(key, newEntryOff));
}

bool searchrecord(string datafile, IndexKey key, BinaryStorage& ret) {
    IndexFile idx(datafile + ".ndx");
    DataFile dataf(datafile, idx);
    BTree tree(idx, dataf);

    Offset off;
    if (tree.isKeyOffset(key, off)) {
        ret = dataf.getEntryAtOffset(off).second;
        return true;
    }
    return false;  
}

bool updaterecord(string datafile, IndexKey key, BinaryStorage value) {
    IndexFile idx(datafile + ".ndx");
    DataFile dataf(datafile, idx);
    BTree tree(idx, dataf);

    Offset off;
    if (tree.isKeyOffset(key, off)) {
        dataf.setEntryAtOffset(value, off);
        return true;
    }
    return false;
}

void testIndexing(string filename, int recordsize, int keysize, bool recreateIndex) {
    if (recreateIndex) {
        createindex(filename, recordsize, keysize);

    }
    IndexFile idx(filename + ".ndx");
    DataFile dataf(filename, idx);
    BTree tree(idx, dataf);
    srand((int)time(0));
    // Choose 10 random positions in the datafile and check search function
    for (int i = 0; i < 10; i++) {
        int64_t off = (rand() % dataf.getEOFOffset().getVal());
        off -= off % dataf.entrySize;
        IndexKey key = dataf.getEntryAtOffset(off).first;
        BinaryStorage searchRes(recordsize - keysize);
        if (searchrecord(filename, key, searchRes)) {
            cout << "FOUND, with value: " << searchRes.toString(false) << endl;
        }
        else {
            cout << "NOT FOUND" << endl;
        }
    }

    const vector<unsigned char> sevens(recordsize - keysize, 0x7);
    // Choose 10 random positions in the datafile and check update function
    for (int i = 0; i < 10; i++) {
        int64_t off = (rand() % dataf.getEOFOffset().getVal());
        off -= off % dataf.entrySize;
        auto initial = dataf.getEntryAtOffset(off);
        // Set BinaryStorage to sevens and recheck
        BinaryStorage updatedVal(&sevens[0], recordsize - keysize);
        updaterecord(filename, initial.first, updatedVal);
        BinaryStorage searchRes(recordsize - keysize);
        if (searchrecord(filename, initial.first, searchRes)) {
            cout << "FOUND, with value: " << searchRes.toString(true) << endl;
            // Reset the original value to restore file
            updaterecord(filename, initial.first, initial.second);
        }
        else {
            cout << "NOT FOUND!" << endl;
        }
    }
    cout << endl;
    //// Fabricate potential unique key and set val to sevens for brevity
    //vector<unsigned char> key(keysize);
    //for (unsigned i = 0; i < key.size(); i++) {
    //    key[i] = (unsigned char)(rand() % 256);
    //}
    //if (addrecord(filename, IndexKey(&key[0], keysize), BinaryStorage(&sevens[0], recordsize - keysize))) {
    //    cout << "Successfully added!" << endl << "Searching for added value:" << endl;
    //    BinaryStorage searchRes(recordsize - keysize);
    //    if (searchrecord(filename, IndexKey(&key[0], keysize), searchRes)) {
    //        cout << "FOUND, with value: " << searchRes.toString(true) << endl;
    //    }
    //    else {
    //        cout << "NOT FOUND!" << endl;
    //    }
    //}
    //else {
    //    cout << "KEY NOT UNIQUE -- Insertion not possible!" << endl;
    //}
}

int main(){

    cout << "Insert data file for testing with following format:\n";
    cout << "\t<filename> <recordsize> <keysize>" << endl << "\t";
    string filename;
    int recordsize = -1, keysize = -1;
    cin >> filename >> recordsize >> keysize;
    if (recordsize <= 0 || keysize <= 0) {
        cout << "Invalid parameters" << endl;
    }
    else {
        auto start = chrono::high_resolution_clock::now();
        try {
            testIndexing(filename, recordsize, keysize, true);
        }
        catch (exception e) {
            cerr << e.what();
        }

        auto stop = chrono::high_resolution_clock::now();
        auto duration = chrono::duration_cast<chrono::milliseconds>(stop - start);
        cout << endl << "Time duration for execution: " << duration.count() << " ms" << endl;
    }

    //try {
    //    testIndexing("res\\small.dat", 32, 1, true);
    //}
    //catch (exception e) {
    //    cerr << e.what();
    //    return 1;
    //}

    //try {
    //    testIndexing("res\\medium.dat", 84, 4, true);
    //}
    //catch (exception e) {
    //    cerr << e.what();
    //    return 1;
    //}

    //try {
    //    testIndexing("res\\large.dat", 109, 7, true);
    //}
    //catch (exception e) {
    //    cerr << e.what();
    //}   

    system("pause");

    return 0;
}


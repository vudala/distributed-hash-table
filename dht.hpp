#pragma once

#include <bits/stdc++.h>

using namespace std;

class DHTNode {
public:
    unsigned index;
    
    unsigned prev;
    unsigned next;

    bool first;
    
    vector<unsigned> fingertb;
    set<unsigned> keys;

    // constructor
    DHTNode(){};
    DHTNode(unsigned index, bool first) {
        this->index = index;

        this->prev = index;
        this->next = index;

        this->fingertb = vector<unsigned> (32, 0xFFFFFFFF);

        this->first = first;
    };

    // destructor
    ~DHTNode(){};
};

void join(map<unsigned, DHTNode>& chord, unsigned index);

void leave(map<unsigned, DHTNode>& chord, unsigned index);

void insert(map<unsigned, DHTNode>& chord, unsigned index, unsigned key);

void lookup(map<unsigned, DHTNode>& chord, unsigned index, unsigned key, unsigned timestamp);

void print_ftb(DHTNode& node);

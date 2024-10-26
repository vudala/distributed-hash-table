#pragma once

#include <bits/stdc++.h>

using namespace std;

class DHTNode {
public:
    unsigned index;

    unsigned lower_lim;
    
    unsigned prev;
    unsigned next;

    bool last;
    
    vector<unsigned> fingertb;
    set<unsigned> keys;

    // constructor
    DHTNode(){};
    DHTNode(unsigned index, bool last) {
        this->index = index;

        this->prev = index;
        this->next = index;

        this->lower_lim = 0x7FFFFFFF;

        this->fingertb = vector<unsigned> (32, index);

        this->last = last;
    };

    // destructor
    ~DHTNode(){};
};

void join(map<unsigned, DHTNode>& chord, unsigned index);

void leave(map<unsigned, DHTNode>& chord, unsigned index);

void insert(map<unsigned, DHTNode>& chord, unsigned index, unsigned key);

void lookup(map<unsigned, DHTNode>& chord, unsigned index, unsigned key);

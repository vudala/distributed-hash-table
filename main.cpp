#include <bits/stdc++.h>

#include "dht.hpp"

map<unsigned, DHTNode> chord;


typedef struct {
    unsigned timestamp;
    char op;
    unsigned index;
    unsigned key;
} Operation;

int main()
{

    vector<Operation> ops;

    unsigned timestamp, index, key;
    char op, trash;

    while (cin >> timestamp) {
        cin >> op;
        cin >> index;

        if (op == 'L' || op == 'I')
            cin >> key;
        else
            cin >> trash;
            
        ops.push_back({timestamp, op, index, key});
    }

    sort(
        ops.begin(), ops.end(),
        [](Operation a, Operation b){ return a.timestamp < b.timestamp;}
    );

    for (Operation op : ops) {
        if (op.op == 'E') {
            join(chord, op.index);
        }
        else if (op.op == 'S') {
            leave(chord, op.index);
        }
        else if (op.op == 'I') {
            insert(chord, op.index, op.key);
        }
        else if (op.op == 'L') {
            lookup(chord, op.index, op.key, op.timestamp);
        }
    }
}
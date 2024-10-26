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

    chord[1] = DHTNode(1, true);

    for (Operation op : ops) {
        if (op.op == 'E') {
            join(chord, op.index);
        }
        else if (op.op == 'S') {
            leave(chord, op.index);
        }
        else if (op.op == 'I') {
            insert(chord, op.index, key);
        }
        else if (op.op == 'L') {
            lookup(chord, op.index, key);
        }

        for (auto itr = chord.begin(); itr != chord.end(); itr++) {
            DHTNode n = (*itr).second;

            cout << n.index << ' ' << n.prev << ' ' << n.next;
            cout << endl;
        }
        cout << endl;
    }


    for (auto itr = chord.begin(); itr != chord.end(); itr++) {
        DHTNode node = (*itr).second;
        set<unsigned> keys = node.keys;
        cout << node.index << " : ";
        for (auto itr1 = keys.begin(); itr1 != keys.end(); itr1++) {
            cout << (*itr1) << ' ';
        }
        cout << endl;
    }
    
}
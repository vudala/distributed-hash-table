#pragma once

#include <bits/stdc++.h>
#include "dht.hpp"

using namespace std;

int log2(int x)
{
    int targetlevel = 0;
    while (x >>= 1) ++targetlevel;
    return x;
}

unsigned pow2(unsigned power)
{
    return 1 << power;
}

// join
// leave
// insert
// lookup

// Retorna o index do dois nodos entre o index
pair<unsigned, unsigned> get_bounds(map<unsigned, DHTNode>& chord, unsigned index){
    map<unsigned, DHTNode>::iterator p_itr;
    for (auto itr = chord.begin(); itr != chord.end(); itr++) {
        auto el = *(itr);
        if (el.first > index) {
            return {(*p_itr).first, (*itr).first};
        }
        p_itr = itr;
    }
    return {(*p_itr).first, (*(p_itr)).second.next};
}


void update_fingertb(map<unsigned, DHTNode>& chord)
{
    for (auto itr = chord.begin(); itr != chord.end(); itr++) {
        DHTNode n = (*itr).second;

        for(unsigned i = 0; i < 32; i++){
            auto [a, b] = get_bounds(chord, n.index + pow2(i));
            n.fingertb[i] = b;
            //cout << b << ' ' ;
        }

        //cout << endl;
    }
    //cout << endl;
}


void join(map<unsigned, DHTNode>& chord, unsigned index)
{   
    // Determina a nova vizinhança do nodo que está entrando
    auto [prev, next] = get_bounds(chord, index);

    cout << prev << ' ' << next << endl;
    DHTNode& prev_node = chord[prev];
    DHTNode& next_node = chord[next];

    // Cria o novo nodo
    DHTNode new_node = DHTNode(index, false);

    // Se o vizinho anterior era o ultimo, agora o novo nodo é o ultimo
    if (prev_node.last) {
        prev_node.last = false;
        new_node.last = true;
    }

    // Atualiza os limites dos nodos
    new_node.prev = prev;
    new_node.next = next;

    prev_node.next = index;
    next_node.prev = index;

    new_node.lower_lim = prev + 1;
    next_node.lower_lim = index + 1;

    // Rearranja os valores entre os nodos
    set<unsigned>& keys = next_node.keys;
    vector<unsigned> to_erase = vector<unsigned> (0);
    for (auto itr = keys.begin(); itr != keys.end(); itr++) {
        unsigned x = *(itr);
        if (x < next_node.lower_lim) {
            new_node.keys.insert(x);
            to_erase.push_back(x);
        }
    }

    for (auto x : to_erase)
        keys.erase(x);

    chord[index] = new_node;

    update_fingertb(chord);
}


void leave(map<unsigned, DHTNode>& chord, unsigned index)
{   
    // Determina a vizinhança do nodo que está entrando
    DHTNode& node = chord[index];
    DHTNode& prev_node = chord[node.prev];
    DHTNode& next_node = chord[node.next];
 
    // Se o nodo era o ultimo, agora o anterior é o ultimo
    if (node.last) {
        prev_node.last = true;
        node.last = true;
    }

    // Atualiza as vizinhanças
    prev_node.next = node.next;
    next_node.prev = node.prev;

    next_node.lower_lim = node.prev + 1;

    // Rearranja os valores entre os nodos
    set<unsigned>& keys = node.keys;
    for (auto itr = keys.begin(); itr != keys.end(); itr++)
        next_node.keys.insert(*itr);

    chord.erase(index);

    update_fingertb(chord);
}


void insert(map<unsigned, DHTNode>& chord, unsigned index, unsigned key)
{
    // Determina a nova vizinhança do nodo que está entrando
    auto [prev, next] = get_bounds(chord, key);
    DHTNode& prev_node = chord[prev];
    DHTNode& next_node = chord[next];

    if (prev == key)
        prev_node.keys.insert(key);
    else
        next_node.keys.insert(key);
}


void lookup(map<unsigned, DHTNode>& chord, unsigned index, unsigned key)
{
    return;
    // Determina a nova vizinhança do nodo que está entrando
    auto [prev, next] = get_bounds(chord, key);
    DHTNode& prev_node = chord[prev];
    DHTNode& next_node = chord[next];

    // if (prev == key)
    //     prev_node.keys.insert(key);
    // else
    //     next_node.keys.insert(key);
}
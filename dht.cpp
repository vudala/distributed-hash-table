#pragma once

#include <bits/stdc++.h>
#include "dht.hpp"

using namespace std;


unsigned first = 0;
unsigned max_index = 0;

unsigned log2(unsigned x)
{
    unsigned target = 0;
    while (x >>= 1) target++;
    return target;
}

unsigned pow2(unsigned power)
{
    return 1 << power;
}

// Retorna o index do dois nodos entre o index
pair<unsigned, unsigned> get_bounds(map<unsigned, DHTNode>& chord, unsigned index)
{
    map<unsigned, DHTNode>::iterator itr;
    pair<unsigned, DHTNode> el;
    for (itr = chord.begin(); itr != chord.end(); itr++) {
        el = (*itr);
        if (el.first >= index) {
            return {el.second.prev, el.first};
        }
    }
    return {el.first, el.second.next};
}

// Atualiza a finger table de um nodo
void update_fingertb(map<unsigned, DHTNode>& chord)
{
    for (auto itr = chord.begin(); itr != chord.end(); itr++) {
        DHTNode& n = (*itr).second;
        for(unsigned i = 0; i < log2(max_index) + 1; i++){
            auto [a, b] = get_bounds(
                chord,
                (n.index + pow2(i)) % pow2(log2(max_index) + 1)
            );

            n.fingertb[i] = b;
        }
    }
}


void join(map<unsigned, DHTNode>& chord, unsigned index)
{
    if (index > max_index)
        max_index = index;

    if (chord.size() == 0) {
        chord[index] = DHTNode(index, true);
        first = index;
        update_fingertb(chord);
        return;
    }

    // Determina a nova vizinhança do nodo que está entrando
    auto [prev, next] = get_bounds(chord, index);

    DHTNode& prev_node = chord[prev];
    DHTNode& next_node = chord[next];

    // Cria o novo nodo
    DHTNode new_node = DHTNode(index, false);

    if (next_node.first && next_node.index > index) {
        first = index;
        next_node.first = false;
        new_node.first = true;
    }

    // Atualiza os limites dos nodos
    new_node.prev = prev;
    new_node.next = next;

    prev_node.next = index;
    next_node.prev = index;

    // Rearranja os valores entre os nodos
    set<unsigned>& keys = next_node.keys;
    vector<unsigned> to_erase = vector<unsigned> (0);
    for (auto itr = keys.begin(); itr != keys.end(); itr++) {
        unsigned x = *(itr);
        if (x < index + 1) {
            new_node.keys.insert(x);
            to_erase.push_back(x);
        }
    }
    for (auto x : to_erase)
        keys.erase(x);

    // Adiciona o nodo na rede
    chord[index] = new_node;
    update_fingertb(chord);

    cout << first << endl;
}


void leave(map<unsigned, DHTNode>& chord, unsigned index)
{
    // Se é o maior nodo que está saindo, o anterior a ele agora é o maior
    if (index == max_index)
        max_index = chord[max_index].prev;

    // Determina a vizinhança do nodo que está saindo
    DHTNode& node = chord[index];
    DHTNode& prev_node = chord[node.prev];
    DHTNode& next_node = chord[node.next];
 
    // Se o nodo era o primeiro, agora o prox é o primeiro
    if (node.first) {
        first = next_node.index;
        next_node.first = true;
        node.first = false;
    }

    // Atualiza as vizinhanças
    prev_node.next = node.next;
    next_node.prev = node.prev;

    // Rearranja os valores entre os nodos
    set<unsigned>& keys = node.keys;
    for (auto itr = keys.begin(); itr != keys.end(); itr++)
        next_node.keys.insert(*itr);

    // Apaga ele da rede
    chord.erase(index);
    update_fingertb(chord);

    cout << first << endl;
}


void print_ftb(DHTNode& node)
{
    cout << '{';
    unsigned i;
    for (i = 0; i <= log2(max_index) - 1; i++)
        cout << node.fingertb[i] << ',';
    cout << node.fingertb[i] << '}';
}

// Retorna diferença absoluta
unsigned diff(unsigned a, unsigned b)
{
    return (a > b) ? a - b : b - a;
}

// Retorna o indice mais proximo de key na fingertb
unsigned get_entry(map<unsigned, DHTNode>& chord, unsigned index, unsigned key)
{
    DHTNode node = chord[index];

    unsigned min_diff = 0xFFFFFFFF;
    unsigned k = 0;

    // Atravessa a ftb procurando a entrada com menor diff
    for (unsigned i = 0; i < (log2(max_index) + 1); i++) {
        unsigned d = diff(key, node.fingertb[i]);
        if (d < min_diff) {
            min_diff = d;
            k = i;
        }
    };

    return node.fingertb[k];
}

// Determina se a chave pertence ao nodo
bool belongs_to(map<unsigned, DHTNode>& chord, unsigned target, unsigned key)
{
    DHTNode node = chord[target];

    if (node.first && node.prev < key)
        return true;

    return node.index >= key;
}


// Insere um valor na rede
void insert(map<unsigned, DHTNode>& chord, unsigned index, unsigned key)
{
    if (key > max_index)
        max_index = key;

    update_fingertb(chord);

    unsigned prev_target;
    unsigned target = index;
    while(!belongs_to(chord, target, key)) {
        prev_target = target;
        target = get_entry(chord, target, key);

        if (prev_target > target) {
            target = chord[prev_target].next;
            break;
        }
    }

    chord[target].keys.insert(key);
}


// Procura por um valor na rede
void lookup(map<unsigned, DHTNode>& chord, unsigned index, unsigned key, unsigned timestamp)
{
    vector<unsigned> jumps;

    unsigned target = index;
    jumps.push_back(target);
    
    // Enquanto não achou o dono, fica pulando
    while(!belongs_to(chord, target, key)) {
        target = get_entry(chord, target, key);
        jumps.push_back(target);
    }

    // Imprime
    cout << timestamp << " L " << key << " " << '{';
    unsigned i;
    for (i = 0; i < jumps.size() - 1; i++)
        cout << jumps[i] << ',';
    cout << jumps[i] << '}' << endl;

    for (unsigned j : jumps) {
        cout << timestamp << " T " << j << ' ';
        print_ftb(chord[j]);
        cout << endl;
    }
}
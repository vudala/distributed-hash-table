#pragma once

#include <bits/stdc++.h>
#include "dht.hpp"

using namespace std;


unsigned first = 0;
unsigned max_node = 0;

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
        for(unsigned i = 0; i < log2(max_node) + 1; i++){
            auto [a, b] = get_bounds(
                chord,
                (n.index + pow2(i)) % pow2(log2(max_node) + 1)
            );

            n.fingertb[i] = b;
        }
    }
}


void join(map<unsigned, DHTNode>& chord, unsigned index)
{
    // Atualiza o nodo máx
    if (index > max_node)
        max_node = index;

    // Se a rede ainda está vazia
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

    // Se estiver sendo inserido antes do primeiro nodo, ele agora é o primeiro
    if (next_node.first && next_node.index > index) {
        first = index;
        next_node.first = false;
        new_node.first = true;
    }

    // Atualiza os vizinhoss dos nodos
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
}


void leave(map<unsigned, DHTNode>& chord, unsigned index)
{
    // Se é o maior nodo que está saindo, o anterior a ele agora é o maior
    if (index == max_node)
        max_node = chord[max_node].prev;

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
}


void print_ftb(DHTNode& node)
{
    cout << '{';
    unsigned i;
    for (i = 0; i <= log2(max_node) - 1; i++)
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
    for (unsigned i = 0; i < (log2(max_node) + 1); i++) {
        unsigned d = diff(key, node.fingertb[i]);
        if (d < min_diff) {
            min_diff = d;
            k = i;
        }
    };

    // Caso a primeira entrada da finger table (prox nodo), seja menor que o nodo
    // atual, significa que está loopando
    if (node.fingertb[0] < index)
        k = 0;

    return node.fingertb[k];
}

// Determina se a chave pertence ao nodo
bool belongs_to(map<unsigned, DHTNode>& chord, unsigned target, unsigned key)
{
    DHTNode node = chord[target];

    if (key == target)
        return true;

    if (key < target && key > node.prev)
        return true;

    if (key > max_node && node.first)
        return true;

    return false;
}


// Procura por um valor na rede
void insert(map<unsigned, DHTNode>& chord, unsigned index, unsigned key)
{
    vector<unsigned> jumps;

    unsigned target = index;

    // Enquanto não achou o dono, fica pulando
    while(!belongs_to(chord, target, key)) {
        target = get_entry(chord, target, key);
    }

    // Atribui o valor
    DHTNode& node = chord[target];
    node.keys.insert(index);
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

#include <iostream>
#include "../MusicClass/music.hpp"
#pragma once

#define ORDER 4
#define FILE_NAME "arquivo_de_dados_binario"

class BTreeNode {
public:
    // Vetor De Chaves (agora do tipo music)
    music keys[ORDER - 1]; 
    // Vetor de ponteiros para crianças
    BTreeNode* children[ORDER]; 
    // Número atual de chaves
    int n;
    bool leaf; 

    BTreeNode(bool isLeaf = true) : n(0), leaf(isLeaf) {
        for (int i = 0; i < ORDER; i++)
            children[i] = nullptr;
    }
};

class BTree {
private:
    BTreeNode* root; 

    void clear(BTreeNode* node) {
        if (node != nullptr) {
            if (!node->leaf) {
                for (int i = 0; i <= node->n; i++) {
                    clear(node->children[i]);
                }
            }
            delete node;
        }
    }

    // Split de um nó criança
    // Split clássico ajustado para cópia segura de objetos com string no C++
    void splitChild(BTreeNode* x, int i) {
        BTreeNode* y = x->children[i];
        BTreeNode* z = new BTreeNode(y->leaf);
        
        // Grau mínimo (t) para ORDER 4 será 2
        int t = ORDER / 2; 

        z->n = t - 1;

        // Copia as chaves do final de Y para o início de Z
        for (int j = 0; j < t - 1; j++) {
            z->keys[j] = y->keys[j + t];
        }

        // Copia os filhos correspondentes, se não for folha
        if (!y->leaf) {
            for (int j = 0; j < t; j++) {
                z->children[j] = y->children[j + t];
                y->children[j + t] = nullptr; // Limpa o ponteiro antigo
            }
        }

        y->n = t - 1;

        // Move os filhos do pai X para abrir espaço para Z
        for (int j = x->n; j >= i + 1; j--) {
            x->children[j + 1] = x->children[j];
        }

        x->children[i + 1] = z;

        // Move as chaves do pai X para abrir espaço para a chave que vai subir
        for (int j = x->n - 1; j >= i; j--) {
            x->keys[j + 1] = x->keys[j];
        }

        // Sobe a chave do meio para o pai X
        x->keys[i] = y->keys[t - 1];
        x->n = x->n + 1;
    }
    
    // Inserir em um nó não cheio (Recebe a música inteira)
    void insertNonFull(BTreeNode* x, music k) {
        int i = x->n - 1;

        if (x->leaf) {
            while (i >= 0 && k.getid() < x->keys[i].getid()) {
                x->keys[i + 1] = x->keys[i];
                i--;
            }

            x->keys[i + 1] = k;
            x->n = x->n + 1;
        } else {
            while (i >= 0 && k.getid() < x->keys[i].getid())
                i--;

            i++;
            if (x->children[i]->n == ORDER - 1) {
                splitChild(x, i);

                if (k.getid() > x->keys[i].getid())
                    i++;
            }
            insertNonFull(x->children[i], k);
        }
    }

    // Procura uma chave (Recebe apenas o ID)
    BTreeNode* search(BTreeNode* x, int id) {
        int i = 0;
        while (i < x->n && id > x->keys[i].getid())
            i++;

        if (i < x->n && id == x->keys[i].getid())
            return x;

        if (x->leaf)
            return nullptr;

        return search(x->children[i], id);
    }

    music getPredecessor(BTreeNode* node, int idx) {
        BTreeNode* current = node->children[idx];
        while (!current->leaf)
            current = current->children[current->n];
        return current->keys[current->n - 1];
    }

    music getSuccessor(BTreeNode* node, int idx) {
        BTreeNode* current = node->children[idx + 1];
        while (!current->leaf)
            current = current->children[0];
        return current->keys[0];
    }

    // Encher um nó criança
    void fill(BTreeNode* node, int idx) {
        if (idx != 0 && node->children[idx - 1]->n >= ORDER / 2)
            borrowFromPrev(node, idx);
        else if (idx != node->n && node->children[idx + 1]->n >= ORDER / 2)
            borrowFromNext(node, idx);
        else {
            if (idx != node->n)
                merge(node, idx);
            else
                merge(node, idx - 1);
        }
    }

    // Empresta do anterior
    void borrowFromPrev(BTreeNode* node, int idx) {
        BTreeNode* child = node->children[idx];
        BTreeNode* sibling = node->children[idx - 1];

        for (int i = child->n - 1; i >= 0; --i)
            child->keys[i + 1] = child->keys[i];

        if (!child->leaf) {
            for (int i = child->n; i >= 0; --i)
                child->children[i + 1] = child->children[i];
        }

        child->keys[0] = node->keys[idx - 1];

        if (!child->leaf)
            child->children[0] = sibling->children[sibling->n];

        node->keys[idx - 1] = sibling->keys[sibling->n - 1];

        child->n += 1;
        sibling->n -= 1;
    }

    // Empresta da frente
    void borrowFromNext(BTreeNode* node, int idx) {
        BTreeNode* child = node->children[idx];
        BTreeNode* sibling = node->children[idx + 1];

        child->keys[child->n] = node->keys[idx];

        if (!child->leaf)
            child->children[child->n + 1] = sibling->children[0];

        node->keys[idx] = sibling->keys[0];

        for (int i = 1; i < sibling->n; ++i)
            sibling->keys[i - 1] = sibling->keys[i];

        if (!sibling->leaf) {
            for (int i = 1; i <= sibling->n; ++i)
                sibling->children[i - 1] = sibling->children[i];
        }

        child->n += 1;
        sibling->n -= 1;
    }

    // Merge
    void merge(BTreeNode* node, int idx) {
        BTreeNode* child = node->children[idx];
        BTreeNode* sibling = node->children[idx + 1];

        child->keys[ORDER / 2 - 1] = node->keys[idx];

        for (int i = 0; i < sibling->n; ++i)
            child->keys[i + ORDER / 2] = sibling->keys[i];

        if (!child->leaf) {
            for (int i = 0; i <= sibling->n; ++i)
                child->children[i + ORDER / 2] = sibling->children[i];
        }

        for (int i = idx + 1; i < node->n; ++i)
            node->keys[i - 1] = node->keys[i];

        for (int i = idx + 2; i <= node->n; ++i)
            node->children[i - 1] = node->children[i];

        child->n += sibling->n + 1;
        node->n--;

        delete sibling;
    }

    // Remoção de não-folha
    void removeFromNonLeaf(BTreeNode* node, int idx) {
        int id = node->keys[idx].getid(); // Pega o ID da música atual

        if (node->children[idx]->n >= ORDER / 2) {
            music pred = getPredecessor(node, idx);
            node->keys[idx] = pred;
            remove(node->children[idx], pred.getid()); // Passa o ID do predecessor
        } else if (node->children[idx + 1]->n >= ORDER / 2) {
            music succ = getSuccessor(node, idx);
            node->keys[idx] = succ;
            remove(node->children[idx + 1], succ.getid()); // Passa o ID do sucessor
        } else {
            merge(node, idx);
            remove(node->children[idx], id); // Passa o ID original
        }
    }

    // Remoção de folha
    void removeFromLeaf(BTreeNode* node, int idx) {
        for (int i = idx + 1; i < node->n; ++i)
            node->keys[i - 1] = node->keys[i];

        node->n--;
    }

    // Remove (Recebe apenas o ID)
    void remove(BTreeNode* node, int id) {
        int idx = 0;
        while (idx < node->n && node->keys[idx].getid() < id)
            ++idx;

        if (idx < node->n && node->keys[idx].getid() == id) {
            if (node->leaf)
                removeFromLeaf(node, idx);
            else
                removeFromNonLeaf(node, idx);
        } else {
            if (node->leaf) {
                std::cout << "A música com ID " << id << " não está presente na árvore.\n";
                return;
            }

            bool flag = ((idx == node->n) ? true : false);

            if (node->children[idx]->n < ORDER / 2)
                fill(node, idx);

            if (flag && idx > node->n)
                remove(node->children[idx - 1], id);
            else
                remove(node->children[idx], id);
        }
    }

public:
    BTree() { root = new BTreeNode(true); }

    // Insere na árvore (Continua recebendo a música inteira)
    void insert(music k) {
        if (root->n == ORDER - 1) {
            BTreeNode* s = new BTreeNode(false);
            s->children[0] = root;
            
            // Garante que o ponteiro extra gerado esteja limpo
            s->children[1] = nullptr; 
            
            root = s;
            splitChild(s, 0);
            insertNonFull(s, k);
        } else {
            insertNonFull(root, k);
        }
    }

    // Busca na árvore (Recebe apenas o ID)
    BTreeNode* search(int id) {
        return (root == nullptr) ? nullptr : search(root, id);
    }

    // Remove da árvore (Recebe apenas o ID)
    void remove(int id) {
        if (!root) {
            std::cout << "A árvore está vazia.\n";
            return;
        }

        remove(root, id);

        if (root->n == 0) {
            BTreeNode* tmp = root;
            if (root->leaf)
                root = nullptr;
            else
                root = root->children[0];

            delete tmp;
        }
    }

    BTree(const BTree&) = delete;
    BTree& operator=(const BTree&) = delete;

    ~BTree() {
        clear(root);
    }
};

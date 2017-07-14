/*  
 *  Authors : Dimitrios Christaras-Papageorgiou et Tong Xue
 *  Date : 08/05/2017
 */

#ifndef LABYRINTHE_H
#define LABYRINTHE_H

#include "Environnement.h"

#include <list>
#include <iostream>
#include <math.h>
#include <vector>

class Labyrinthe : public Environnement {
 private:
    std::vector<std::vector<char> > _data;
    std::vector<std::vector<char> > _tresor_distances;
    int dmax; // distance maximale au tresor
    int seuil; // seuil pour le calcul des potentiels de defence
    int* get_info(char*);
    void init_lab(char*, int[], Box[], Wall[]);
    void calcul_distances_tresor();
    void calcul_dmax();
    void calcul_seuil();
    
 public:
    Labyrinthe (char*);
    int width () { return _data.size(); }	// retourne la largeur du labyrinthe.
    int height () { return _data[0].size(); }	// retourne la longueur du labyrinthe.
    char data (int i, int j) {
        return _data [i][j];
    }	// retourne la case (i, j).
    void remove_guard_data(int i);
    void add_guard_data(int i);
    void remove_data(int x, int y);
    void add_data(int x, int y);
    void kill_guard(int i);
    bool isWall(int x, int y) {
        for(int i = 0; i < _nwall; i++) {
            if((_walls[i]._x1 == _walls[i]._x2 && _walls[i]._x1 == x && ((_walls[i]._y1 >= y && _walls[i]._y2 <= y) || (_walls[i]._y1 <= y && _walls[i]._y2 >= y))) || (_walls[i]._y1 == _walls[i]._y2 && _walls[i]._y1 == y && ((_walls[i]._x1 >= x && _walls[i]._x2 <= x) || (_walls[i]._x1 <= x && _walls[i]._x2 >= x)))) {
                return true;
            }
        }
        return false;
    };
    void affiche_distances_tresor() {
        for( int i = 0; i < width(); i++ ) {
            for( int j = 0; j < height(); j++ ) {
                if(_tresor_distances[i][j] != -1) {
                    std::cout << _tresor_distances[i][j] << " ";
                } else {
                    std::cout << "-   ";
                }
            }
            std::cout << std::endl;
        }
    };
    int get_tresor_distance(int i, int j) {
        return _tresor_distances[i][j];
    };
    /*int get_chasseur_distance(int i, int j) {
        return sqrt(pow(i - (_guards[0] -> _x),2)-pow(j - (_guards[0] -> _y),2));
        }*/
    int get_dmax() { return dmax; }
    int get_seuil() { return seuil; }

    bool _fini;
};

#endif

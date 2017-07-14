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

// On donne la largeur et la hauteur du labyrinthe
#define	LAB_WIDTH	92
#define	LAB_HEIGHT	62

class Labyrinthe : public Environnement {
 private:
    char _data [LAB_WIDTH][LAB_HEIGHT];
    int _tresor_distances [LAB_WIDTH][LAB_HEIGHT]; // les distances au tresors
    
    // Ci-dessous d'autres variables pour representer data et tresor distances pour que ca marche sur n'importe quel fichier (ne marche pas)
    //int** _data;
    //int** _tresor_distances; // les distances au tresors
    int dmax; // distance maximale au tresor
    int seuil; // seuil pour le calcul des potentiels de defence
    int* get_info(char*);
    void init_lab(char*, int[], Box[], Wall[]);
    void calcul_distances_tresor();
    void calcul_dmax();
    void calcul_seuil();
 public:
    Labyrinthe (char*);
    int width () { return LAB_WIDTH;}	// retourne la largeur du labyrinthe.
    int height () { return LAB_HEIGHT;}	// retourne la longueur du labyrinthe.
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
        for( int i = 0; i < LAB_WIDTH; i++ ) {
            for( int j = 0; j < LAB_HEIGHT; j++ ) {
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

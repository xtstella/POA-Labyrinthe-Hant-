/*  
 *  Authors : Dimitrios Christaras-Papageorgiou et Tong Xue
 *  Date : 08/05/2017
 */

#include "Labyrinthe.h"
#include "Chasseur.h"
#include "Gardien.h"

#include <fstream>
#include <string>
#include <algorithm>
#include <list>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <queue>
#include <vector>

#include <iostream>

using namespace std;

Sound*	Chasseur::_hunter_fire;	// bruit de l'arme du chasseur.
Sound*	Chasseur::_hunter_hit;	// cri du chasseur touche.
Sound*	Chasseur::_wall_hit;	// on a tape un mur.
Sound* Gardien::_guard_fire;    // bruit de l'arme des gardiens.
Sound* Gardien::_guard_hit;     // cri du gardien touche.
Sound* Gardien::_guard_die;     // cri du gardien mort.
Sound* Gardien::_wall_hit;      // la boule de feu du gardien a tape un mur.

Environnement* Environnement::init (char* filename) {
    return new Labyrinthe (filename);
}

/* Les quatres methodes suivantes sont utilises pour mettre a jour _data (qui represente si les cases sont vides ou non) quand les gardiens bougent */

/* La place du gardien i dans le labyrinthe devient 0 */
void Labyrinthe::remove_guard_data(int i) {
    _data[(int)(_guards[i] -> _x/scale)][(int) (_guards[i] -> _y/scale)] = 0;
}

/* La place du gardien i dans le labyrinthe devient 1 */
void Labyrinthe::add_guard_data(int i) {
    _data[(int)(_guards[i] -> _x/scale)][(int) (_guards[i] -> _y/scale)] = 1;
}

/* La case du labyrinthe des coordonnees x et y devient 0 */
void Labyrinthe::remove_data(int x, int y) {
    _data[(int)(x/scale)][(int)(y/scale)] = 0;
}

/* La case du labyrinthe des coordonnees x et y devient 1 */
void Labyrinthe::add_data(int x, int y) {
    _data[(int)(x/scale)][(int)(y/scale)] = 1;
}

/* fonction qui renvoi les nombre maximal des murs (par rapport au caractere '+'), le nombre des gardiens et le nombre des boxes */
int* Labyrinthe::get_info(char* filename) {

    int maxwalls = 0;
    int nguards = 0;
    int nboxes = 0;
    int npicts = 0;

    int width = 0;
    int height = 0;

    bool firstline = false;
    
    string line;
    ifstream myfile (filename);
    if (myfile.is_open()) {
        while ( getline (myfile,line) ) {
            if(!firstline && line[0] == '+') {
                firstline = true;
            }
            if(std::count(line.begin(), line.end(), '#') == 0) {
                size_t o = std::count(line.begin(), line.end(), '+');
                maxwalls += 4 * o;
                o = std::count(line.begin(), line.end(), 'G');
                nguards += o;
                o = std::count(line.begin(), line.end(), 'x');
                nboxes += o;
                o = std::count(line.begin(), line.end(), 'a');
                npicts += o;
                o = std::count(line.begin(), line.end(), 'b');
                npicts += o;
            }
            if(firstline) {
                if(line.size() > width) { width = line.size(); }
                height += 1;
            }
        }
        std::cout << width << " " << height << std::endl;
        myfile.close();
    }
    static int res[6];
    res[0]= maxwalls;
    res[1] = nguards;
    res[2] = nboxes;
    res[3] = npicts;
    res[4] = width;
    res[5] = height;
    return res;
}

/* Initialisation du labyrinthe */
void Labyrinthe::init_lab(char* filename, int* chasseur, Box* boxes, Wall* picts) {
    char* modeles[] = { "Blade", "drfreak", "Droid", "garde", "Lezart", "Marvin", "Potator", "Samourai", "Serpent", "Squelette" };
    string line;
    ifstream myfile (filename);
    
    if (myfile.is_open()) {
        Wall hwall;
        hwall._ntex = 0;
        int co = 0;
        int w = 0;
        int firstline = false;
        int vwalls[width()];
        int nbox = 0;
        int nguard = 1;
        int npict = 0;
        fill_n(vwalls, sizeof(vwalls)/sizeof(*vwalls), -1);
        int l = 0;
        _nwall = 0;
        while ( getline (myfile,line) ) {
            if(firstline == false && line[0] == '+') {
                firstline = true;
                l = 0;
            }
            for(size_t c = 0; c < line.size(); c++) {

                // murs horizontaux
                if(line[c] == '+' && co == 0 && (line[c+1] == '-' || line[c+1] == '+')) {
                    hwall._x1 = c;
                    co++;
                    hwall._y1 = l;
                    co++;
                } else if(line[c] == '+' && co == 2 && (line[c+1] == '-' || line[c+1] == '+')) {
                    hwall._x2 = c;
                    co++;
                    hwall._y2 = l;
                    co++;
                    _walls[w] = hwall;
                    _nwall++;
                    w++;
                    co = 0;
                    hwall._x1 = c;
                    co++;
                    hwall._y1 = l;
                    co++;
                } else if(line[c] == '+' && co == 2) {
                    hwall._x2 = c;
                    co++;
                    hwall._y2 = l;
                    co++;
                    _walls[w] = hwall;
                    _nwall++;
                    w++;
                    co = 0;
                }

                // murs verticaux
                if(vwalls[c] != -1 && line[c] == '+') {
                    Wall vwall;
                    vwall._ntex = 0;
                    vwall._x1 = c;
                    vwall._y1 = vwalls[c];
                    vwall._x2 = c;
                    vwall._y2 = l;
                    _walls[w] = vwall;
                    _nwall++;
                    w++;
                    vwalls[c] = l;
                } else if(vwalls[c] == -1 && line[c] == '+') {
                    vwalls[c] = l;
                } else if(line[c] != '+' && line[c] != '|' && line[c] != 'a' && line[c] != 'b' && vwalls[c] != -1) {
                    vwalls[c] = -1;
                }
                
                // on ne marche pas sur les obstacles
                if(line[c] != ' ') {
                    _data[c][l] = 1;
                } else {
                    _data[c][l] = 0;
                }

                // coordonnees du chasseur
                if(line[c] == 'C') {
                    _guards [0] = new Chasseur (this); _guards [0] -> _x = c*scale; _guards [0] -> _y = l*scale;
                } else if(line[c] == 'G') {
                    int m = rand()%(sizeof(modeles)/sizeof(*modeles));
                    _guards [nguard] = new Gardien (this, modeles[m], nguard); _guards [nguard] -> _x = c*scale; _guards[nguard] -> _y = l*scale;
                    nguard++;
                } else if(line[c] == 'T') {
                    _treasor._x = c;
                    _treasor._y = l;
                } else if(firstline != false && line[c] == 'x') {
                    Box box = { c, l, 0 };
                    boxes[nbox] = box;
                    nbox++;
                } else if(firstline != false && (line[c] == 'a' || line[c] == 'b') && line[c-1] != '-') {
                    Wall p;
                    p._ntex = -1;
                    p._x1 = c;
                    p._y1 = l-1;
                    p._x2 = c;
                    p._y2 = l;
                    picts[npict] = p;
                    npict += 1;
                } else if(firstline != false && (line[c] == 'a' || line[c] == 'b') && line[c-1] != '|') {
                    Wall p;
                    p._ntex = -1;
                    p._x1 = c;
                    p._y1 = l;
                    p._x2 = c+1;
                    p._y2 = l;
                    picts[npict] = p;
                    npict += 1;
                }
            }
            l++;
        }
    }
    myfile.close();
}

/* Calcule la distance minimale de chaque case du labyrinthe au tresor (Algorithme de Dijkstra) */
void Labyrinthe::calcul_distances_tresor() {
    struct Pos {
        int x;
        int y;
        int dist;
    };
    queue<Pos> q;
    for(int i = 0; i < width(); i++) {
        for(int j = 0; j < height(); j++) {
            _tresor_distances[i][j] = -1;
        }
    }
    _tresor_distances[_treasor._x][_treasor._y] = 0;
    Pos init;
    init.x = _treasor._x;
    init.y = _treasor._y;
    init.dist = 1;
    init.x = init.x - 1;
    q.push(init); // case x-1 y
    init.y = init.y - 1;
    q.push(init); // case x-1 y-1
    init.y = init.y + 2;
    q.push(init); // case x-1 y+1
    init.x = init.x + 1;
    q.push(init); // case x y+1
    init.y = init.y - 2;
    q.push(init); // case x y-1
    init.x = init.x + 1;
    q.push(init); // case x+1 y-1
    init.y = init.y + 1;
    q.push(init); // case x+1 y
    init.y = init.y + 1;
    q.push(init); // case x+1 y+1
    while(!q.empty()) {
        Pos p = q.front();
        q.pop();
        if(p.x > 0 && p.x < width() && p.y > 0 && p.y < height() && !isWall(p.x,p.y) && _tresor_distances[p.x][p.y] == -1 || _tresor_distances[p.x][p.y] > p.dist) {
            _tresor_distances[p.x][p.y] = p.dist;
            p.dist = p.dist+1;
            p.x = p.x - 1;
            q.push(p); // case x-1 y
            p.y = p.y - 1;
            q.push(p); // case x-1 y-1
            p.y = p.y + 2;
            q.push(p); // case x-1 y+1
            p.x = p.x + 1;
            q.push(p); // case x y+1
            p.y = p.y - 2;
            q.push(p); // case x y-1
            p.x = p.x + 1;
            q.push(p); // case x+1 y-1
            p.y = p.y + 1;
            q.push(p); // case x+1 y
            p.y = p.y + 1;
            q.push(p); // case x+1 y+1
        } else if (isWall(p.x,p.y)) {
            _tresor_distances[p.x][p.y] = -1;
        }
    }
}

/* Trouve la distance maximale au tresor */
void Labyrinthe::calcul_dmax() {
    dmax = 0;
    for(int k = 0; k < width(); k++) {
        for(int t = 0; t < height(); t++) {
            if(get_tresor_distance(k,t) > dmax) {
                dmax = get_tresor_distance(k,t);
            }
        }
    }
}

/* Calcule le seuil pour le calcule du potentiel de defence (Le seuil est la sommme des potentiels de defence de tous les gardien en mode defence au debut du programme) */
void Labyrinthe::calcul_seuil() {
    float s = 0;
    for(int k = 1; k < _nguards; k++) {
        if(k < (_nguards)/2) {
            if(Gardien* g = dynamic_cast<Gardien*>(_guards[k])) {
                s += (float) dmax/(float) (get_tresor_distance(g -> _x/scale, g -> _y/scale));
            }
        }
    }
    seuil = (int) s;
}

/* Constructeur qui initialise le labyrinthe */
Labyrinthe::Labyrinthe (char* filename) {
    
    int * info;
    info = get_info(filename);

    _data.resize(info[4]);
    _tresor_distances.resize(info[4]);
    for(int i = 0; i < info[4]; i++) {
        _data[i].resize(info[5]);
        _tresor_distances[i].resize(info[5]);
    }
    
    int maxwalls = info[0];
    _nguards = info[1]+1;
    _nboxes = info[2];
    _npicts = info[3];
    _walls = new Wall[maxwalls];
    Box * boxes = new Box[info[2]];
    Wall * picts = new Wall[_npicts];
    int * chasseur = new int[2];
    _guards = new Mover* [_nguards];
    init_lab(filename, chasseur, boxes, picts);
    _boxes = boxes;
    _picts = picts;
    _fini = false;
    calcul_distances_tresor();
    calcul_dmax();
    calcul_seuil();
    //affiche_distances_tresor();
}

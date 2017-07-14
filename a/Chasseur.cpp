/*  
 *  Authors : Dimitrios Christaras-Papageorgiou et Tong Xue
 *  Date : 08/05/2017
 */

#include "Chasseur.h"
#include "Gardien.h"

#include <string>

#include <iostream>

/*
 *	Tente un deplacement.
 */
bool Chasseur::move_aux (double dx, double dy) {
    
    // quand le chasseur est mort on ne peut pas deplacer
    if(is_dead()) {
        return false;
    }
    if (0 == _l -> data ((int)((_x + dx) / Environnement::scale),
                         (int)((_y + dy) / Environnement::scale))) {
        _x += dx;
        _y += dy;
        return true;
    }
    return false;
}

/*
 *	Constructeur.
 */
Chasseur::Chasseur (Labyrinthe* l) : Mover (100, 80, l, 0) {
    _dead = false;
    _health = 200; // le chasseur a deux fois plus des points de vie que les gardiens
    _hunter_fire = new Sound ("sons/hunter_fire.wav");
    _hunter_hit = new Sound ("sons/hunter_hit.wav");
    if (_wall_hit == 0)
        _wall_hit = new Sound ("sons/hit_wall.wav");
    message (std::to_string(_health).c_str());
}

/*
 *	Fait bouger la boule de feu (ceci est une exemple, à vous de traiter les collisions spécifiques...)
 */
bool Chasseur::process_fireball (float dx, float dy) {

    bool gardien_hit = false;
    
    // calculer la distance entre le chasseur et le lieu de l'explosion.
    float	x = (_x - _fb -> get_x ()) / Environnement::scale;
    float	y = (_y - _fb -> get_y ()) / Environnement::scale;
    float	dist2 = x*x + y*y;
    // on bouge que dans le vide!
    if (0 == _l -> data ((int)((_fb -> get_x () + dx) / Environnement::scale),
                         (int)((_fb -> get_y () + dy) / Environnement::scale))) {
        float	dmax2 = (_l -> width ())*(_l -> width ()) + (_l -> height ())*(_l -> height ());        
        //message ("Woooshh ..... %d", (int) dist2);
        // il y a la place.
        return true;
    } else if (0 != _l -> data ((int)((_fb -> get_x () + dx) / Environnement::scale),
                                (int)((_fb -> get_y () + dy) / Environnement::scale))) {
        float	dmax2 = (_l -> width ())*(_l -> width ()) + (_l -> height ())*(_l -> height ());
        for(int i = 0; i < (_l -> _nguards); i++) { // on parcourt tous les gardiens pour voir si on a tape quelqu'un
            if( (_fb -> get_x ())/Environnement::scale > (_l -> _guards[i] -> _x)/Environnement::scale - 2 &&
                (_fb -> get_x ())/Environnement::scale < (_l -> _guards[i] -> _x)/Environnement::scale + 2 &&
                (_fb -> get_y ())/Environnement::scale > (_l -> _guards[i] -> _y)/Environnement::scale - 2 &&
                (_fb -> get_y ())/Environnement::scale < (_l -> _guards[i] -> _y)/Environnement::scale + 2) {

                // Decommenter ci-dessous pour que le chasseur se blesse si la boule de feu explose pres de lui
                /*if(i == 0) {
                  if(get_health() != 50) {
                  injure(dist2, dmax2);
                  } else if(get_health() == 50) {
                  kill(dist2, dmax2);
                  }
                  } else {*/
                if(Gardien* g = dynamic_cast<Gardien*>(_l -> _guards[i])) {
                    g -> injure(dist2, dmax2); // quand on tape un gardien on diminue ses points de vie
                    gardien_hit = true;
                }
                //}
                
            }
        }

        // teleporte le chasseur vers un autre pict, si on tire sur un pict
        for(int i = 0; i < _l -> _npicts; i++) {
            int px1 = _l -> _picts[i]._x1;
            int px2 = _l -> _picts[i]._x2;
            int py1 = _l -> _picts[i]._y1;
            int py2 = _l -> _picts[i]._y2;
            if(((_fb -> get_x ())/Environnement::scale > px1-2 && (_fb -> get_x ())/Environnement::scale < px2+2 && (_fb -> get_y ())/Environnement::scale > py1-2 && (_fb -> get_y ())/Environnement::scale < py2+2) && (_l -> _npicts) > 1) {
                _x = _l -> _picts[i+1]._x1*Environnement::scale;
                _y = _l -> _picts[i+1]._y1*Environnement::scale;
                
                //std::cout << i << std::endl;
                
                if(i+1 == _l -> _npicts || i == 8) {
                    _x = _l -> _picts[1]._x1*Environnement::scale;
                    _y = _l -> _picts[1]._y1*Environnement::scale;
                }
                break;
            }
        }
    }
    // calculer la distance maximum en ligne droite.
    float	dmax2 = (_l -> width ())*(_l -> width ()) + (_l -> height ())*(_l -> height ());
    // faire exploser la boule de feu avec un bruit fonction de la distance.
    if(!gardien_hit) {
        _wall_hit -> play (1. - dist2/dmax2);
        //message ("Booom...");
    }
    return false;
}

/*
 *	Tire sur un ennemi.
 */
void Chasseur::fire (int angle_vertical) {
    //message ("Woooshh...");
    _hunter_fire -> play ();
    _fb -> init (/* position initiale de la boule */ _x, _y, 10.,
                 /* angles de visée */ angle_vertical, _angle);
}

/*
 *  Methode qui diminue les poitnes de vie du chasseur quand il est tape par une boule de feu d'un gardien
 */
void Chasseur::injure(float dist2, float dmax2) {
    _hunter_hit -> play(1. - dist2/dmax2);
    _health -= 40;
    if(_health == 0) {
        kill(dist2, dmax2);
    }
    message (std::to_string(_health).c_str());
}

/* 
 *  Quand le chasseur est mort on finit la partie
 */
void Chasseur::kill(float dist2, float dmax2) {
    partie_terminee(false);
    _dead = true;
}

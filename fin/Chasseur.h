/*  
 *  Authors : Dimitrios Christaras-Papageorgiou et Tong Xue
 *  Date : 08/05/2017
 */

#ifndef CHASSEUR_H
#define CHASSEUR_H

#include <stdio.h>
#include "Mover.h"
#include "Sound.h"

class Labyrinthe;

class Chasseur : public Mover {
 private:
    // accepte ou non un deplacement.
    int _health;
    bool _dead;
    bool move_aux (double dx, double dy);
 public:
    /*
     *	Le son...
     */
    static Sound*	_hunter_fire;	// bruit de l'arme du chasseur.
    static Sound*	_hunter_hit;	// cri du chasseur touché.
    static Sound*	_wall_hit;		// on a tapé un mur.
    
    Chasseur (Labyrinthe* l);
    // ne bouger que dans une case vide (on 'glisse' le long des obstacles)
    bool move (double dx, double dy) {
        return move_aux (dx, dy) || move_aux (dx, 0.0) || move_aux (0.0, dy);
    }
    // le chasseur ne pense pas!
    void update (void) {};
    bool process_fireball (float dx, float dy);
    // tire sur un ennemi.
    void fire (int angle_vertical);
    bool is_dead() { return _dead; };
    void injure(float dist2, float dmax2);
    void kill(float dist2, float dmax2);
    int get_health() { return _health; }
};

#endif

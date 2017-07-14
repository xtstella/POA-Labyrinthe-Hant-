/*  
 *  Authors : Dimitrios Christaras-Papageorgiou et Tong Xue
 *  Date : 08/05/2017
 */

#ifndef GARDIEN_H
#define GARDIEN_H

#include "Mover.h"
#include "Sound.h"
#include "Labyrinthe.h"

#include <iostream>
#include <cstdlib>
#include <math.h>
#include <random>
#include <vector>

class Labyrinthe;

class Gardien : public Mover {
 private:
    int _i;
    bool previous_empty;
    bool previous_positive_angle_change;
    float _protection;
    float _speed; // vitesse du gardine (pas utilise)
    char* _mode;
    bool _vivant;
    int _health;
    bool _fired;
    int _vision;
    int milli_after_fire;
    double previous_x, previous_y;
    double previous_dx, previous_dy;
    void change_angle(double dx, double dy);
    bool is_point_of_horizontal_wall(double x, double y);
    double * avoid_obstacle(double dx, double dy);
    bool chasseur_is_visible();
    float get_chasseur_distance(int x, int y);
    void patrouille_mouvement();
    void defence_mouvement();
    void attack_mouvement();

 public:
    Labyrinthe * lab;
    
    static Sound* _guard_fire;
    static Sound* _guard_hit;
    static Sound* _guard_die;
    static Sound* _wall_hit;

    Gardien (Labyrinthe* l, const char* modele, int i);
    void update (void);
    bool move (double dx, double dy);
    void fire (int angle_vertical);
    bool process_fireball (float dx, float dy);
    bool is_dead() { return _health == 0; }
    void injure(float dist2, float dmax2);
    void kill(float dist2, float dmax2);
    int get_health() { return _health; }
    int get_i() { return _i; }
    double get_previous_x() { return previous_x; }
    double get_previous_y() { return previous_y; }
    char* get_mode() { return _mode; }
    void set_mode(char* mode) { _mode = mode; }
    float potentiel_defence();
    int somme_potentiel_defence();
    void ajoute_defenseur();
    void ajoute_patrouilleur();
};

#endif

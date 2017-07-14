/*  
 *  Authors : Dimitrios Christaras-Papageorgiou et Tong Xue
 *  Date : 08/05/2017
 */

#include "Gardien.h"
#include "Chasseur.h"

#include <iostream>
#include <ctime>
#include <stdlib.h>
#include <stdio.h>
#include <math.h>

/*
 *  Change l'angle de vision du gardien par rapport a son deplacement
 */
void Gardien::change_angle (double dx, double dy) {
    float new_x = _x + dx;
    float new_y = _y + dy;
    _angle = (int) (atan ( (new_y - _y) / (new_x - _x) ) * 180/3.14) + 90;
    if((new_x > _x && new_y > _y) || (!(new_x < _x) && new_y > _y) || new_x > _x && new_y < _y) { _angle += 180; }
};

/*
 *  Les gardien ont 85% des chances de continuer dans le meme deplacement qu'avant est 15% des chances de changer
 */
int select_direction() {
    std::random_device rd; // obtain a random number from hardware
    std::mt19937 eng(rd()); // seed the generator
    std::uniform_int_distribution<> distr(0, 100); // define the range
    if(distr(eng) < 85) {
        return 1;
    }
    return 0;
};

/*
 *  Trouve l'angle entre deux points
 */
double find_angle(double x1, double y1, double x2, double y2) {
    return atan2(y2-y1,x2-x1);
};

/*
 *  Rotation d'un gardien par rapport a un angle donne (utiliser pour eviter de se bloquer sur un mur)
 */
double * rotate_point(double dx, double dy, double angle, double x, double y) {
    angle = angle/180 * 3.14;
    double * res = new double[2];
    res[0] = dx * cos(angle) - dy * sin(angle);
    res[1] = dx * sin(angle) + dy * cos(angle);
    return res;
};

/*
 *  Vrai si ce point est un point d'un mur horizontal
 */
bool Gardien::is_point_of_horizontal_wall(double x, double y) {
    for(int i = 0; i < _l -> _nwall; i++) {
        if((_l -> _walls)[i]._y1 == (_l -> _walls)[i]._y2) {
            if((int) y == (_l -> _walls)[i]._y1 && x > (_l -> _walls)[i]._x1 && x < (_l -> _walls)[i]._x2) {
                return true;
            }
        }
    }
    return false;
};


/*
 *  Fonction qui deplace les gardiens d'une facon naturelle quand ils trouvent un mur
 */
double * Gardien::avoid_obstacle(double dx, double dy) {
    if(previous_empty) {
        double angle = find_angle(_x,_y,_x+dx,_y+dy)*180/3.14;
        bool is_horizontal = is_point_of_horizontal_wall((_x+15*dx)/Environnement::scale, (_y+15*dy)/Environnement::scale);
        if((is_horizontal && ((_angle > 90 && _angle < 180) || (_angle > 270 && _angle < 360))) || (!is_horizontal && ((_angle > 0 && _angle < 90) || (_angle > 180 && _angle < 270)))) {
            previous_positive_angle_change = false;
            return rotate_point(dx,dy,-20,_x,_y);
        } else {
            previous_positive_angle_change = true;
            return rotate_point(dx,dy,20,_x,_y);
        }
    } else {
        if(previous_positive_angle_change) {
            return rotate_point(dx,dy,20,_x,_y);
        } else {
            return rotate_point(dx,dy,-20,_x,_y);
        }
    }
};

/*
 *  Trace une ligne entre le chasseur et le gardien et parcourt tous les points de la ligne. Si tous sont vides alors le chasseur est visible
 *  Algorithme de tracé de segment de Bresenham
 */
bool Gardien::chasseur_is_visible() {
    float StartX = _x/Environnement::scale;
    float StartY = _y/Environnement::scale;
    float EndX = (_l -> _guards[0] -> _x)/Environnement::scale;
    float EndY = (_l -> _guards[0] -> _y)/Environnement::scale;

    // l'algo marche seulement avec StartX < EndX sinon on echange les valeurs
    if(EndX < StartX) {
        float TempX = StartX;
        float TempY = StartY;
        StartX = EndX;
        StartY = EndY;
        EndX = TempX;
        EndY = TempY;
    }

    //std::cout << _i << " s : " << StartX << " " << StartY << std::endl;
    
    // m = rise / run
    float m = 0.0;
    
    float run = EndX - StartX;
    float rise = EndY - StartY;
    m = rise / run;
    
    float b = StartY - (m * StartX);
    // note: supposons EndX > StartX
    for (float x = StartX+1; x < EndX; ++x) {
        // solution pour y
        float y = (m * x) + b;

        // Si la case associee a x et y du labyrinthe n'est pas vide, alors le chasseur n'est pas visible
        if(_l -> data((int) x, (int) y) != 0) {
            return false;
        }
        //std::cout << (int) x << "   " << (int) y << std::endl;
    }
    
    //std::cout << _i << " e : " << EndX << " " << EndY << std::endl;
    //std::cout << get_chasseur_distance(_x, _y) << std::endl;

    // Si la distance au chasseur est inferieure a la distance de vision, alors le chasseur est visible
    if(get_chasseur_distance(_x, _y)/Environnement::scale < _vision) {
        return true;
    }
    return false;
}

/*
 *  Calcul de la distance entre le chasseur et un gardien
 */
float Gardien::get_chasseur_distance(int x, int y) {
    return sqrt(pow((lab -> _guards[0] -> _x) - x, 2) + pow((lab -> _guards[0] -> _y) - y, 2));
}

/*
 *  Bouge le gardien aleatoirement
 */        
void Gardien::patrouille_mouvement() {
    previous_x = _x;
    previous_y = _y;
    lab -> remove_data(previous_x, previous_y); // la case de la position precedente du gardien devient vide

    // Si on voit un pur on appelle la fonction avoid_obstable pour trouver le mouvement approprie
    if (0 != _l -> data ((int)((_x + 15*previous_dx) / Environnement::scale), (int)((_y + 15*previous_dy) / Environnement::scale))) {
            
        double * newd = new double[2];
        newd = avoid_obstacle(previous_dx, previous_dy);
            
        change_angle(newd[0], newd[1]);
        
        move(newd[0], newd[1]);
        
        lab -> add_guard_data(_i);
        previous_dx = newd[0];
        previous_dy = newd[1];
            
        previous_empty = false;
            
    } else if(select_direction() == 0) { // on choisit le mouvement qui change le moins la direction
            
        double * selected = new double[2];
        int angle = 500;
        std::random_device rd; // obtain a random number from hardware
        std::mt19937 eng(rd()); // seed the generator
        std::uniform_real_distribution<> distr(-0.2, 0.2); // define the range
        double dx, dy;
        for(int i = 0; i < 50; i++) {
            dx = distr(eng);
            dy = distr(eng);
            float new_x = _x + dx;
            float new_y = _y + dy;
            int a = (int) (atan ( (new_y - _y) / (new_x - _x) ) * 180/3.14) + 90;
            if((new_x > _x && new_y > _y) || (!(new_x < _x) && new_y > _y) || (new_x > _x && new_y < _y)) { a += 180; }
            if(abs(_angle - a) < angle) {
                angle = abs(_angle - a);
                selected[0] = dx;
                selected[1] = dy;
            }
        }
        change_angle(selected[0], selected[1]);
        
        move(selected[0], selected[1]);
        
        lab -> add_guard_data(_i);
        previous_dx = selected[0];
        previous_dy = selected[1];

        previous_empty = true;
    } else { // meme mouvement avec avant
        
        move(previous_dx, previous_dy);
        
        lab -> add_guard_data(_i);
        previous_empty = true;
    }
};

/*
 *  Garde le meme type de mouvement que patrouille, mais cette fois on essaie de se rapprocher au tresor
 */
void Gardien::defence_mouvement() {
    previous_x = _x;
    previous_y = _y;
    lab -> remove_data(previous_x, previous_y);
    if (0 != _l -> data ((int)((_x + 15*previous_dx) / Environnement::scale), (int)((_y + 15*previous_dy) / Environnement::scale))) {
            
        double * newd = new double[2];
        newd = avoid_obstacle(previous_dx, previous_dy);
            
        change_angle(newd[0], newd[1]);
        
        move(newd[0], newd[1]);
        
        lab -> add_guard_data(_i);
        previous_dx = newd[0];
        previous_dy = newd[1];
            
        previous_empty = false;
            
    } else if(select_direction() == 0) {

        int _tresor_distance = 9999;
        double * selected = new double[2];
        int angle = 500;
        std::random_device rd; // obtain a random number from hardware
        std::mt19937 eng(rd()); // seed the generator
        std::uniform_real_distribution<> distr(-0.2, 0.2); // define the range
        double dx, dy;
        for(int i = 0; i < 50; i++) {
            dx = distr(eng);
            dy = distr(eng);
            float new_x = _x + dx;
            float new_y = _y + dy;
            int a = (int) (atan ( (new_y - _y) / (new_x - _x) ) * 180/3.14) + 90;
            if((new_x > _x && new_y > _y) || (!(new_x < _x) && new_y > _y) || (new_x > _x && new_y < _y)) { a += 180; }
            
            // Ci-dessus la seule difference avec le code de la methode patrouille_mouvement
            if(abs(_angle - a) < angle || lab -> get_tresor_distance(new_x/Environnement::scale, new_y/Environnement::scale) < _tresor_distance) {
                _tresor_distance = lab -> get_tresor_distance(new_x/Environnement::scale, new_y/Environnement::scale);
                angle = abs(_angle - a);
                selected[0] = dx;
                selected[1] = dy;
            }
        }
        change_angle(selected[0], selected[1]);
        
        move(selected[0], selected[1]);
        
        lab -> add_guard_data(_i);
        previous_dx = selected[0];
        previous_dy = selected[1];

        previous_empty = true;
    } else {
        
        move(previous_dx, previous_dy);
        
        lab -> add_guard_data(_i);
        previous_empty = true;
    }
};

/*
 *  Garde le meme type de mouvement que patrouille, mais cette fois on essaie de se rapprocher au chasseur
 */
void Gardien::attack_mouvement() {
    previous_x = _x;
    previous_y = _y;
    lab -> remove_data(previous_x, previous_y);
    if (0 != _l -> data ((int)((_x + 15*previous_dx) / Environnement::scale), (int)((_y + 15*previous_dy) / Environnement::scale))) {
            
        double * newd = new double[2];
        newd = avoid_obstacle(previous_dx, previous_dy);
            
        change_angle(newd[0], newd[1]);
        
        move(newd[0], newd[1]);
        
        lab -> add_guard_data(_i);
        previous_dx = newd[0];
        previous_dy = newd[1];
            
        previous_empty = false;
            
    } else if(select_direction() == 0) {

        double chasseur_distance = 9999;
        double x_dist = 9999;
        double y_dist = 9999;
        double * selected = new double[2];
        int angle = 500;
        std::random_device rd; // obtain a random number from hardware
        std::mt19937 eng(rd()); // seed the generator
        std::uniform_real_distribution<> distr(-0.5, 0.5); // define the range
        double dx, dy;
        for(int i = 0; i < 1000; i++) {
            dx = distr(eng);
            dy = distr(eng);
            float new_x = _x + dx;
            float new_y = _y + dy;
            int a = (int) (atan ( (new_y - _y) / (new_x - _x) ) * 180/3.14) + 90;
            if((new_x > _x && new_y > _y) || (!(new_x < _x) && new_y > _y) || (new_x > _x && new_y < _y)) { a += 180; }
            
            // Ci-dessus la seule difference avec le code de la methode patrouille_mouvement
            if(abs(_angle - a) < angle || (get_chasseur_distance(new_x, new_y) < chasseur_distance && (abs(new_x - lab -> _guards[0] -> _x) < x_dist && abs(new_y - lab -> _guards[0] -> _y) < y_dist))) {
                chasseur_distance = get_chasseur_distance(new_x, new_y);
                x_dist = abs(new_x - lab -> _guards[0] -> _x);
                y_dist = abs(new_y - lab -> _guards[0] -> _y);
                
                angle = abs(_angle - a);
                selected[0] = dx;
                selected[1] = dy;
            }
        }
        change_angle(selected[0], selected[1]);
        
        move(selected[0], selected[1]);
        
        lab -> add_guard_data(_i);
        
        previous_dx = selected[0];
        previous_dy = selected[1];

        previous_empty = true;
    } else {
        
        move(previous_dx, previous_dy);
        
        lab -> add_guard_data(_i); // la case representant la nouvelle position du gardien n'est pas vide
        
        previous_empty = true;
    }
};

/*
 *  Constructeur.
 */
Gardien::Gardien (Labyrinthe* l, const char* modele, int i) : Mover (120, 80, l, modele) {
    _i = i;
    previous_empty = true;
    previous_positive_angle_change = false;
    if(i < (l -> _nguards)/2) {
        _mode = "defence";
    } else {
        _mode = "patrouille";
    }
    previous_x = _x;
    previous_y = _y;
    previous_dx = 0;
    previous_dy = 0;
    _guard_fire = new Sound ("sons/guard_fire.wav");
    _guard_hit = new Sound ("sons/guard_hit.wav");
    _guard_die = new Sound ("sons/guard_die.wav");
    _wall_hit = new Sound ("sons/hit_wall.wav");
    _vivant = true;
    _health = 100;
    _fired = false;
    _vision = _l -> width ();
    milli_after_fire = 500; // Si on voit un chasseur pour la premiere fois, alors on tire vers lui apres 0,5 secondes
    lab = l;
}

/*
 *  Calcul du potentiel de defence du gardien
 */
float Gardien::potentiel_defence() {
    float dmax = 0;
    float dist = 0;
    if(Labyrinthe* l = dynamic_cast<Labyrinthe*>(_l)) {
        dmax = l -> get_dmax(); // distance maximale au tresor
        dist = l -> get_tresor_distance((int) (_x / Environnement::scale), (int) (_y / Environnement::scale));
    }
    
    //std::cout << dist << std::endl;
    //std::cout << dmax/dist << std::endl;
    
    return dmax/dist;
}

/*
 *  Somme des potentiels de defence de tous les gardiens
 */
int Gardien::somme_potentiel_defence() {
    float somme = 0;
    for(int i = 1; i < _l -> _nguards; i++) {
        if(Gardien* g = dynamic_cast<Gardien*>(_l -> _guards[i])) {
            
            //std::cout << g -> get_mode() << std::endl;
            
            if(g -> get_mode() == "defence" && _vivant) {
                somme += g -> potentiel_defence();
            }
        }
    }
    
    //std::cout << std::endl << std::endl;
    //std::cout << somme << std::endl;
    
    return (int) somme;
}

/*
 *  Si la somme des potentiels de defence est inferieur au seuil, alors on utilise cette methode pour changer la mode d'un gardien de patrouille en defence
 */
void Gardien::ajoute_defenseur() {
    bool ajoute = false;
    int i = 1;
    while(!ajoute && i <= _l -> _nguards) {
        if(Gardien* g = dynamic_cast<Gardien*>(_l -> _guards[i])) {
            if(g -> get_mode() == "patrouille") {
                g -> set_mode("defence");
                ajoute = true;
            }
        }
        i += 1;
    }
}

/*
 *  Si la somme des potentiels de defence est superieur au seuil, alors on utilise cette methode pour changer la mode d'un gardien de defence en patrouille
 */
void Gardien::ajoute_patrouilleur() {
    bool ajoute = false;
    int i = 1;
    while(!ajoute && i <= _l -> _nguards) {
        if(Gardien* g = dynamic_cast<Gardien*>(_l -> _guards[i])) {
            if(g -> get_mode() == "defence") {
                g -> set_mode("patrouille");
                ajoute = true;
            }
        }
        i += 1;
    }
}

void Gardien::update (void) {
    srand(time(NULL));
    // Si le chasseur est arrive au tresor, alors la partie est terminee
    if(lab -> get_tresor_distance(_l -> _guards[0] -> _x / Environnement::scale, _l -> _guards[0] -> _y / Environnement::scale) == 1) {
        partie_terminee(true);
        lab -> _fini = true;
    }
    // Si le gardien est mort alors data est vide (donc on peut passer dessus)
    if(_vivant == false) {
        lab -> remove_guard_data(_i);
    }
    if(Chasseur* c = dynamic_cast<Chasseur*>(_l -> _guards[0])) {
        if(!lab -> _fini && _vivant == true && !(c -> is_dead())) {

            // Calcul la somme des potentiels de defence pour voir si on ajoute un defenseur ou un patrouilleur
            int pot_def = 0;
            if(_i == 1) {
                pot_def = somme_potentiel_defence();
                if(Labyrinthe* l = dynamic_cast<Labyrinthe*>(_l)) {
                    
                    //std::cout << pot_def << " " << l -> get_seuil() << std::endl;
                    
                    if(pot_def < l -> get_seuil()) {
                        ajoute_defenseur();
                    } else if(pot_def > l -> get_seuil()) {
                        ajoute_patrouilleur();
                    }
                }
            }

            // Changer le mode du gardien si on voit le chasseur
            if(chasseur_is_visible() && _mode != "attaque") {
                _mode = "attaque";
            } else if(_mode == "attaque") {
                _mode = "patrouille";
            }

            // bouge le gardien par rapport a son mode
            if(_mode == "patrouille") {
                patrouille_mouvement();
            } else if(_mode == "defence") {
                defence_mouvement();
            } else if(_mode == "attaque") {
                attack_mouvement();
                // On tire chaque 1 seconde
                if(!_fired && milli_after_fire > 1000) {
                    fire(0);
                    _fired = true;
                    milli_after_fire = 0;
                } else {
                    milli_after_fire += 10;
                }
            }
        }
    }
};

/*
 *	Tente un deplacement.
 */
bool Gardien::move (double dx, double dy) {
    if (0 == _l -> data ((int)((_x + dx) / Environnement::scale),
                         (int)((_y + dy) / Environnement::scale))) {
        _x += dx;
        _y += dy;
        return true;
    }
    return false;
}

/*
 *  Tire sur le chasseur en lancant une boule de feu qui se dirige vers le chasseur
 */

void Gardien::fire (int angle_vertical) {
    //message ("Woooshh...");
    _guard_fire -> play ();
    int fire_angle = (int) (atan ( ((_l -> _guards[0] -> _y) - _y) / ((_l -> _guards[0] -> _x) - _x) ) * 180/3.14);
    int a = fire_angle;
    fire_angle += ceil(-fire_angle/360) * 360;
    if(_x < (_l -> _guards[0] -> _x)) {
        fire_angle += 90;
    } else if(_x > (_l -> _guards[0] -> _x)){
        fire_angle -= 90;
    }
    fire_angle -= 2 * a;

    int t = 0;
    if(_health == 50) { // si le gardien est blesse il peut rate sa cible avec une probabilite de 50%
        std::random_device rd; // obtain a random number from hardware
        std::mt19937 eng(rd()); // seed the generator
        std::uniform_int_distribution<> distr(0, 100); // define the range
        if(distr(eng) < 50) {
            t = 1;
        }
    }
    if(t == 1) { // Le gardien rate sa cible
        fire_angle += 10;
    }
    //std::cout << fire_angle << " " << _i << std::endl;
    //std::cout << "angle " << fire_angle << std::endl;
    
    _fb -> init (_x, _y, 10., angle_vertical, fire_angle);
}

/*
 *  Fait bouger la boule de feu
 */

bool Gardien::process_fireball (float dx, float dy) {

    bool mover_hit = false;
    
    // calculer la distance entre le chasseur et le lieu de l'explosion.
    float	x = (_x - _fb -> get_x ()) / Environnement::scale;
    float	y = (_y - _fb -> get_y ()) / Environnement::scale;
    float	dist2 = x*x + y*y;
    // on bouge que dans le vide!
    if (0 == _l -> data ((int)((_fb -> get_x () + dx) / Environnement::scale),
                         (int)((_fb -> get_y () + dy) / Environnement::scale))) {
        //message ("Woooshh ..... %d", (int) dist2);

        float	dmax2 = (_l -> width ())*(_l -> width ()) + (_l -> height ())*(_l -> height ());

        // il y a la place.
        if( (_fb -> get_x ())/Environnement::scale > (_l -> _guards[0] -> _x)/Environnement::scale - 2 &&
            (_fb -> get_x ())/Environnement::scale < (_l -> _guards[0] -> _x)/Environnement::scale + 2 &&
            (_fb -> get_y ())/Environnement::scale > (_l -> _guards[0] -> _y)/Environnement::scale - 2 &&
            (_fb -> get_y ())/Environnement::scale < (_l -> _guards[0] -> _y)/Environnement::scale + 2) {
            _fired = false;
            if(Chasseur* c = dynamic_cast<Chasseur*>(_l -> _guards[0])) {
                mover_hit = true;
                c -> injure(dist2, dmax2);
            }
            return false;
        }
        return true;
    } else if (0 != _l -> data ((int)((_fb -> get_x () + dx) / Environnement::scale),
                                (int)((_fb -> get_y () + dy) / Environnement::scale)) && (int)((_fb -> get_x () + dx) / Environnement::scale) != _x/Environnement::scale && (int)((_fb -> get_y () + dy) / Environnement::scale) != _y/Environnement::scale) {
        _fired = false;
        float	dmax2 = (_l -> width ())*(_l -> width ()) + (_l -> height ())*(_l -> height ());
        for(int i = 0; i < (_l -> _nguards); i++) {
            if( (_fb -> get_x ())/Environnement::scale > (_l -> _guards[i] -> _x)/Environnement::scale - 1 &&
                (_fb -> get_x ())/Environnement::scale < (_l -> _guards[i] -> _x)/Environnement::scale + 1 &&
                (_fb -> get_y ())/Environnement::scale > (_l -> _guards[i] -> _y)/Environnement::scale - 1 &&
                (_fb -> get_y ())/Environnement::scale < (_l -> _guards[i] -> _y)/Environnement::scale + 1) {
                
                // Decommenter si les gardiens peuvent blesser d'autres gardiens
                /*if(Gardien* g = dynamic_cast<Gardien*>(_l -> _guards[i])) {
                  if(g -> get_i() != _i) {
                  g -> injure(dist2, dmax2);
                  }
                  }*/
                mover_hit = true;
            }
        }
    }
    // calculer la distance maximum en ligne droite.
    float	dmax2 = (_l -> width ())*(_l -> width ()) + (_l -> height ())*(_l -> height ());
    if(!mover_hit) {
        // faire exploser la boule de feu avec un bruit fonction de la distance.
        _wall_hit -> play (1. - dist2/dmax2);
        //message ("Booom...");
    }
    return false;
}

/*
 *  Blesse un gardien (diminue ses points de vie et sa distance de vision)
 */
void Gardien::injure(float dist2, float dmax2) {
    _health -= 50;
    _vision /= 2;
    if(_health == 0) {
        kill(dist2, dmax2);
    } else {
        _guard_hit -> play(1. - dist2/dmax2);
    }
}

/*
 *  Tue un gardien
 */
void Gardien::kill(float dist2, float dmax2) {
    _health -= 50;
    _guard_die -> play(1. - dist2/dmax2);
    tomber();
    rester_au_sol();
    _vivant = false;
    previous_x = 9999;
    previous_y = 9999;
}

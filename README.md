# Wolf3D

Remake du jeu culte **Wolfenstein 3D**, réalisé en C avec la bibliothèque graphique **CSFML**, dans le cadre du cursus Epitech.

Moteur de rendu par raycasting, génération procédurale de niveaux par **BSP (Binary Space Partitioning)**, et interface de menus complète.

---

## Contexte & histoire

**Quelque part en Europe, en pleine guerre.**

Le front s'effondre par endroits. Certains soldats tombent au combat, d'autres sont faits prisonniers. **Ralf**, jeune commandant capturé par les forces ennemies, est enfermé dans les sous-sols d'un bunker fortifié — profondeur inconnue, sortie encore plus incertaine.

Une nuit, alors que la garde se relâche et que la fatigue gagne ses geôliers, Ralf parvient à assommer le soldat chargé de le surveiller. Il lui prend son arme, et entame son ascension : étage après étage, il doit remonter des entrailles du bunker jusqu'à la surface, avant que l'alerte ne se généralise.

Un homme seul, sans renfort, remontant vers la lumière à travers des couloirs de plus en plus hostiles.

*(Chaque niveau du jeu correspond à un étage du bunker : la génération procédurale BSP produit un nouvel étage à chaque remontée.)*

---

## Vision du gameplay (MVP)

- Déplacement fluide à la première personne (raycasting, style Wolfenstein 3D / DOOM)
- Génération de niveaux procédurale via BSP : chaque niveau = un étage du bunker, la sortie d'un étage mène à la génération du suivant
- Difficulté croissante étage par étage (plus d'ennemis, patrouilles plus denses)
- Combat contre des ennemis avec IA basique (patrouille → détection → poursuite/tir)
- Ramassage d'objets : soins, munitions, clés, trésors (bonus de score)
- Interface complète : menu principal, pause, réglages (audio/résolution/contrôles), écran de saisie du nom, tableau des scores
- Système de score et de progression

---

## Architecture

### Principe : découper l'état par durée de vie

L'état du jeu est réparti en trois structures, selon ce qui vit aussi longtemps que le programme, ce qui vit le temps d'une partie, et ce qui vit le temps d'un étage.

```c
// Durée de vie : tout le programme
typedef struct app {
    platform_t   *platform;   // fenêtre, clock, input brut
    assets_t     *assets;     // textures/fonts chargées une seule fois
    audio_t      *audio;
    settings_t    settings;   // volume, résolution, contrôles
    ui_t         *ui;         // scenes + machine à états d'interface
    session_t    *session;    // NULL tant qu'on est dans les menus
} app_t;

// Durée de vie : une partie
typedef struct session {
    level_t   *level;         // étage courant
    player_t   player;
    int        floor_number;
    int        score;
} session_t;

// Durée de vie : un étage
typedef struct level {
    int      **grid;
    int        w, h;
    vec2_t     spawn;
    vec2_t     exit;
    entities_t entities;      // ennemis + items de CET étage
} level_t;
```

### Modules (arborescence du repo)

```
wolf3d/
├── Makefile
├── README.md
├── include/
│   ├── wolf3d.h          # types partagés (vec2_t, enums) + app_t
│   ├── platform.h        # fenêtre, clock, input
│   ├── assets.h
│   ├── level.h
│   ├── entities.h
│   ├── renderer.h
│   ├── audio.h
│   └── ui.h
├── src/
│   ├── main.c
│   ├── core/             # boucle principale, cycle de vie app_t / session_t
│   ├── engine/           # raycasting, texture mapping, sol/plafond, z-buffer
│   ├── map/              # génération BSP, API level_t
│   ├── entities/         # joueur, ennemis, items, IA
│   ├── ui/               # scenes, zones cliquables, rendu des menus
│   └── audio/            # musique et effets sonores
└── assets/
    ├── textures/         # murs, sprites, items
    ├── ui/               # backgrounds de menus (mainmenu, pause, settings, scores...)
    ├── fonts/
    └── sounds/
```

### Stack technique

| Domaine          | Choix                      |
|------------------|-----------------------------|
| Langage          | C11                        |
| Rendu / fenêtre  | CSFML (Graphics, Window, Audio) |
| Build            | Makefile                   |
| Génération map   | BSP maison                 |
| Plateforme cible | Linux (KDE Neon)            |

---

## Roadmap

- [ ] Définition des types et des headers publics par module
- [ ] Cycle de vie `app_t` / `session_t` / `level_t` (create / destroy)
- [ ] Génération de map par BSP
- [ ] Moteur de raycasting (murs)
- [ ] Texture mapping
- [ ] Rendu sol/plafond
- [ ] Déplacement et collisions du joueur
- [ ] Sprites (ennemis, items) + tri par distance
- [ ] IA ennemie basique
- [ ] Interface complète (menus, pause, settings, scores)
- [ ] Audio (musique, effets sonores)
- [ ] Polish & tests

---

## Build

```bash
make
./wolf3d
```

Cibles disponibles : `make` (build), `make clean`, `make fclean`, `make re`.

*(Dépendance : `libcsfml-dev` installable via `apt install libcsfml-dev` sur Debian/Ubuntu/KDE Neon)*

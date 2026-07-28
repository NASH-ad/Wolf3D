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
│   ├── player.h
│   ├── entities.h
│   ├── renderer.h
│   ├── audio.h
│   └── ui.h
├── src/
│   ├── main.c
│   ├── core/             # boucle principale, cycle de vie app_t / session_t
│   ├── engine/           # raycasting, texture mapping, sol/plafond, z-buffer
│   ├── map/              # génération BSP, API level_t
│   ├── player/
│   ├── entities/         # ennemis, items, IA
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

- [✅] Définition des types et des headers publics par module
- [ ] Cycle de vie `app_t` / `session_t` / `level_t` (create / destroy)
- [✅] Génération de map par BSP
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

Cibles disponibles : `make` (build), `make debug` (build avec `-g3` + AddressSanitizer), `make clean`, `make fclean`, `make re`, `make test_bsp`.

### Installer CSFML

> ⚠️ **Contrainte de version.** Le code cible **CSFML 2.6.x** (API 2.x). La dernière version amont est la 3.0, dont l'API est incompatible — n'installez pas la 3.x. La version majeure de SFML doit correspondre à celle de CSFML.

**Linux (Debian / Ubuntu / KDE Neon)**

```bash
sudo apt install libcsfml-dev
```

**Windows**

Le plus simple reste d'utiliser le devcontainer (voir plus bas) via Docker Desktop + WSL2 : mêmes dépendances que sous Linux, aucune installation manuelle. Pour un environnement Windows natif :

1. Télécharger l'archive CSFML **2.6.x** depuis <https://www.sfml-dev.org/download/csfml/>, en choisissant le paquet correspondant au compilateur **MinGW (GCC)** — l'archive fournit les dossiers `include/`, `lib/` et `bin/`.
2. Extraire l'archive, par exemple dans `C:\CSFML`.
3. Compiler en pointant le Makefile vers ces dossiers :
   ```bash
   make CFLAGS="-std=c11 -Wall -Wextra -Iinclude -IC:/CSFML/include" \
        LDFLAGS="-LC:/CSFML/lib -lcsfml-graphics -lcsfml-window -lcsfml-audio -lcsfml-system -lm"
   ```
4. Copier les `.dll` de `C:\CSFML\bin` **à côté de l'exécutable** avant de le lancer (Windows les cherche dans le dossier du binaire ou le `PATH`).

Le compilateur GCC/`make` sous Windows s'obtient via [MSYS2](https://www.msys2.org/) ou MinGW-w64. MSYS2 propose aussi CSFML directement via `pacman` — vérifier alors que le paquet installé est bien en 2.x avant de l'utiliser.

### Environnement conteneurisé (optionnel)

Un dossier `.devcontainer/` fournit un environnement de build reproductible (GCC, Make, CSFML, GDB, Valgrind) identique pour tous les contributeurs, quelle que soit leur machine hôte. Ouvrir le projet dans VS Code puis **"Reopen in Container"**. La compilation se fait dans le conteneur ; le jeu s'exécute ensuite sur l'hôte.

> **Le conteneur produit un binaire Linux (ELF), pas un `.exe` Windows.** Sur une machine Windows, le devcontainer tourne dans WSL2 (une VM Linux) : le binaire s'exécute donc **dans WSL2**, et sa fenêtre CSFML est affichée par WSLg comme une fenêtre Windows classique. On lance alors `./wolf3d` depuis le terminal WSL2, pas depuis PowerShell.
>
> Le devcontainer sert donc à garantir une compilation identique pour tous et à matcher l'environnement de correction (Linux) — il ne génère pas d'exécutable Windows distribuable. Pour un vrai `.exe` lançable sans WSL, il faut la compilation MinGW native décrite dans la section Windows ci-dessus. Ce projet étant corrigé sous Linux, cette dernière option n'est en pratique nécessaire pour aucun rendu.

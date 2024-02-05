# BAST

A C++ library to represent B Abstract Syntax Trees

Ce module est utilisé

- comme un sous-module de Atelier B

## Numéro de version

Il n'existe pas de moyen d'instropection pour identifier un numéro
de version.

## Dépendances

La bibliothèque repose sur Qt5Core et Qt5Xml. Tout logiciel utilisant
BAST doit donc se mettre en conformité avec leur licence:
- (un pointeur vers) le code source,
- les binaires,
- l'attribution,
- le texte de la licence LGPLv3.

## Compilation

Il n'existe pas de procédure d'installation dédiée pour ce module.

Les variables CMake suivantes sont définies par inclusion du fichier CMakeLists.txt:
- La cible BAST correspond à la bibliothèque `libBAST.a`
- Le dossier `$BAST_SOURCE_DIR` contient les en-têtes
- Le dossier `$BAST_BINARY_DIR` contient la bibliothèque.

### Instructions pour Windows

Ces instructions sont destinées à un environnement MSYS2/Mingw-64 comme décrit dans
https://projets-clearsy.com/projects/ratb/wiki

Soit `$SOURCE` le dossier où le clonage du dépôt git a été réalisé et `$BUILD` le dossier où la compilation sera effectuée.

La compilation s'effectue en faisant :

```sh
cd $BUILD
cmake -G Ninja -DCMAKE_BUILD_TYPE=release "$SOURCE"
cmake --build .
```

### Instructions pour Linux

Il faut avoir installé le paquet `libqtbase5-dev` (ou équivalent).

Soit `$SOURCE` le dossier où le clonage du dépôt git a été réalisé, `$BUILD` le dossier où la compilation sera effectuée.

La compilation s'effectue en faisant :

```sh
cd $BUILD
cmake -DCMAKE_BUILD_TYPE=release "$SOURCE"
cmake --build .
```


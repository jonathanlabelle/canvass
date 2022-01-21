# Dessiner sur un canevas ASCII


## Format Markdown (supprimer cette section avant la remise)

N'oubliez pas de bien exploiter le format Markdown.

Sauter une ligne pour changer de paragraphe.

Mettre les noms de fichier et bout de code courts entre apostrophes inversés.
Par exemple, si vous parlez du fichier `Makefile`.

Mettre les longs bouts de code dans des blocs de code (triples apostrophes
inversés). Par exemple, vous pouvez donner un exemple de commande comme suit:

```sh
$ make
$ ls -a
```

Utiliser des listes à puces ou des énumérations le plus possible (plus agréable
à lire). Par exemple, pour décrire le contenu du projet:

* `README.md`: ce fichier
* `Makefile`: permet d'automatiser la compilation
* etc.

Bien aérer le contenu du fichier source (`README.md`). Éviter les longues
lignes dans le fichier Markdown (par exemple, limiter à 80) pour une meilleure
lisibilité avec un éditeur de texte.

## Description

Ceci est un programme en C qui sert à créer un canvass ou à un lire un à partir d'un
fichier texte. Il est possible à travers diverses options d'écrire et de colorier
dans le canvass à partir de la ligne de commande. Ce projet à été créer dans le cadre
du cours INF3135 à l'Université Du Québec À Montréal donné par Serge Dogny.

La description du travail demandé peut se retrouver ici: sujet du [sujet du travail](sujet.md)

## Auteur

Jonathan Labelle (LABJ28039006)

## Fonctionnement

Ce programme doit être lancé lors par la ligne de commande. 

Pour être utilisable, il doit d'abord être compilé comme ceci:

```sh
make
```

Pour créer un canvass vide, l'option `-n` doit être utilisé, ainsi que la hauteur 
et la largeur du canvass (à noté que la hauteur maximale est de 40 et largeur 80).

```sh
./canvascii -n 20,20
```

Pour lire un canvass à partir d'un fichier texte, utilisez l'option `-s` ainsi qu'une
redirection dans la ligne de commande.

```sh
./canvascii -s < chemin du fichier
```

Pour faire une ligne horizontale ou verticale, utlisez les options `-h` ou `-v` ainsi
que la ligne à laquelle la ligne sera.

```sh
./canvascii -n 20,20 -h 4 -v 8
```

Pour créer un rectangle ou une ligne, utilisez les options `-r` ou `-l`. Les deux premières
coodonnées à entrer sont la le point de départ. Pour le rectangle, les deux autres sont la hauteur
la largeur et pour la ligne ce sont les coordonnées du point d'arrivé. À noté que la hauteur et largeur
du rectangle ne peuvent être négatives.

```sh
./canvascii -n 20,20 -l 1,2,3,4 -r 4,7 4,5 
```

Pour créer cercle, utilisez l'option `-c`. Les 2 premiers chiffres serrent de point
de départ et le troisième chiffre sert de rayon. À noter que le rayon ne peut être négatif.

```sh
./canvascii -n 20,20 -c 7,7,4
```

Pour changer la couleur du pinceau, utilisez l'option `-p`. Utilisez un chiffre entre 0 et 7. 
Les numéros sont respectivement associés aux couleurs noire,
rouge, vert, jaune, bleu, magenta, cyan, blanc.

```sh
./canvascii -n 20,20 -c 7,7,4 -p 4 -h 5
```

Pour imprimer sur la console le canvass en couleur, utilisez l'option `-k`

```sh
./canvascii -n 20,20 -c 7,7,4 -p 4 -h 5 -k
```

Pour avoir ce fichier en html ou bien le sujet.md, il faut utiliser cette commande

```sh
make html
```

## Tests

Pour lancer la suite de test, utilisez la commande 

```sh
make test
```

Sur le serveur Unix de l'uqam, tout les tests fonctionnent.

## Dépendances

Ce programma a trois dépendances:

- [GCC](https://gcc.gnu.org/)
- [Bats](https://github.com/bats-core/bats-core)
- [Pandoc](https://pandoc.org/index.html)


## Références

Pour les algorithmes de Brensenham (pour la ligne et pour le cercle): le code est une version adaptée de 
ce qu'on peut trouver [ici](https://gist.github.com/bert/1085538) du code de Bert Timmerman.

Ainsi, le code de couleur ANSI a été fourni par Serge Dogny sur le Mattermost du cours


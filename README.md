# ASE

Marc BALOUP - Pape BADIANE

Université Lille 1 - Master 1 Semestre 1 - 2016/2017

## `try_throw`

Exécuter (Utilisation : entrer des nombres successivement)

    make
    ./try

* `Ctrl+D` pour arrêter et afficher résultat
* `0` pour arrêter et afficher 0 (throw)


## `switch_to`

Comportement si un des contexte se termine : retour au main (simple à implémenter).
Exécuter :

    make
    ./switch_to


## `ordonnanceur`

Exécuter :

    make
    ./context


## `filesystem`



Exécuter :

    make
        // les commandes nécessitent la variable d'env. HW_CONFIG
    ./dmps (couche drive ; dump un secteur du disque)
    ./fmrt (couche drive ; formater tout le disque)
    ./mkvol (couche vol ; créer un volume)
    ./dvol (couche vol ; affiche les volumes)
    ./rmvol (couche vol ; supprimer un volume)
        // la suite nécessite la variable d'environnement CURRENT_VOLUME
    ./mknfs (couche fs ; initiliser le système de fichier dans le volume (super block et free block))
    ./dfs (couche fs ; afficher les infos (espace libre) du volume)
    ./valid_super (couche fs ; effectue des tests sur la gestion de l'espace libre)
    ./valid_inode (couche fs ; effectue des tests sur la gestion des inodes (blocs de données, direct, indirect, dbl-indirect))
    ./if_nfile (couche ifile ; créer un fichier et retourne l'inoeud sur stdout. Le fichier est lu depuis stdin)
    ./if_pfile (couche ifile ; affiche sur stdout le contenu du fichier)
    ./if_dfile (couche ifile ; supprime un fichier via son inoeud)
    ./if_cfile (couche ifile ; copie un fichier déjà présent dans le volume, vers un nouvel inoeud)
    ./valid_dir (couche ftree ; effectue des tests sur la gestion de l'arborescence du disque)
    

## `virtualisation`







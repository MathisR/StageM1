# StageM1
Rapport et code source de mon stage.

## Articles:
Les références citées dans la bibliographie se trouvent dans le dossier `Articles` au format pdf, à l'exception de la dernière référence, qui n'est pas un article mais une page internet accessible normalement en ligne.

## Code:
Les programmes importants réalisés pendant mon stage se trouvent dans le dossier `Code`.
- Le fichier `enum_c.c` s'occupe de la génération de tous les N-ominos par la méthode de Redelmeier, de l'éviction des trous et des formes non-canoniques, d'appeler le programme de `beauquier_nivat.c` pour éliminer les polyominos exacts, et d'appeler le programme de `genLPbit_c.c` pour lancer la génération et la résolution du programme linéaire, avant d'afficher les résultats globaux.
- Le fichier `enum_alea0_c.c` réalise la même chose mais avec un moyen de paramétrer le pourcentage des polyominos trouvés que l'on veut tester, afin par exemple d'accélerer l'exécution en ne traitant qu'une partie aléatoire des données.
- Le fichier `beauquier_nivat.c` s'occupe de vérifier si un mot passé en argument est un pseudo-hexagone ou un pseudo-carré.
- Le fichier `genLPbit_c.c` s'occupe, une fois un polyomino passé en argument sous le format liste de coordonnées, de générer la liste des motifs, puis la liste des configurations, et enfin de créer l'environnement gurobi et de transformer les équations de formation des configurations en contraintes sur les densités.

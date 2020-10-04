## Utilisation de Perf

#### Commandes utiles:

`perf list`

#### Méthodologie de mesure:

- Fabrication de 4 applications `t1`, `t2`, `t4` et `t8`, compilées suivant 4 modes: `-vec -es 1`, `-vec -es 0`,`-scal -es 1` et `-scal -es 0`,
- Pour chaque exécution, on fait varier le nombre d'effets actifs, de 0 à 8 pour `t8`, et on mesure l'utilisation cpu en utilisant la commande `perf stat -d -p <pid>` . Le `<pid>` est déterminé en utilisant la commande `ps -e -T |grep t8`


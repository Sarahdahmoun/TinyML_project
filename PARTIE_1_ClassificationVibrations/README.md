# Partie 1 - Vibration Classification 

## Objectif 
L’objectif de cette partie est de concevoir un système TinyML capable de reconnaître différents états de vibration à partir des données du capteur inertiel de la carte Arduino Nano 33 BLE.

Le système doit :
- mesurer les vibrations avec l’Arduino Nano 33 BLE,
- entraîner un modèle de classification,
- exécuter l’inférence directement sur la carte.

Les états étudiés sont :
- **off** : absence de vibration,
- **normal** : vibration régulière,
- **dysfunction** : vibration irrégulière ou anormale.

## Principe général
Les données de vibration sont mesurées par l’accéléromètre de l’IMU.
Chaque mesure contient trois axes : ax, ay et az.

Les signaux sont découpés en fenêtres temporelles. Chaque fenêtre représente un état de fonctionnement.
Un modèle de machine learning est entraîné à reconnaître ces états.

## Étape 1 – Génération et collecte des données
Un programme Arduino initialise l’IMU et lit l’accéléromètre.
Les données brutes (ax, ay, az) sont envoyées vers le PC via la liaison série.

Les vibrations sont générées à l’aide d’un smartphone.
Trois situations sont enregistrées :
- carte immobile (off),
- vibration régulière (normal),
- vibration irrégulière (dysfunction).

Les données sont sauvegardées dans des fichiers CSV.
Chaque fichier CSV correspond à un état.

## Étape 2 – Entraînement du modèle
Les fichiers CSV sont importés dans un notebook Jupyter.
Les signaux sont découpés en fenêtres temporelles de taille fixe.

Un réseau de neurones simple est entraîné à partir de ces fenêtres.
Les données sont séparées en ensembles, d’entraînement et de test.

Le modèle entraîné est converti au format TensorFlow Lite (.tflite), car celui-ci est adapté aux microcontrôleurs.


## Étape 3 – Inférence sur Arduino
Le modèle TensorFlow Lite est intégré dans un programme Arduino.
L’IMU est lue en temps réel.

La classe prédite (off, normal ou dysfunction) est affichée sur le moniteur série.
L’inférence est exécutée directement sur l’Arduino.




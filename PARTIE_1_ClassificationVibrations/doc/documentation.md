# Documentation détaillée – Classification de vibrations avec TinyML

## Introduction

Cette partie du projet consiste à créer un système capable de reconnaître différents types de vibrations à l'aide de l'Arduino Nano 33 BLE et de son capteur IMU intégré.

Le projet est divisé en trois étapes principales :

1. Génération et collecte des données de vibration avec l'Arduino
2. Entraînement d'un modèle de classification avec TensorFlow
3. Déploiement du modèle sur l'Arduino pour faire l'inférence en temps réel

L'objectif final est de détecter automatiquement différents états de vibration.

Dans ce projet les classes utilisées sont :

off  
normal  
dysfunction


## Matériel utilisé

Carte microcontrôleur

Arduino Nano 33 BLE

Capteurs utilisés

IMU intégré (LSM9DS1)

Ce capteur permet de mesurer :

accélération sur l'axe X  
accélération sur l'axe Y  
accélération sur l'axe Z


## Bibliothèques utilisées

Arduino

Arduino_LSM9DS1

TensorFlow Lite Micro

tensorflow/lite/micro/all_ops_resolver.h  
tensorflow/lite/micro/micro_interpreter.h  
tensorflow/lite/micro/micro_error_reporter.h


## Étape 1 – Génération et collecte des données

Les données sont collectées à l'aide du capteur IMU intégré dans l'Arduino Nano 33 BLE.

Le programme Arduino initialise le capteur puis lit l'accélération sur les trois axes.

Le capteur est échantillonné à une fréquence de 50 Hz, ce qui correspond à une mesure toutes les 20 ms.

Code utilisé : data_generation.ino

Fonctionnement du programme

1. Initialisation du port série
2. Initialisation du capteur IMU
3. Lecture de l'accélération
4. Envoi des données vers le PC via Serial

Les données envoyées ont le format suivant :

ACC,t_ms,ax,ay,az

Exemple :

ACC,10234,0.012,-0.987,0.045

où

t_ms représente le temps en millisecondes  
ax ay az représentent les valeurs d'accélération sur les trois axes

Ces données sont ensuite enregistrées sur l'ordinateur pour constituer le dataset utilisé pour l'entraînement.


## Étape 2 – Entraînement du modèle

Les données collectées sont utilisées pour entraîner un modèle de classification dans le notebook :

training_vibrations.ipynb

Prétraitement des données

Les données sont organisées sous forme de fenêtres temporelles.

Chaque fenêtre contient :

50 échantillons  
3 variables par échantillon (ax ay az)

La taille d'entrée du modèle est donc :

50 x 3

Chaque fenêtre correspond à environ 1 seconde de données car la fréquence d'échantillonnage est de 50 Hz.

Les données sont ensuite associées à une étiquette correspondant au type de vibration.

Classes utilisées

off  
normal  
dysfunction

Le modèle est entraîné avec TensorFlow puis converti au format TensorFlow Lite.

Le modèle final est exporté sous forme d'un fichier :

vibration_model.tflite

Ce fichier est ensuite converti en fichier header pour être intégré dans Arduino :

vibration_model.h


## Étape 3 – Inférence sur Arduino

Le programme d'inférence est contenu dans :

inference_vibrations.ino

Le programme réalise les étapes suivantes :

1. Initialisation du port série
2. Initialisation du capteur IMU
3. Chargement du modèle TensorFlow Lite
4. Allocation de la mémoire pour le modèle
5. Lecture des données du capteur
6. Construction d'une fenêtre de 50 échantillons
7. Envoi de la fenêtre au modèle
8. Prédiction de la classe

Le modèle attend en entrée un tenseur de dimension :

1 x 50 x 3

Le programme construit donc une fenêtre contenant :

50 échantillons d'accélération  
3 axes pour chaque échantillon

Ces valeurs sont ensuite copiées dans le tenseur d'entrée du modèle.

L'inférence est ensuite exécutée avec la fonction :

interpreter->Invoke()


## Mémoire utilisée

Le modèle TensorFlow Lite est exécuté avec TensorFlow Lite Micro.

Une zone mémoire appelée tensor arena est utilisée pour stocker les tenseurs.

Dans ce projet la taille utilisée est :

30 KB


## Résultat de l'inférence

Après l'exécution du modèle, le programme récupère les probabilités associées aux différentes classes.

Exemple de sortie dans le Serial Monitor :

Pred: normal | probs = [0.02, 0.95, 0.03]

Cela signifie que la vibration détectée correspond très probablement à l'état normal.


## Fonctionnement global du système

Le fonctionnement complet du système peut être résumé ainsi :

Arduino IMU  
↓  
Lecture des accélérations (ax ay az)  
↓  
Création d'une fenêtre de 50 échantillons  
↓  
Entrée dans le modèle TensorFlow Lite  
↓  
Prédiction de la classe de vibration  
↓  
Affichage du résultat sur le Serial Monitor


## Conclusion

Ce projet démontre l'utilisation de TinyML pour effectuer de la classification de signaux directement sur un microcontrôleur.

L'Arduino Nano 33 BLE permet de :

collecter des données de capteurs  
entraîner un modèle sur ordinateur  
exécuter le modèle directement sur la carte

Cela permet de réaliser des systèmes intelligents capables de reconnaître des comportements physiques en temps réel.

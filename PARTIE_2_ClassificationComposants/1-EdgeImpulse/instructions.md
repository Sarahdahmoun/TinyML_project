# Reproduction du projet Edge Impulse

# Étape 1 : Création du projet

Créer un nouveau projet sur la plateforme Edge Impulse Studio.

Nom du projet :
ClassificationComposants

Objectif du projet :
Créer un modèle capable de reconnaître différents composants électroniques à partir d’images, puis de déployer ce modèle sur une carte Arduino Nano 33 BLE.

Les classes utilisées dans ce projet sont :

- background
- capacitor
- diode
- led
- resistor


# Étape 2 : Import du dataset

Les images utilisées dans ce projet ont été collectées au préalable.

Le dataset contient :

- 50 images de resistor
- 50 images de led
- 50 images de diode
- 50 images de capacitor
- 50 images de background

Pour importer les données :

1. Aller dans la section Data acquisition dans Edge Impulse.
2. Cliquer sur Upload existing data.
3. Cliquer sur Choose Files.
4. Sélectionner toutes les images correspondant à une classe (par exemple resistor).
5. Entrer le nom du label correspondant dans le champ Label.
6. Laisser l’option Automatically split between training and testing activée.
7. Cliquer sur Begin upload.

Répéter ces étapes pour chaque classe du dataset.

Une fois l’import terminé, vérifier que les images sont bien réparties entre training set et test set.


# Étape 3 : Création de l’Impulse

Aller dans la section Impulse design puis cliquer sur Create impulse.

Configurer les paramètres suivants :

Image width : 64  
Image height : 64  
Resize mode : Fit shortest axis  

Ensuite ajouter les blocs suivants :

- Image
- Transfer Learning (Images)

Le pipeline doit être :

Image data -> Image -> Transfer Learning

Cliquer ensuite sur Save Impulse.


# Étape 4 : Génération des features

Aller dans le bloc Image dans la partie gauche de l’interface.

Configurer le paramètre suivant :

Color depth : Grayscale

Puis effectuer les actions suivantes :

1. Cliquer sur Save parameters.
2. Cliquer sur Generate features.

Cette étape permet à Edge Impulse de transformer les images et d’extraire les caractéristiques utilisées pour entraîner le modèle.


# Étape 5 : Entraînement du modèle

Aller dans la section Transfer Learning.

Configurer les paramètres d’entraînement :

Training cycles : 40  
Learning rate : valeur par défaut  
Training processor : CPU  

Cliquer ensuite sur Start training.

Une fois l’entraînement terminé, vérifier :

- l’accuracy du modèle
- la confusion matrix
- la séparation entre les différentes classes




# Étape 6 : Déploiement du modèle

Aller dans la section Deployment.

Configurer les paramètres suivants :

Deployment target : Arduino Library  
Inference engine : EON Compiler  
Model optimization : Quantized (int8)

Cliquer ensuite sur Build.

Edge Impulse génère alors une bibliothèque Arduino contenant le modèle entraîné.

Télécharger le fichier zip généré.

Cette bibliothèque sera ensuite utilisée dans Arduino IDE pour exécuter le modèle sur la carte Arduino Nano 33 BLE.

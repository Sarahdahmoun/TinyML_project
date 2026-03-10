# TinyML_project
# Projet TinyML – Instrumentation 3

## Présentation générale

Ce projet a pour objectif d’explorer le Tiny Machine Learning (TinyML) sur des systèmes embarqués à l’aide de la carte Arduino Nano 33 BLE.  
Le but est de collecter des données à partir de capteurs ou d’images, d’entraîner un modèle de machine learning, puis de déployer ce modèle directement sur la carte Arduino afin de réaliser de l’inférence en temps réel.

Le projet est divisé en deux parties principales :

- **Partie 1 : Classification de vibrations**
- **Partie 2 : Classification et comptage de composants électroniques**

---

## Matériel utilisé

- Arduino Nano 33 BLE
- Caméra OV7670
- Composants électroniques (LED, résistances, condensateurs)
- Ordinateur pour l’entraînement et la communication avec la carte

Logiciels et outils utilisés :

- Arduino IDE
- Edge Impulse
- Node-RED
- Python / Jupyter Notebook

## Partie 1 – Classification de vibrations

L’objectif de cette partie est de reconnaître différents types de vibrations à l’aide du capteur IMU intégré dans l’Arduino Nano 33 BLE.

Étapes principales :

1. Collecte des données de vibrations à l’aide de l’IMU.
2. Transmission des données vers l’ordinateur.
3. Prétraitement et entraînement d’un modèle de classification.
4. Conversion du modèle en TensorFlow Lite.
5. Déploiement du modèle sur l’Arduino pour réaliser l’inférence en temps réel.

---

## Partie 2 – Classification de composants électroniques

Cette partie utilise la vision par ordinateur pour reconnaître des composants électroniques à partir d’images capturées par la caméra OV7670.

Étapes principales :

1. Création d’un dataset d’images de composants électroniques.
2. Annotation et entraînement du modèle sur Edge Impulse.
3. Export du modèle sous forme de bibliothèque Arduino.
4. Intégration du modèle dans le code Arduino pour effectuer l’inférence.
5. Envoi des résultats vers  Node-RED afin de compter les composants détectés et les afficher sur un tableau de bord.

Lien du projet Edge Impulse :

https://studio.edgeimpulse.com/public/902176/live




## Auteur

Projet réalisé dans le cadre du module **Instrumentation 3 – TinyML**.


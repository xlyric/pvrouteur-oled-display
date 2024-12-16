# Afficheur Routeur

## Description
Ce projet est un afficheur OLED sous ESP8266 (Wemos D1 mini) conçu pour récupérer et afficher les informations du routeur photovoltaïque. L'afficheur trouve automatiquement le routeur sur le réseau grâce à la découverte mDNS.

## Fonctionnalités
- **Découverte mDNS** : Trouve automatiquement le routeur photovoltaïque sur le réseau.
- **WiFi AutoConnect** : Simplifie la connexion WiFi lors de la première installation.
- **Mise à jour de firmware à distance** : Accessible via la page `/update`.
- **Affichage OLED** : Utilise un écran OLED connecté aux pins D1 (SDA) et D2 (SCL).

## Matériel Requis
- Wemos D1 mini (ESP8266)
- Écran OLED
- Câbles de connexion

## Connexions
- **SDA** : Pin D1
- **SCL** : Pin D2
- **GND** : Pin GND
- **VCC** : Pin 5v

## Installation
1. Connectez l'écran OLED aux pins D1 et D2 du Wemos D1 mini, ainsi que GND et VCC. 
2. Téléversez le firmware sur l'ESP8266.
3. Lors de la première installation, utilisez WiFi AutoConnect pour configurer la connexion WiFi.
4. Accédez à la page `/update` pour les mises à jour de firmware futures.

## Boîtier
Le modèle de boîtier pour l'impression 3D se trouve sur internet. [Lien vers le modèle](https://www.thingiverse.com/thing:2884823)

## Licence
Ce projet est sous licence MIT.

## Acknowledgements
Merci à la communauté open-source pour les bibliothèques et les outils utilisés dans ce projet.

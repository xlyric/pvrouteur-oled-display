# Afficheur Routeur

## Description
Ce projet est un afficheur OLED sous ESP8266 (Wemos D1 mini) conçu pour récupérer et afficher les informations du routeur photovoltaïque. L'afficheur trouve automatiquement le routeur sur le réseau grâce à la découverte mDNS.
Il fait parti du microcosme d'applications autour du [ routeur Solaire Xlyric ](https://ota.apper-solaire.org/) et gracieusement offert à la communautée par [Sunstain Tech Solutions](https://sunstain.fr)

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

## le fonctionnement : 
L'ensemble du projet fonctionne sous forme de tasks qui sont lancées : 
- la récupération de données ( Task_getdata )
- l'affichage Oled ( Task_oled )
- l'affichage web ( par ESPAsyncWebServer ), mais n'est vraiment utilisée que pour l'update on the air. 

Au démarrage, l'afficheur cherche sur le réseau le routeur par un appel mDNS, et se connecte dessus pour récuperer les informations de routage solaire. 

il est possible de rajouter des informations météo, dans ces cas là, il faut se créer une clé API en suivant [les instructions de la page web] (https://docs.thingpulse.com/how-tos/openweathermap-key/) 

à partir de là, il faut injecter les informations sur la page Web de votre écran : 
http://monIP.oled/set?city=maville,FR
et 
http://monIP.oled2/set?id=ma_cle_API

les informations seront stockées en ROM jusqu'a "Erase Flash" de l'ESP8266 

# Demo Vidéo 

[![Démo de l'afficheur](https://img.youtube.com/vi/-gTVRqzRFMw/0.jpg)](https://youtube.com/shorts/-gTVRqzRFMw)

## Boîtier
Le modèle de boîtier pour l'impression 3D se trouve sur internet. [Lien vers le modèle](https://www.thingiverse.com/thing:2884823)

Il existe aussi sur Aliexpress l'ensemble en un seul pack, il reste juste souder les composants et à uploader le projet
[ici](https://fr.aliexpress.com/item/1005006902051922.html)
[ou là](https://fr.aliexpress.com/item/1005006293267065.html)

## Licence
Ce projet est sous licence MIT.

## Acknowledgements
Merci à la communauté open-source pour les bibliothèques et les outils utilisés dans ce projet.

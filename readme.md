---
author: Xavier NAXARA
output: pdf_document
title: Rendu Projet SEC
date: 21 Avril 2023
---


# Note de rendu intermediaire

## Questions traités

Le code que je rends traite complètement les questions 1, 2, 3, 4 et 5 du sujet. La question 6 est presque complètement traitée, car j'ai réussi à implémenter la majorité des fonctionnalités, mais je n'ai pas pu aller jusqu'au bout de la résolution du problème. En effet il existe encore des comportement indéfini et des bugs qui ne sont pas résolus. 

## Choix de conceptions

En ce qui concerne mes choix de conception, j'ai opté pour une approche modulaire en créant plusieurs fonctions pour chaque question, ce qui m'a permis d'avoir un code plus clair et plus facile à maintenir. J'ai également utilisé des structures de données appropriées pour stocker les informations nécessaires à chaque question.

### Concernant la liste des processus

Lorsqu'une commande est exécutée, le programme crée un nouveau processus en utilisant la fonction ```fork()```. Si le processus enfant est créé avec succès, il exécute la commande à l'aide de la fonction ```execlp()```. Si ```execlp()``` échoue, le processus enfant retourne une valeur de sortie différente de zéro. Si la commande est exécutée en arrière-plan, le programme ne doit pas attendre que le processus enfant se termine avant de continuer l'exécution, sinon il doit attendre que le processus enfant se termine avant de supprimer les informations du job.

Pour gérer les jobs, le programme utilise un tableau de structures ```struct Job```. Chaque structure Job contient les informations suivantes :

- id : un identifiant unique pour chaque job, attribué par le programme
- pid : l'identifiant du processus correspondant au job
- status : l'état du job, qui peut être ACTIF, SUSPENDU ou TERMINE
- cmd : une chaîne de caractères contenant la commande qui a lancé le job

Le programme utilise des fonctions pour ajouter, supprimer et afficher des jobs, ainsi que pour gérer les signaux. En particulier, le programme utilise le signal SIGCHLD pour détecter la fin de l'exécution d'un processus enfant. Lorsque le signal SIGCHLD est reçu, le programme met à jour les informations sur le job correspondant et supprime les jobs qui se sont terminés.

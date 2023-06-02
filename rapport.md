---
title: Rapport du Projet de SEC
author: Xavier Naxara
date: 2 Juin 2023
output: pdf_document
margin-left: 3cm
margin-right: 3cm
margin-top: 3cm
margin-bottom: 3cm
---

# Introduction

Ce document est le rapport du projet de SEC. Il a pour but de présenter les
différentes étapes de la réalisation du projet, ainsi que les choix qui ont été
faits.

# Fonctionnalités du minishell

## Question traitées

Les question 1 à 6 et 9 à 11 sont complètement traitées.

## Question non traitées

Je n'ai pas réussi à traiter les question 7 et 8. J'ai donc retiré les fonctions 
associées à ces questions du code car elle provoquait des bugs.

# Architecture de l'application

## Structure du projet

Le projet est structuré en plusieurs fichiers. 

Le fichier `minishell.c` contient la fonction `main` qui est le point d'entrée
du programme. Il contient également les fonctions internes au minishell.

Les fichiers `job_manager.c` et `job_manager.h` contiennent les fonctions et les
structures de données permettant de gérer les jobs. 

Enfin les fichiers `readcmd.c` et `readcmd.h` permettent de lire les commandes 
et d'extraire les arguments.

# Choix et spécificités de conception

## Gestion des jobs

Les jobs sont stockés dans un tableau de taille dynamique, avec des `malloc` et
`realloc`.
Une amélioration de mon projet serait de stocker les jobs dans une liste chaînée, cela
permettrait de gérer plus facilement les suppressions et les insertions et d’éviter des
erreur de segmentation.

## Gestion des commandes

Les commandes internes au minishell sont gérées dans le fichier `minishell.c`. Elles sont reconnues
par un une fonction is_internal_command qui renvoie un booléen. Ensuite elle sont exécutées par la fonction
execute_internal_command.

Les commandes externes quand à elles sont lancées par le biais d'un processus fils. Le processus fils exécute
la commande grâce à la fonction execvp. Le processus père attend la fin de l'exécution de la commande grâce à la
primitive wait.

# Méthodologie de tests

J'ai développé un script de test (`test.sh`) qui permet de tester un échantillon de commandes du minishell. 
Pour tester les résultats de ces commandes, je compare la sortie du minishell avec la sortie du bash pour des 
commandes équivalentes.
Mes tests ne sont pas exhaustifs mais permettent de tester les fonctionnalités principales du minishell, et ainsi 
avoir une méthode de suivi de potentiels nouveau bugs lors du développement.

Pour lancer les tests, il suffit de lancer la commande `sh test.sh` dans le dossier du projet avec l'application déjà compilée.

# Conclusion

Ce projet m'a permis de me familiariser avec le language C, et de comprendre les bases de la programmation système.
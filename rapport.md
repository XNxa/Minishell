---
title: Rapport du Projet de SEC
author: Xavier Naxara
date: 2 Juin 2023
output: pdf_document
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
Les jobs sont stockés dans un tableau de taille dynamique. Une amélioration de mon
projet serait de stocker les jobs dans une liste chaînée.

Enfin les fichiers `readcmd.c` et `readcmd.h` permettent de lire les commandes 
et d'extraire les arguments.

# Choix et spécificités de conception



# Méthodologie de tests

# Conclusion


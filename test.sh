#!/bin/bash

# Test des fonctions externes

# Vérification de l'existence de ./minishell
if [ ! -f "./minishell" ]; then
    echo "Le fichier ./minishell n'existe pas."
    echo "Veuillez compiler le minishell avec la commande 'make all'."
    exit 1
fi

if [ ! -x "./minishell" ]; then
    echo "Le fichier ./minishell n'est pas exécutable."
    exit 1
fi

echo "Test du minishell :"

if [ -f "test1" ]; then
    rm test1
fi
touch test1
if [ -f "test2" ]; then
    rm test2
fi
touch test2

# Mon minishell
{
    echo "pwd"
    echo "ls"
    echo "cat Makefile"
    echo "exit"
} | ./minishell >> test1

# Bash
pwd >> test2
ls >> test2
cat Makefile >> test2

diff test1 test2 | wc -l > a

if [ `cat a` -eq 5 ]; then
    echo "Commandes externes : OK"
else
    echo "Commandes externes : KO"
fi

rm test1 test2 a

sleep 0.1

# Test des commandes internes

mkdir dir1
mkdir dir2
touch dir1/file1
{
    echo "cd dir1"
    echo "ls"
    echo "cd ../dir2"
    echo "ls"
    echo "cd .."
    echo "pwd"
    echo "exit"
} | ./minishell >> test1

cd dir1
ls >> ../test2
cd ../dir2
ls >> ../test2
cd ..
pwd >> test2

head -n 2 test1 > b

diff b test2 | wc -l > a

if [ `cat a` -eq 0 ]; then
    echo "Commandes internes : OK"
else
    echo "Commandes internes : KO"
fi

# Nettoyage

rm -rf dir1 dir2 test1 test2 a b

sleep 0.1

# Test des redirection

{
    echo "echo salut > f1"
    echo "cat < f1 > f2"
    echo "exit"
} | ./minishell >> foo

diff f1 f2 | wc -l > a

if [ `cat a` -eq 0 ]; then
    echo "Redirection : OK"
else
    echo "Redirection : KO"
fi 

rm f1 f2 a

sleep 0.1

# Test des pipes

{
    echo "ls | grep h | wc -l > a"
    echo "exit"
} | ./minishell >> foo

ls | grep h | wc -l > b

diff a b | wc -l > c

if [ `cat c` -eq 0 ]; then
    echo "Pipes : OK"
else
    echo "Pipes : KO"
fi

rm a b c foo

sleep 0.1

# Test des commandes Jobs
echo "Test des commandes jobs : (attendre svp...)"

# Test de lj
{
    echo "sleep 5 &"
    echo "lj"
    sleep 2
    echo "lj"
    sleep 4
    echo "lj"
    echo "exit"
} | ./minishell >> foo

grep "sleep" foo | wc -l > a

if [ `cat a` -eq 2 ]; then
    echo "  Commande lj : OK"
else
    echo "  Commande lj : KO"
fi

rm a foo

# Test de sj
{
    echo "sleep 5 &"
    sleep 2
    echo "lj"
    echo "sj 0"
    sleep 4
    echo "lj"
    echo "fg 0"
    echo "exit"
} | ./minishell >> foo

grep "sleep" foo | wc -l > a
grep "state: 1" foo | wc -l > b
grep "state: 0" foo | wc -l > c

if [ `cat a` -eq 2 ] && [ `cat b` -eq 1 ] && [ `cat c` -eq 1 ]; then
    echo "  Commande sj+fg : OK"
else
    echo "  Commande sj+fg : KO"
fi

rm a b c foo

# Test de bg
{
    echo "sleep 5 &"
    sleep 2
    echo "sj 0"
    echo "lj"
    echo "bg 0"
    echo "lj"
    echo "exit"
} | ./minishell >> foo

grep "sleep" foo | wc -l > a
grep "state: 1" foo | wc -l > b
grep "state: 0" foo | wc -l > c

if [ `cat a` -eq 2 ] && [ `cat b` -eq 1 ] && [ `cat c` -eq 1 ]; then
    echo "  Commande sj+bg : OK"
else
    echo "  Commande sj+bg : KO"
fi

rm a b c foo

echo "Fin des tests."

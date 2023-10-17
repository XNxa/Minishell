**This project was developed as part of a course on Centralised Operating Systems during my first year at ENSEEIHT.**

## Goal

The aim of this project was to understand and reproduce a (mini) shell. This is a program that provides an interface to operating system functions.

## Usage

### How to use the minishell
```sh
git clone https://github.com/XNxa/Minishell.git
cd Minishell
make all
./minishell
```

### How to run the test file
**Please note: the tests are not exhaustive**
```sh
git clone https://github.com/XNxa/Minishell.git
cd Minishell
make all
./minishell
sh test.sh
```

## Features

- Internal commands on the minishell
  - exit : exit the program
  - cd : change directory, similar to the bash command
  - lj : List processes/jobs currently running
  - sj \[id\] : Pauses the "id" process
  - bg \[id\] : Restart a paused process in the background
  - fg \[id\] : Restart a paused process in the foreground
 
- External commands :
  The program can run any command installed on the system (More specifically, installed on the shell from which you run minishell).
  For example, `ls`, `git`, `whoami`, ...

- Pipes :
  The minishell is able to handle pipes. You can try with `echo "Hello World" | wc -w`, you should obtain 2.

- Redirecting :
  The minishell is also able to handle redirections. You can try with `echo "Hello World" > newfile`.

- Run program in background :
 You can run any programme in the background by adding `&` to your command line.


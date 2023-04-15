#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/wait.h> /* wait */
#include <string.h>
#include <stdbool.h>

#include "readcmd.h"


struct Job {
    int id;
    int pid;
    bool backgrounded;
    struct cmdline* cmd;
};

bool is_internal_cmd(struct cmdline* cmd);
void executer_internal_cmd(struct cmdline* cmd);
void executer_exit();
void executer_lj(struct cmdline* cmd);
void executer_cd(struct cmdline* cmd);

int main(void) {

    struct Job *jobs = malloc(sizeof(struct Job));

    do {
        printf(">>>");

        struct cmdline* cmd = readcmd();

        // Si la commande est vide
        if (cmd->seq[0] == NULL) {
            continue;
        } else if ( is_internal_cmd(cmd) ) {
            executer_internal_cmd(cmd);
            continue;
        }
        
        int pid = fork();
        int codeTerm;

        if (pid == -1) {
            printf("Erreur fork\n");
            exit(1);
        } else if (pid == 0) {
            execlp(cmd->seq[0][0], cmd->seq[0][0], NULL);
            return EXIT_FAILURE; // Si on execute ici c'est que l'exec a echoué
        } else {
            if (cmd->backgrounded) {
                printf("Lancement en tâche de fond !\n");
            } else {
                waitpid(pid, &codeTerm, 0);
                if (WEXITSTATUS(codeTerm) == 0){
                    printf("SUCCESS\n");
                } else {
                    printf("ECHEC\n");
                }
            }
        }
        
    } while (1);

    return EXIT_SUCCESS;
}

bool is_internal_cmd(struct cmdline* cmd) {
    char *internal_cmds[] = {"exit", "cd", "lj", "sj", "bg", "fg"};

    for (int i = 0; i < 6; i++) {
        if (strcmp(cmd->seq[0][0], internal_cmds[i]) == 0) {
            return true;
        }
    }
    return false;
}

void executer_internal_cmd(struct cmdline* cmd) {
    if (strcmp(cmd->seq[0][0], "exit") == 0) {
        executer_exit();
    } else if (strcmp(cmd->seq[0][0], "cd") == 0) {
        executer_cd(cmd);
    } else if (strcmp(cmd->seq[0][0], "lj") == 0) {
        executer_lj(cmd);
    } else if (strcmp(cmd->seq[0][0], "sj") == 0) {
        printf("sj\n");
    } else if (strcmp(cmd->seq[0][0], "bg") == 0) {
        printf("bg\n");
    } else if (strcmp(cmd->seq[0][0], "fg") == 0) {
        printf("fg\n");
    }
}

void executer_lj(struct cmdline* cmd) {
    printf("lj\n");
}

void executer_exit() {
    printf("Salut, à bientôt\n");
    exit(0);
}

void executer_cd(struct cmdline* cmd) {
    int res;
    if (cmd->seq[0][1] != NULL) {
        res = chdir(cmd->seq[0][1]);
    } else {
        res = chdir(getenv("HOME"));
    }

    if (res == 0) {
        printf("SUCCESS\n");
    } else {
        printf("ECHEC\n");
    }
}
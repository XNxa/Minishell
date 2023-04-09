#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/wait.h> /* wait */
#include <string.h>

#include "readcmd.h"

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

int main(void) {

    char buf[30];

    do {
        printf(">>>");

        struct cmdline* cmd = readcmd();

        // Commandes internes ?
        if (cmd->seq[0] == NULL) {
            continue;
        } else if ( strcmp(cmd->seq[0][0], "exit") == 0 ) {
            break;
        } else if ( strcmp(cmd->seq[0][0], "cd") == 0) {
            executer_cd(cmd);
            continue;
        } else {
            // Commande externe ...
            strcpy(buf, cmd->seq[0][0]);
        }
        
        int pid = fork();
        int codeTerm;

        if (pid == -1) {
            printf("Erreur fork\n");
            exit(1);
        } else if (pid == 0) {
            execlp(buf, buf , NULL);
            return EXIT_FAILURE; // Si on execute ici c'est que l'exec a echoué
        } else {
            if (cmd->backgrounded) {
                printf("Lancement en tâche de fond !\n");
            } else {
                wait(&codeTerm);
                if (WEXITSTATUS(codeTerm) == 0){
                    printf("SUCCESS\n");
                } else {
                    printf("ECHEC\n");
                }
            }
            
        }
        
    } while (1);

    printf("\nSalut, à bientôt\n");

    return EXIT_SUCCESS;
}

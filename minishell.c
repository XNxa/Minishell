#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/wait.h> /* wait */
#include <string.h>

int main(void) {

    char buf[30];
    char exite[30];
    strcpy(exite, "exit");

    do {
        printf(">>>");
        if (fgets(buf, sizeof(buf), stdin) == NULL) {
            break;
        }
        buf[strcspn(buf, "\n")] = '\0'; // Supprimer le caractère de fin de ligne
        
        if (strcmp(buf, "") == 0) {
            continue; // Appuie simple sur return
        } else if (strcmp(buf, exite) == 0) {
            break; // Quitter le shell
        } else if (strcmp(buf, "cd\0") == 0)
        {
            int res = chdir(getenv("HOME"));
            if (res == 0) {
                printf("SUCCESS\n");
            } else {
                printf("ECHEC\n");
            }
            continue;
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
            wait(&codeTerm);
            if (WEXITSTATUS(codeTerm) == 0){
                printf("SUCCESS\n");
            } else {
                printf("ECHEC\n");
            }
        }
        
    } while (1);

    printf("\nSalut, à bientôt\n");

    return EXIT_SUCCESS;
}

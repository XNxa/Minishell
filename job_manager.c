#include "job_manager.h"
#include <assert.h>   /* assert */
#include <stdio.h>    /* printf */
#include <stdlib.h>   /* exit */
#include <string.h>   /* strcmp */
#include <sys/types.h>
#include <sys/wait.h> /* wait */
#include <unistd.h>   /* fork, exec */

int nb_jobs = 0;
struct Job *jobs = NULL;

void add_job(int pid, char * cmd) {
    assert(cmd != NULL);

    jobs = realloc(jobs, (nb_jobs + 1) * sizeof(struct Job));
    if (jobs) {
        jobs[nb_jobs].id = nb_jobs;
        jobs[nb_jobs].pid = pid;
        jobs[nb_jobs].status = ACTIF;
        jobs[nb_jobs].cmd = malloc(sizeof(char) * (strlen(cmd) + 1));
        strcpy(jobs[nb_jobs].cmd, cmd);
        nb_jobs++;
    } else {
        printf("Erreur realloc add\n");
        exit(1);
    }
}


void del_job(int pid) {
    assert(nb_jobs > 0);

    if (nb_jobs == 1) {
        free(jobs[0].cmd);
        free(jobs);
        jobs = NULL;
        nb_jobs--;
    } else {
        for (int i = 0; i < nb_jobs; i++) {
            if (jobs[i].pid == pid) {
                free(jobs[i].cmd);
                jobs[i] = jobs[nb_jobs - 1];
                jobs = realloc(jobs, sizeof(struct Job) * (nb_jobs - 1));
                if (jobs) {
                    nb_jobs--;
                } else {
                    printf("Erreur realloc del\n");
                    exit(1);
                }
                break;
            }
        }
    }
}

void check_jobs() {
    int i = 0;
    while (i < nb_jobs) {
        int status;
        pid_t result = waitpid(jobs[i].pid, &status, WNOHANG);
        if (result == -1) {
            // Pas de processus fils
            i++;
        }
        else if (result == 0) {
            // Processus n'a pas terminé
            i++;
        }
        else {
            // Processus terminé
            if (WIFEXITED(status) || WIFSIGNALED(status)) {
                del_job(jobs[i].pid);
            } else if (WIFSTOPPED(status)) {
                jobs[i].status = SUSPENDU;
                i++;
            }
        }
    }
}
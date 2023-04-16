#include <stdio.h> /* printf */
#include <unistd.h> /* fork, exec */
#include <stdlib.h> /* exit */
#include <signal.h> /* signals */
#include <sys/types.h> 
#include <sys/wait.h> /* wait */
#include <string.h> /* strcmp */
#include <stdbool.h> /* bool */
#include <assert.h> /* assert */

#include "readcmd.h"

enum Status
{
    ACTIF,
    SUSPENDU,
    TERMINE
};

struct Job
{
    int id;
    int pid;
    enum Status status;
    char *cmd;
};

int nb_jobs = 0;
struct Job *jobs = NULL;

bool is_internal_cmd(struct cmdline *cmd);
void executer_internal_cmd(struct cmdline *cmd);
void executer_exit();
void executer_lj();
void executer_sj(struct cmdline* cmd);
void executer_bg(struct cmdline* cmd);
void executer_fg(struct cmdline* cmd);
void executer_cd(struct cmdline *cmd);
void add_job(int pid, struct cmdline *cmd);
void del_job(int pid);
void check_jobs();
void handler_chld(int sig);

int main(void)
{   
    struct sigaction sa;
    sa.sa_handler = handler_chld;
    sigemptyset(&sa.sa_mask);
    sa.sa_flags = 0;
    sigaction(SIGCHLD, &sa, NULL);

    do
    {
        printf(">>>");

        struct cmdline *cmd = readcmd();

        
        if (cmd == NULL)
        {
            continue;// Si le handler a été appelé et a supprimé un job
        }
        else if (cmd->seq[0] == NULL)
        {
            continue;// Si la commande est vide
        }
        else if (is_internal_cmd(cmd))
        {
            executer_internal_cmd(cmd);
            continue;
        }

        int pid = fork();
        int codeTerm;

        if (pid == -1)
        {
            printf("Erreur fork\n");
            exit(1);
        }
        else if (pid == 0)
        {
            execlp(cmd->seq[0][0], cmd->seq[0][0], NULL);
            return EXIT_FAILURE; // Si on execute ici c'est que l'exec a echoué
        }
        else
        {
            add_job(pid, cmd);
            if (cmd->backgrounded)
            {
                printf("Lancement en tâche de fond !\n");
            }
            else
            {
                waitpid(pid, &codeTerm, 0);
                del_job(pid);
                if (WEXITSTATUS(codeTerm) == 0)
                {
                    printf("SUCCESS\n");
                }
                else
                {
                    printf("ECHEC\n");
                }
            }
        }

    } while (1);

    return EXIT_SUCCESS;
}

bool is_internal_cmd(struct cmdline *cmd)
{
    char *internal_cmds[] = {"exit", "cd", "lj", "sj", "bg", "fg"};

    for (int i = 0; i < 6; i++)
    {
        if (strcmp(cmd->seq[0][0], internal_cmds[i]) == 0)
        {
            return true;
        }
    }
    return false;
}

void executer_internal_cmd(struct cmdline *cmd)
{
    if (strcmp(cmd->seq[0][0], "exit") == 0)
    {
        executer_exit();
    }
    else if (strcmp(cmd->seq[0][0], "cd") == 0)
    {
        executer_cd(cmd);
    }
    else if (strcmp(cmd->seq[0][0], "lj") == 0)
    {
        executer_lj();
    }
    else if (strcmp(cmd->seq[0][0], "sj") == 0)
    {
        executer_sj(cmd);
    }
    else if (strcmp(cmd->seq[0][0], "bg") == 0)
    {
        executer_bg(cmd);
    }
    else if (strcmp(cmd->seq[0][0], "fg") == 0)
    {
        executer_fg(cmd);
    }
}

void executer_lj()
{
    for (int i = 0; i < nb_jobs; i++)
    {
        printf("Id: %d | pid: %d | state: %d | cmd: %s\n", jobs[i].id, jobs[i].pid, jobs[i].status, jobs[i].cmd);
    }
}

void executer_sj(struct cmdline* cmd)
{
    if (cmd->seq[0][1] != NULL)
    {
        int id = atoi(cmd->seq[0][1]);
        bool trouve = false;
        for (int i = 0; i < nb_jobs; i++)
        {
            if (jobs[i].id == id)
            {
                jobs[i].status = SUSPENDU;
                kill(jobs[i].pid, SIGSTOP);
                trouve = true;
            }
        }
        if (!trouve)
        {
            printf("Aucun processus avec l'id %d\n", id);
        }
    }
    else
    {
        printf("Usage: sj <id>\n");
    }   
}

void executer_bg(struct cmdline* cmd)
{
    if (cmd->seq[0][1] != NULL)
    {
        int id = atoi(cmd->seq[0][1]);
        bool trouve = false;
        for (int i = 0; i < nb_jobs; i++)
        {
            if (jobs[i].id == id)
            {
                if (jobs[i].status == ACTIF)
                {
                    printf("Le processus %d est déjà actif\n", id);
                } else
                {
                    jobs[i].status = ACTIF;
                    kill(jobs[i].pid, SIGCONT);
                    trouve = true;
                }
            }
        }
        if (!trouve)
        {
            printf("Aucun processus avec l'id %d\n", id);
        }
    }
    else
    {
        printf("Usage: bg <id>\n");
    }   
}

void executer_fg(struct cmdline* cmd)
{
    if (cmd->seq[0][1] != NULL)
    {
        int id = atoi(cmd->seq[0][1]);
        bool trouve = false;
        for (int i = 0; i < nb_jobs; i++)
        {
            if (jobs[i].id == id)
            {
                if (jobs[i].status == ACTIF)
                {
                    printf("Le processus %d est déjà actif\n", id);
                } else
                {
                    jobs[i].status = ACTIF;
                    kill(jobs[i].pid, SIGCONT);
                    int codeTerm;
                    waitpid(jobs[i].pid, &codeTerm, 0);
                    if (WEXITSTATUS(codeTerm) == 0)
                    {
                        printf("SUCCESS\n");
                    } else {
                        printf("ECHEC\n");
                    }
                    
                    del_job(jobs[i].pid);
                    trouve = true;
                }
            }
        }
        if (!trouve)
        {
            printf("Aucun processus avec l'id %d\n", id);
        }
    }
    else
    {
        printf("Usage: fg <id>\n");
    }
}

void executer_exit()
{
    printf("Salut, à bientôt\n");
    exit(0);
}

void executer_cd(struct cmdline *cmd)
{
    int res;
    if (cmd->seq[0][1] != NULL)
    {
        res = chdir(cmd->seq[0][1]);
    }
    else
    {
        res = chdir(getenv("HOME"));
    }

    if (res == 0)
    {
        printf("SUCCESS\n");
    }
    else
    {
        printf("ECHEC\n");
    }
}

void add_job(int pid, struct cmdline *cmd)
{
    jobs = realloc(jobs, sizeof(struct Job) * (nb_jobs + 1));
    if (jobs)
    {
        jobs[nb_jobs].id = nb_jobs;
        jobs[nb_jobs].pid = pid;
        jobs[nb_jobs].status = ACTIF;
        jobs[nb_jobs].cmd = malloc(sizeof(char) * (strlen(cmd->seq[0][0]) + 1));
        strcpy(jobs[nb_jobs].cmd, cmd->seq[0][0]);
        nb_jobs++;
    }
    else
    {
        printf("Erreur realloc add\n");
        exit(1);
    }
}

void del_job(int pid)
{
    assert(nb_jobs > 0);

    if (nb_jobs == 1)
    {
        free(jobs[0].cmd);
        free(jobs);
        jobs = NULL;
        nb_jobs--;
    }
    else
    {
        for (int i = 0; i < nb_jobs; i++)
        {
            if (jobs[i].pid == pid)
            {   
                free(jobs[i].cmd);
                jobs[i] = jobs[nb_jobs - 1];
                jobs = realloc(jobs, sizeof(struct Job) * (nb_jobs - 1));
                if (jobs)
                {
                    nb_jobs--;
                }
                else
                {
                    printf("Erreur realloc del\n");
                    exit(1);
                }
                break;
            }
        }
    }
}

void handler_chld(int sig) {
    assert(sig == SIGCHLD);
    check_jobs();
}

void check_jobs() {
    int i = 0;
    while (i < nb_jobs)
    {
        int status;
        pid_t result = waitpid(jobs[i].pid, &status, WNOHANG);
        if (result == -1)
        {
            printf("Erreur waitpid\n");
            exit(1);
        }
        else if (result == 0)
        {
            // Processus n'a pas terminé
            i++;
        }
        else
        {
            // Processus terminé
            if (WIFEXITED(status) || WIFSIGNALED(status))
            {
                del_job(jobs[i].pid);
            }
            else if (WIFSTOPPED(status))
            {
                jobs[i].status = SUSPENDU;
                i++;
            }
        }
    }
}

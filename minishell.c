#define _POSIX_SOURCE
#define _DEFAULT_SOURCE

#include <stdio.h>  /* printf */
#include <unistd.h> /* fork, exec */
#include <stdlib.h> /* exit */
#include <signal.h> /* signals */
#include <sys/types.h>
#include <sys/wait.h> /* wait */
#include <string.h>   /* strcmp */
#include <stdbool.h>  /* bool */
#include <assert.h>   /* assert */
#include <fcntl.h>    /* open */

#include "readcmd.h"
#include "job_manager.h"

bool is_internal_cmd(struct cmdline *cmd);
void executer_internal_cmd(struct cmdline *cmd);
void executer_external_cmd(char ** cmd, int input_fd, int output_fd, bool backgrounded);
void executer_pipeline(struct cmdline *cmd);
void executer_exit();
void executer_lj();
void executer_sj(struct cmdline *cmd);
void executer_bg(struct cmdline *cmd);
void executer_fg(struct cmdline *cmd);
void executer_cd(struct cmdline *cmd);
void handler_chld(int sig);

extern int nb_jobs;
extern struct Job *jobs;

int main(void)
{
    struct cmdline *cmd = NULL;

    struct sigaction sa;
    sa.sa_handler = handler_chld;
    sigemptyset(&sa.sa_mask);
    sa.sa_flags = 0;
    sigaction(SIGCHLD, &sa, NULL);

    do
    {
        printf(">>> ");
        cmd = readcmd();
        if (cmd != NULL)
        {

            if (cmd->seq[0] == NULL)
            {
                continue; // Si la commande est vide
            }
            else if (is_internal_cmd(cmd))
            {
                executer_internal_cmd(cmd);
                continue;
            }

            executer_pipeline(cmd);

        }

    } while (1);

    return EXIT_SUCCESS;
}

void executer_pipeline(struct cmdline *cmd) 
{
    int i = 0;
    int input_fd = STDIN_FILENO;
    int output_fd = STDOUT_FILENO;
    int pipefd[2];
    
    if (cmd->in != NULL)
    {
        int fd = open(cmd->in, O_RDONLY);
        if (fd == -1)
        {
            perror("open");
            return;
        }
        input_fd = fd;
    } 
    if (cmd->out != NULL)
    {
        int fd = open(cmd->out, O_WRONLY | O_CREAT | O_TRUNC, 0644);
        if (fd == -1)
        {
            perror("open");
            return;
        }
        output_fd = fd;
    }

    while (cmd->seq[i+1] != NULL) {
        pipe(pipefd); // Créer un tube

        executer_external_cmd(cmd->seq[i], input_fd, pipefd[1], cmd->backgrounded);

        close(pipefd[1]); // Fermer l'extrémité d'écriture du tube
        input_fd = pipefd[0]; // L'extrémité de lecture devient l'entrée pour la prochaine commande
        i++;
    }

    executer_external_cmd(cmd->seq[i], input_fd, output_fd, cmd->backgrounded);
}

void executer_external_cmd(char** cmd, int input_fd, int output_fd, bool backgrounded)
{
    int pid = fork();
    int codeTerm;

    if (pid == -1)
    {
        printf("Erreur fork\n");
        exit(1);
    }
    else if (pid == 0)
    {   
        if (input_fd != -1)
        {
            dup2(input_fd, STDIN_FILENO);
        }
        if (output_fd != -1)
        {
            dup2(output_fd, STDOUT_FILENO);
        }
        execvp(cmd[0], cmd);
        exit(EXIT_FAILURE); // Si on execute ici c'est que l'exec a echoué
    }
    else
    {
        add_job(pid, cmd[0]);
        if (backgrounded)
        {
            printf("Lancement en tâche de fond !\n");
        }
        else
        {
            waitpid(pid, &codeTerm, 0);
            del_job(pid);
            if (WEXITSTATUS(codeTerm) != 0)
            {
                printf("ECHEC\n");
            }
        }
    }
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

void executer_sj(struct cmdline *cmd)
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

void executer_bg(struct cmdline *cmd)
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
                }
                else
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

void executer_fg(struct cmdline *cmd)
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
                }
                else
                {
                    jobs[i].status = ACTIF;
                    kill(jobs[i].pid, SIGCONT);

                    int codeTerm;
                    waitpid(jobs[i].pid, &codeTerm, 0);

                    waitpid(jobs[i].pid, &codeTerm, 0);
                    if (WEXITSTATUS(codeTerm) != 0)
                    {
                        printf("Echec\n");
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
    if (res != 0)
    {
        perror("cd");
    }
}

void handler_chld(int sig)
{
    assert(sig == SIGCHLD);
    check_jobs();
}

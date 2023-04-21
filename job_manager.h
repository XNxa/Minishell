
#ifndef JOB_MANAGER
#define JOB_MANAGER

#include <unistd.h>
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


/// @brief Ajouter un nouveau processus a la liste  
/// @param pid Pid du processus
/// @param cmd cmdline utilisée pour lancer le processus
void add_job(int pid, char * cmd);

/// @brief Supprimer un processus de la liste
/// @param pid Pid du processus
void del_job(int pid);

/// @brief Vérifier l'état de tous les processus de la liste
void check_jobs();

#endif
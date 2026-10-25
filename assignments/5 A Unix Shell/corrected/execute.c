/* CORRECTED: Assignment 5, Jeon Sangeon. Keep fork/execvp, but create
   every pipeline stage before waiting; close unused pipe endpoints. */
#include "execute.h"
#include "signals.h"
#include <assert.h>
#include <errno.h>
#include <fcntl.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/wait.h>
#include <unistd.h>

/* CORRECTED: Report errno with executable and operation context. */
static void reportSystemError(const char *pgmName, const char *context)
{
    int savedErrno = errno;
    assert(pgmName != NULL);
    assert(context != NULL);
    fprintf(stderr, "%s: %s: %s\n", pgmName, context,
            strerror(savedErrno));
}
/* CORRECTED: Change the parent for environment/cd builtins only. */
static void executeBuiltin(const struct Command *command,
                            const char *pgmName, int *exitRequested)
{
    char **args;
    int result = 0;
    assert(command != NULL);
    assert(pgmName != NULL);
    assert(exitRequested != NULL);
    args = command->pgmArgs;
    switch (command->type) {
        case SETENV:
            result = setenv(args[1], command->argc == 3 ? args[2] : "", 1);
            break;
        case UNSETENV: result = unsetenv(args[1]); break;
        case CD: {
            const char *directory = command->argc == 2 ? args[1] : getenv("HOME");
            if (directory == NULL) {
                fprintf(stderr, "%s: HOME is not set\n", pgmName);
                return;
            }
            result = chdir(directory);
            break;
        }
        case EXIT: *exitRequested = 1; break;
        default: assert(0);
    }
    if (result == -1) reportSystemError(pgmName, args[0]);
}
/* CORRECTED: Install fd as target; a child error exits via _exit. */
static void connectFd(int fd, int target, const char *pgmName)
{
    assert(fd >= 0);
    assert(pgmName != NULL);
    if (fd != target && dup2(fd, target) == -1) {
        reportSystemError(pgmName, "dup2");
        _exit(EXIT_FAILURE);
    }
}
/* CORRECTED: Open a redirect target; ensure output permission 0600. */
static void redirectFile(const char *path, int output,
                          const char *pgmName)
{
    int fd, target = output ? STDOUT_FILENO : STDIN_FILENO;
    assert(path != NULL);
    assert(pgmName != NULL);
    fd = output ? open(path, O_WRONLY | O_CREAT | O_TRUNC, 0600)
                : open(path, O_RDONLY);
    if (fd == -1) {
        reportSystemError(pgmName, path);
        _exit(EXIT_FAILURE);
    }
    if (output && fchmod(fd, 0600) == -1) {
        reportSystemError(pgmName, "fchmod");
        _exit(EXIT_FAILURE);
    }
    connectFd(fd, target, pgmName);
    if (fd != target) close(fd);
}
/* CORRECTED: Set up this child, close pipe FDs and replace its image. */
static void executeChild(const struct Command *command, int previous,
                           const int next[2], const char *pgmName)
{
    assert(command != NULL);
    assert(next != NULL);
    assert(pgmName != NULL);
    if (resetChildSignals() == -1) {
        reportSystemError(pgmName, "signal");
        _exit(EXIT_FAILURE);
    }
    if (previous != -1) connectFd(previous, STDIN_FILENO, pgmName);
    if (next[1] != -1) connectFd(next[1], STDOUT_FILENO, pgmName);
    if (previous > STDERR_FILENO) close(previous);
    if (next[0] > STDERR_FILENO) close(next[0]);
    if (next[1] > STDERR_FILENO) close(next[1]);
    if (command->inputFile) redirectFile(command->inputFile, 0, pgmName);
    if (command->outputFile) redirectFile(command->outputFile, 1, pgmName);
    execvp(command->pgmArgs[0], command->pgmArgs);
    reportSystemError(pgmName, command->pgmArgs[0]);
    _exit(EXIT_FAILURE);
}
/* CORRECTED: Run concurrently, then reap each exact PID. User errors
   and syscall failures must never abort the parent shell. */
void executeLine(const struct Pipeline *plan, const char *pgmName,
                 int *exitRequested)
{
    pid_t *children;
    int previous = -1, created = 0, i;
    assert(plan != NULL);
    assert(pgmName != NULL);
    assert(exitRequested != NULL);
    if (plan->count == 0) return;
    if (plan->commands[0].type != USER) {
        executeBuiltin(&plan->commands[0], pgmName, exitRequested);
        return;
    }
    children = calloc((size_t)plan->count, sizeof(*children));
    if (children == NULL) {
        fprintf(stderr, "%s: Cannot allocate memory\n", pgmName);
        return;
    }
    for (i = 0; i < plan->count && !quitRequested(); i++) {
        int next[2] = {-1, -1};
        pid_t pid;
        if (i + 1 < plan->count && pipe(next) == -1) {
            reportSystemError(pgmName, "pipe");
            break;
        }
        fflush(NULL);
        pid = fork();
        if (pid == -1) {
            reportSystemError(pgmName, "fork");
            if (next[0] != -1) close(next[0]);
            if (next[1] != -1) close(next[1]);
            break;
        }
        if (pid == 0)
            executeChild(&plan->commands[i], previous, next, pgmName);
        children[created++] = pid;
        if (previous != -1) close(previous);
        if (next[1] != -1) close(next[1]);
        previous = next[0];
    }
    if (previous != -1) close(previous);
    for (i = 0; i < created; i++) {
        int status;
        pid_t result;
        do {
            if (quitRequested()) {
                int j;
                for (j = i; j < created; j++)
                    (void)kill(children[j], SIGKILL);
            }
            result = waitpid(children[i], &status, 0);
        } while (result == -1 && errno == EINTR);
        if (result == -1) reportSystemError(pgmName, "waitpid");
    }
    free(children);
}

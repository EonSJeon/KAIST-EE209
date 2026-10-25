/* CORRECTED: Assignment 5, Jeon Sangeon, ish.c. Preserve the original
   singleLine -> lexLine -> synLine -> execution flow, with local
   ownership and separate modules for required source modularity. */
/* CORRECTED: Original skeleton by Bob Dondero, shell work by Jeon
   Sangeon; retrospective corrections prepared with Codex assistance. */
#include "execute.h"
#include "lexer.h"
#include "signals.h"
#include <assert.h>
#include <errno.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

/* CORRECTED: Process acLine, free every intermediate object, and
   return 1 for exit, 0 otherwise. Errors use the argv0 prefix. */
static int singleLine(const char *acLine, const char *argv0)
{
    DynArray_T oTokens;
    struct Pipeline plan = {NULL, 0};
    const char *error;
    int exitRequested = 0;
    assert(acLine != NULL);
    assert(argv0 != NULL);
    oTokens = DynArray_new(0);
    if (oTokens == NULL) {
        fprintf(stderr, "%s: Cannot allocate memory\n", argv0);
        return 0;
    }
    error = lexLine(acLine, oTokens);
    if (error == NULL) error = synLine(oTokens, &plan);
    if (error != NULL) fprintf(stderr, "%s: %s\n", argv0, error);
    else if (!quitRequested()) executeLine(&plan, argv0, &exitRequested);
    freePipeline(&plan);
    freeTokens(oTokens);
    return exitRequested;
}

/* CORRECTED: Read bounded lines from stream, echo startup lines only.
   Return 1 for exit; tolerate EOF/EINTR without reusing stale input. */
static int processStream(FILE *stream, int echo, const char *pgmName)
{
    char acLine[MAX_LINE_SIZE + 1];
    assert(stream != NULL);
    assert(pgmName != NULL);
    while (!quitRequested()) {
        size_t len;
        if (!echo) { fputs("% ", stdout); fflush(stdout); }
        errno = 0;
        if (fgets(acLine, sizeof(acLine), stream) == NULL) {
            if (ferror(stream) && errno == EINTR) {
                clearerr(stream);
                continue;
            }
            if (ferror(stream))
                fprintf(stderr, "%s: input: %s\n", pgmName,
                        strerror(errno));
            break;
        }
        len = strlen(acLine);
        if (echo) {
            printf("%% %s", acLine);
            if (len == 0 || acLine[len - 1] != '\n') putchar('\n');
            fflush(stdout);
        }
        if (len > MAX_LINE_SIZE - 1) {
            int c;
            if (acLine[len - 1] != '\n') {
                do { c = fgetc(stream); }
                while (c != '\n' && c != EOF && !quitRequested());
            }
            fprintf(stderr, "%s: Input line is too long\n", pgmName);
            continue;
        }
        if (singleLine(acLine, pgmName)) return 1;
    }
    return 0;
}

/* CORRECTED: Open HOME/.ishrc. Missing/unreadable files are allowed.
   Return an owned FILE stream or NULL; do not use the working dir. */
static FILE *openStartup(const char *pgmName)
{
    const char *directory = getenv("HOME");
    char *path;
    FILE *stream;
    assert(pgmName != NULL);
    if (directory == NULL) return NULL;
    path = malloc(strlen(directory) + sizeof("/.ishrc"));
    if (path == NULL) {
        fprintf(stderr, "%s: Cannot allocate memory\n", pgmName);
        return NULL;
    }
    sprintf(path, "%s/.ishrc", directory);
    stream = fopen(path, "r");
    free(path);
    if (stream != NULL &&
        fcntl(fileno(stream), F_SETFD, FD_CLOEXEC) == -1) {
        fprintf(stderr, "%s: fcntl: %s\n", pgmName, strerror(errno));
        fclose(stream);
        return NULL;
    }
    return stream;
}

/* CORRECTED: Interpret startup file, then stdin until EOF/exit or
   second SIGQUIT. Return success on normal termination. */
int main(int argc, char **argv)
{
    FILE *startup;
    int exitRequested = 0;
    (void)argc;
    assert(argv != NULL && argv[0] != NULL);
    if (installSignals() == -1) {
        fprintf(stderr, "%s: signal: %s\n", argv[0], strerror(errno));
        return EXIT_FAILURE;
    }
    /* CORRECTED: Prevent stdio read-ahead stealing bytes that a
       foreground child must read from the same stdin descriptor. */
    setvbuf(stdin, NULL, _IONBF, 0);
    startup = openStartup(argv[0]);
    if (startup != NULL) {
        exitRequested = processStream(startup, 1, argv[0]);
        fclose(startup);
    }
    if (!exitRequested && !quitRequested())
        processStream(stdin, 0, argv[0]);
    return EXIT_SUCCESS;
}

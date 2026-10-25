/* CORRECTED: Assignment 5, Jeon Sangeon. Validate every stage before
   forking, including builtins and redirection placement. */
#include "parser.h"
#include <assert.h>
#include <stdlib.h>
#include <string.h>

/* CORRECTED: Map pcName to the original command enum. No I/O. */
enum CommandType commandType(const char *pcName)
{
    const char *names[] = {"setenv", "unsetenv", "cd", "exit"};
    int i;
    assert(pcName != NULL);
    for (i = 0; i < 4; i++)
        if (strcmp(pcName, names[i]) == 0)
            return (enum CommandType)(i + 1);
    return USER;
}
/* CORRECTED: Free a partially or fully built plan. No I/O. */
void freePipeline(struct Pipeline *plan)
{
    int i;
    assert(plan != NULL);
    if (plan->commands != NULL)
        for (i = 0; i < plan->count; i++)
            free(plan->commands[i].pgmArgs);
    free(plan->commands);
    plan->commands = NULL;
    plan->count = 0;
}
/* CORRECTED: Fill plan with NULL-terminated argv arrays; return a
   descriptive error string or NULL, without I/O or global state. */
const char *synLine(DynArray_T oTokens, struct Pipeline *plan)
{
    int len, i, stage = 0;
    assert(oTokens != NULL);
    assert(plan != NULL);
    assert(plan->commands == NULL && plan->count == 0);
    len = DynArray_getLength(oTokens);
    if (len == 0) return NULL;
    plan->count = 1;
    for (i = 0; i < len; i++) {
        struct Token *token = DynArray_get(oTokens, i);
        if (token->eType == TOKEN_SPECIAL && token->pcValue[0] == '|')
            plan->count++;
    }
    plan->commands = calloc((size_t)plan->count, sizeof(*plan->commands));
    if (plan->commands == NULL) return "Cannot allocate memory";
    for (i = 0; i < plan->count; i++) {
        plan->commands[i].pgmArgs = calloc((size_t)len + 1,
                                          sizeof(char *));
        if (plan->commands[i].pgmArgs == NULL)
            return "Cannot allocate memory";
    }
    for (i = 0; i < len; i++) {
        struct Token *token = DynArray_get(oTokens, i);
        struct Command *command = &plan->commands[stage];
        if (token->eType == TOKEN_ORDINARY)
            command->pgmArgs[command->argc++] = token->pcValue;
        else if (token->pcValue[0] == '|') {
            if (command->argc == 0) return "Missing command name";
            stage++;
        }
        else {
            char symbol = token->pcValue[0];
            if (command->argc == 0) return "Missing command name";
            if (++i >= len) return "Redirection without file name";
            token = DynArray_get(oTokens, i);
            if (token->eType != TOKEN_ORDINARY)
                return "Redirection without file name";
            if (symbol == '<') {
                if (command->inputFile != NULL || stage != 0)
                    return "Multiple redirection of standard input";
                command->inputFile = token->pcValue;
            }
            else {
                if (command->outputFile != NULL || stage != plan->count-1)
                    return "Multiple redirection of standard output";
                command->outputFile = token->pcValue;
            }
        }
    }
    for (i = 0; i < plan->count; i++) {
        struct Command *command = &plan->commands[i];
        int argc = command->argc;
        if (argc == 0) return "Missing command name";
        command->type = commandType(command->pgmArgs[0]);
        if (command->type == USER) continue;
        if (plan->count != 1 || command->inputFile || command->outputFile)
            return "Built-in commands cannot use pipes or redirection";
        switch (command->type) {
            case SETENV:
                if (argc != 2 && argc != 3)
                    return "setenv takes one or two parameters";
                break;
            case UNSETENV:
                if (argc != 2) return "unsetenv takes one parameter";
                break;
            case CD:
                if (argc > 2) return "cd takes zero or one parameter";
                break;
            case EXIT:
                if (argc != 1) return "exit takes no parameters";
                break;
            default: assert(0);
        }
    }
    return NULL;
}

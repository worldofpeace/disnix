#ifndef __PROCREACT_TYPES_H
#define __PROCREACT_TYPES_H

typedef struct ProcReact_Type ProcReact_Type;

#include <unistd.h>
#include "procreact_pid.h"

/**
 * @brief Takes a file descriptor as an input and converts it to a given type.
 */
struct ProcReact_Type
{
    /**
     * Initializes a data structure maintaining some state
     *
     * @return Pointer to a data structure to a data structure maintaining the state
     */
    void *(*initialize) (void);

    /**
     * Reads some data from the file descriptor and updates the state accordingly
     *
     * @param type Reference to itself
     * @param state Pointer to a data structure maintaining the state
     * @param fd File descriptor to read from
     * @return The amount of bytes read or -1 in case of an error
     */
    ssize_t (*append) (ProcReact_Type *type, void *state, int fd);

    /**
     * Finalizes the state, checks for errors, clears obsolete data and returns
     * the end result
     *
     * @param state Pointer to a data structure maintaining the state
     * @param pid PID of a process
     * @param status Status option that will be set to any of the status codes
     * @return Pointer to a data structure providing the outcome
     */
    void *(*finalize) (void *state, pid_t pid, ProcReact_Status *status);

    /** Memorizes the delimiter for the string array type */
    char delimiter;
};

/**
 * @brief Tracks the state of a byte stream
 */
typedef struct
{
    /** Contains the read bytes so far */
    void *data;
    /** Contains the size of the byte array */
    unsigned int data_size;
}
ProcReact_BytesState;

/**
 * @brief Tracks the state of a string array
 */
typedef struct
{
    /** Contains the amount of strings processed so far */
    char **result;
    /** Contains the length of the string array */
    unsigned int result_length;
}
ProcReact_StringArrayState;

void *procreact_type_initialize_bytes(void);

ssize_t procreact_type_append_bytes(ProcReact_Type *type, void *state, int fd);

void *procreact_type_finalize_bytes(void *state, pid_t pid, ProcReact_Status *status);

void *procreact_type_finalize_string(void *state, pid_t pid, ProcReact_Status *status);

void *procreact_type_initialize_string_array(void);

ssize_t procreact_type_append_strings_to_array(ProcReact_Type *type, void *state, int fd);

void *procreact_type_finalize_string_array(void *state, pid_t pid, ProcReact_Status *status);

/**
 * Creates a type struct configured for a byte array
 *
 * @return A type struct
 */
ProcReact_Type procreact_create_bytes_type(void);

/**
 * Creates a type struct configured for a NUL-terminated string.
 *
 * @return A type struct
 */
ProcReact_Type procreact_create_string_type(void);

/**
 * Creates a type struct configured for a string array
 *
 * @return A type struct
 */
ProcReact_Type procreact_create_string_array_type(char delimiter);

/**
 * Frees a string array from memory
 *
 * @param arr String array to free
 */
void procreact_free_string_array(char **arr);

#endif

#ifndef TYPES_H
#define TYPES_H

/* User defined types */
typedef unsigned int uint;

/* Status enumeration for function return values */
typedef enum
{
    e_success,      // Indicates encoding success
    e_failure       // Indicates encoding failure
} Status; // Define an enumeration for status codes

/* Enumeration for operation types */
typedef enum
{
    e_encode,       // Indicates encoding operation
    e_decode,       // Indicates decoding operation
    e_unsupported   // Indicates unsupported operation
} OperationType; // Define an enumeration for operation types


#endif

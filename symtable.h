/* 	symtable.h
	
	Papapanagiotakis Phivos, csd3823, hy255 ask3
*/
#include <stdlib.h>
#include <string.h>
#include <assert.h>

typedef struct SymTable_S *SymTable_T;

/* Creates empty SymTable_T

Returns: SymTable_T */
SymTable_T SymTable_new(void);


/* Frees all memory allocated by the argument

Arg: SymTable_T*/
void SymTable_free(SymTable_T oSymTable);


/* Returns amount of elements currently contained

Arg: SymTable_T
Returns: amount of elements of SymTable_T 
Rutime Error: oSymTable = NULL */
unsigned int SymTable_getLength(SymTable_T oSymTable);


/* Adds element to oSymTable with key = pcKey and value = pvValue

Args: SymTable_T, const pointer to string, const void pointer
Returns: 1 if element with pcKey doesnt exist, else 0 
Rutime Error: oSymTable,pcKey = NULL */
int SymTable_put(SymTable_T oSymTable, const long long int pcKey, const long long int pvValue);


/* Removes and frees the element with pcKey

Args: SymTable_T, const pointer to string
Returns: 1 if an element was removed, else 0 
Rutime Error: oSymTable,pcKey = NULL */
int SymTable_remove(SymTable_T oSymTable, const long long int pcKey);


/* Checks if an element with key = pcKey is contained 

Args: SymTable_T, const pointer to string
Returns: 1 if the element is contained, else 0 
Rutime Error: oSymTable,pcKey = NULL */
int SymTable_contains(SymTable_T oSymTable, const long long int pcKey);


/* Returns the value of the element with key = pcKey

Args: SymTable_T, const pointer to string, 
Returns: void pointer to pvValue of element OR NULL if not contained 
Rutime Error: oSymTable,pcKey = NULL */
long long int SymTable_get(SymTable_T oSymTable, const long long int pcKey);
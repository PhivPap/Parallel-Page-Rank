/* 	symtablehash.c
	An edited Symbol Table from hy255.
	Papapanagiotakis Phivos, csd3823, hy255 ask3
*/
#include "symtable.h"
#define SIZE 10509

typedef long long int int64;

struct SymTable_S{
	struct node *Table[SIZE];
	unsigned int numOfItems;
};

struct node{
	int64 key;
	int64 value;
	struct node *next;
};

/*	HASH FUNCTION: */
static unsigned int SymTable_hash(const int64 pcKey){
	return (pcKey % SIZE);
}


/* Creates empty SymTable_T

Returns: SymTable_T */
SymTable_T SymTable_new(void){
	int i;
	struct SymTable_S *ptr = malloc(sizeof(struct SymTable_S));
	for(i=0; i<SIZE; i++){	/* set all element of hash array to NULL */
		ptr->Table[i] = NULL;
	}
	ptr->numOfItems = 0; 
	return ptr;
}


/* Frees all memory allocated by the argument

Arg: SymTable_T*/
void SymTable_free(SymTable_T oSymTable){
	int i;
	struct node *ptr, *prevPtr;
	if(!oSymTable)
		return;
	for(i=0; i<SIZE; i++){	/* Loop all element of the array */
		if(oSymTable->Table[i]){
			ptr = oSymTable->Table[i];
			prevPtr = ptr;
			while(ptr){		/* Loop all elements of list */
				ptr = ptr->next;
				free(prevPtr);	/* Free element */
				prevPtr = ptr;
			}
		}
	}
	free(oSymTable);
}


/* Returns amount of elements currently contained

Arg: SymTable_T
Returns: amount of elements of SymTable_T */
unsigned int SymTable_getLength(SymTable_T oSymTable){
	assert(oSymTable);
	return (oSymTable->numOfItems);
}


/* Adds element to oSymTable with key = pcKey and value = pvValue

Args: SymTable_T, const pointer to string, const void pointer
Returns: 1 if element with pcKey doesnt exist, else 0 */
int SymTable_put(SymTable_T oSymTable, const int64 pcKey, const int64 pvValue){
	struct node *ptr;
	char *tempString;
	assert(oSymTable);
	if(SymTable_contains(oSymTable, pcKey))
		return 0;
	ptr = malloc(sizeof(struct node));
	ptr->key = pcKey;
	ptr->value = pvValue;
	ptr->next = oSymTable->Table[SymTable_hash(pcKey)];
	oSymTable->Table[SymTable_hash(pcKey)] = ptr;
	oSymTable->numOfItems++;
	return 1;
}


/* Removes and frees the element with pcKey

Args: SymTable_T, const pointer to string
Returns: 1 if an element was removed, else 0 */
int SymTable_remove(SymTable_T oSymTable, const int64 pcKey){
	struct node *ptr, *prevPtr;
	assert(oSymTable);
	ptr = oSymTable->Table[SymTable_hash(pcKey)];
	if(!ptr)
		return 0;
	if(pcKey == ptr->key){
		oSymTable->Table[SymTable_hash(pcKey)] = ptr->next;
		free(ptr);
		oSymTable->numOfItems--;
		return 1;
	}
	prevPtr = ptr;
	ptr = ptr->next;
	while(ptr){
		if(pcKey == ptr->key){
			prevPtr->next = ptr->next;
			free(ptr);
			oSymTable->numOfItems--;
			return 1;
		}
		prevPtr = ptr;
		ptr = ptr->next;
	}
	return 0;
}


/* Checks if an element with key = pcKey is contained 

Args: SymTable_T, const pointer to string
Returns: 1 if the element is contained, else 0 */
int SymTable_contains(SymTable_T oSymTable, const int64 pcKey){
	struct node *ptr;
	assert(oSymTable);
	ptr = oSymTable->Table[SymTable_hash(pcKey)];
	while(ptr){
		if(pcKey == ptr->key)
			return 1;
		ptr = ptr->next;
	}
	return 0;
}


/* Returns the value of the element with key = pcKey

Args: SymTable_T, const pointer to string, 
Returns: void pointer to pvValue of element OR NULL if not contained */
int64 SymTable_get(SymTable_T oSymTable, const int64 pcKey){
	struct node *ptr;
	assert(oSymTable);
	ptr = oSymTable->Table[SymTable_hash(pcKey)];
	while(ptr){
		if(pcKey == ptr->key)
			return ptr->value;
		ptr = ptr->next;
	}
	return -1;
}
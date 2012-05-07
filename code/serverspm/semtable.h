/* Thomas Miller thlmille */
/* stringtable.h - stringtable header file */

#if !defined(_STRINGTABLE_H_INCLUDE_)
#define _STRINGTABLE_H_INCLUDE_

#include <string.h>
#include <stdlib.h>
#include <assert.h>
#include <stdio.h>
#include <inttypes.h>

typedef struct sem sem;

typedef char * cstring;

typedef uint32_t hashcode_t;

struct semnode {
  sem *the_sem;
  cstring str;
  hashcode_t hash;
};

typedef struct semtable * semtable_ref;

typedef struct semnode * semnode_ref;

struct semtable {
  int size; /* size of hashtable array */
  int filled; /* number of entries in hashtable */
  semnode_ref *hashtable;
};

semtable_ref new_semtable (void);

void delete_semtable (semtable_ref);

void debugdump_semtable (semtable_ref, FILE*);

semnode_ref intern_semtable (semtable_ref, cstring, sem*);

cstring peek_semtable (semnode_ref);

hashcode_t hashcode_semtable (semnode_ref);

#endif

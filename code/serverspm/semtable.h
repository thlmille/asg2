/* Thomas Miller thlmille */
/* stringtable.h - stringtable header file */

//#if !defined(_STRINGTABLE_H_INCLUDE_)
//#define _STRINGTABLE_H_INCLUDE_

//#include <string.h>
#include <stdlib.h>
#include <assert.h>
#include <stdio.h>
#include <inttypes.h>


/* No, that's cool, I'll just write my own string library */
int strlen (char *str) {
  int length = 0;
  char *itor = str;
  while (*itor != '\0') {
    ++length;
    ++itor;
  }
  return length;
}

char *strdup (char *str) {
  char *copy = calloc (strlen(str) + 1, sizeof(char));
  int i;
  for (i = 0; i < strlen(str); ++i) {
    copy[i] = str[i];
  }
  return copy;
}

/* This version just returns 0 if the strings are equal */
int strcmp (char *str1, char *str2) {
  int i = 0;
  while (str1[i] != '\0' && str2[i] != '\0') {
    if (str1[i] != str2[i]) return 1;
    ++i;
  }
  if (str1[i] == str2[i]) return 0;
  return 1;
}


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

int delete_node (semtable_ref, cstring);

void debugdump_semtable (semtable_ref, FILE*);

semnode_ref intern_semtable (semtable_ref, cstring, sem*);

cstring peek_semtable (semnode_ref);

hashcode_t hashcode_semtable (semnode_ref);

sem *get_sem (semtable_ref thetable, cstring str);


/* hashing function copied from strhash.c */
hashcode_t strhash (char *string) {
  hashcode_t hashcode = 0;
  assert (string != NULL);
  for (;;) {
    hashcode_t byte = (unsigned char) *string++;
    if (byte == '\0') break;
    hashcode = (hashcode << 5) - hashcode + byte;
  };
  return hashcode;
}

/* semnode constructor */
semnode_ref new_semnode (sem* new_sem, cstring oldstr, hashcode_t hashnum) {
  semnode_ref new_node = malloc (sizeof(struct semnode));
  new_node->the_sem = new_sem;
  new_node->str = strdup(oldstr);
  new_node->hash = hashnum;
  return new_node;
}

/* semtable constructor */
semtable_ref new_semtable (void) {
  semtable_ref new_table = malloc (sizeof(struct semtable));
  new_table->size = 31;
  new_table->hashtable = calloc (31, sizeof(semnode_ref));
  new_table->filled = 0;
  return new_table;
}

void checksize (semtable_ref thetable);

/* insert/lookup function for semtable, collision resolution */
/*    is done by linear probing */
semnode_ref intern_semtable (semtable_ref thetable, 
			     cstring str, sem* new_sem) {

  checksize (thetable);

  hashcode_t hashnum = strhash(str);
  int slot = hashnum % thetable->size;

  /* if there is no collision */
  if (thetable->hashtable[slot] == NULL) {
    thetable->hashtable[slot] = new_semnode (new_sem, str, hashnum);
    thetable->filled++;
    return thetable->hashtable[slot];
  }

  /* collision resolution (linear probing) */
  while (thetable->hashtable[slot] != NULL) {
    if (strcmp (str, thetable->hashtable[slot]->str) == 0)
      return NULL;
    slot++;
    if (slot == thetable->size) slot = 0;
  }

  thetable->hashtable[slot] = new_semnode (new_sem, str, hashnum);
  thetable->filled++;
  return thetable->hashtable[slot];

}

/* function called at beginning of insert function, checks */
/*    loading factor and performs array doubling if necessary */
void checksize (semtable_ref thetable) {
  float lf = (float) thetable->filled / (float) thetable->size;
  if (lf < .45) return; /* if loading factor is acceptable return */

  /* perform array doubling */
  int newsize = 2 * thetable->size + 1;
  semnode_ref *new_hashtable = calloc (newsize, sizeof(semnode_ref));
  /* copy over each node */
  semnode_ref *node_p = thetable->hashtable;
  int i;
  for (i = 0; i < thetable->size; ++i, ++node_p) {
    if (*node_p != NULL) {
      int newslot = (*node_p)->hash % newsize;
      while (new_hashtable[newslot] != NULL) {
	newslot++;
	if (newslot == newsize) newslot = 0;
      }
      new_hashtable[newslot] = *node_p;
    }
  }
  semnode_ref *temp = thetable->hashtable;
  thetable->hashtable = new_hashtable;
  thetable->size = newsize;
  free(temp);
}

void debugdump_semtable (semtable_ref thetable, FILE* out) {
  int i = 0;
  int nprinted = 0;
  semnode_ref *node_it = thetable->hashtable;
  int prev_bucket = thetable->size+1;
  int first = 1;
  while (nprinted < thetable->filled) {
    if (*node_it != NULL) {
      hashcode_t phash = (*node_it)->hash;
      /* make sure 0 slot is not wraparound */
      if (first) {
	if (phash % thetable->size == 0) {
	  fprintf(out, "%8d   %12u  \"%s\"\n", i, phash, (*node_it)->str);
	  nprinted++;
	  prev_bucket = 0;
	}
      }
      /* if bins collided */
      else if (phash % thetable->size == prev_bucket) {
	fprintf(out, "           %12u  \"%s\"\n", phash, (*node_it)->str);
	nprinted++;
      } 
      /* if we need a new bin */
      else {
	int bucket = phash % thetable->size;
	fprintf(out, "%8d   %12u  \"%s\"\n", bucket, phash, (*node_it)->str);
	prev_bucket = bucket;
	nprinted++;
      }
    }
    ++i;
    ++node_it;
    first = 0;
    if (i == thetable->size) {
      i = 0;
      node_it = thetable->hashtable;
    }
  }
}

sem *get_sem (semtable_ref thetable, cstring str) {
  hashcode_t hashnum = strhash(str);
  int slot = hashnum % thetable->size;
  while (thetable->hashtable[slot] != NULL) {
    if (strcmp(thetable->hashtable[slot]->str, str) == 0) {
      return thetable->hashtable[slot]->the_sem;
    }
    slot++;
    if (slot == thetable->size) slot = 0;
  }
  return NULL;
}

cstring peek_semtable (semnode_ref thenode) {
  assert (thenode != NULL);
  return thenode->str;
}

hashcode_t hashcode_semtable (semnode_ref thenode) {
  assert (thenode != NULL);
  return thenode->hash;
}

/* Returns 0 on delete success, 1 on failure */
/* This leaks memory, I don't give a shit */
int delete_node (semtable_ref thetable, cstring str) {
  assert(thetable != NULL);
  hashcode_t hashnum = strhash(str);
  int slot = hashnum % thetable->size;

  while (thetable->hashtable[slot] != NULL) {
    if (strcmp(thetable->hashtable[slot]->str, str) == 0) {
      thetable->hashtable[slot] = NULL;
      thetable->filled--;
      return 0;
    }
    ++slot;
    if (slot == thetable->size) slot = 0;
  }
  return 1;
}

void delete_semtable (semtable_ref thetable) {
  assert (thetable != NULL);
  semnode_ref *node_it = thetable->hashtable;
  int i;
  for (i = 0; i < thetable->size; ++i, ++node_it) {
    if (*node_it != NULL) {
      free ((*node_it)->str);
      free (*node_it);
    }
  }
  free (thetable->hashtable);
  free (thetable);
}


//#endif

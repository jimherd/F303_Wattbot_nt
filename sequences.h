/**
 * set of ROM based ubasic light sequences
 *
 * @author  Jim Herd
 */

#ifndef _SEQUENCES_H
#define _SEQUENCES_H

//****************************************************************************
// Inbuilt light sequences
//
static char *seq0 = {"setlight 1 5 1;\
setlight 1 7 0;\
for i=5 to 7 { \
   j = i - 4;\
   setlight 1 i j;\
   delay 2000;\
} ;\
"
};
//****************************************************************************
//
static char *seq1 = {"\
for i=0 to 255 { \
  setcolourRGB 4 i 0 0; \
  setlight 1 6 \'olive\'; \
  delay 100;\
} ;\
"
};

//****************************************************************************
// List of pointers to inbuilt ubasic sequences
//
static char * seq_list[] = {
    seq0,
    seq1,
};

#endif
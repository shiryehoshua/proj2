/*
  spot: Utilities for UChicago CMSC 23700 Intro to Computer Graphics
  Copyright (C) 2012  University of Chicago

  Permission is hereby granted, free of charge, to any person
  obtaining a copy of this software, to deal in the software without
  restriction, including without limitation the rights to use, copy,
  modify, merge, publish, distribute, sublicense, and/or sell copies
  of the software, and to permit persons to whom the software is
  furnished to do so, subject to the following condition: the above
  copyright notice and this permission notice shall be included in all
  copies or substantial portions of the software.
*/

/*
  NOTE: this assumes that the components of 3x3 matrices are indexed as:
  
  m[0]  m[3]   m[6]
  m[1]  m[4]   m[7]
  m[2]  m[5]   m[8]
  
  and 4x4 matrices as:
  
  m[ 0]   m[ 4]   m[ 8]   m[12]
  m[ 1]   m[ 5]   m[ 9]   m[13]
  m[ 2]   m[ 6]   m[10]   m[14]
  m[ 3]   m[ 7]   m[11]   m[15]
  
  which happens to conform to OpenGL conventions
*/

#ifndef SPOT_MACROS_HAS_BEEN_INCLUDED
#define SPOT_MACROS_HAS_BEEN_INCLUDED

#ifdef __cplusplus
extern "C" {
#endif

/* use this to "warning: unused parameter" warnings */
#define SPOT_UNUSED(x) (void)(x)

/* M_PI is supposed to be defined in C, not all compilers comply */
#ifndef M_PI
#define M_PI 3.14159265358979323846264338327
#endif

/* Note: all of these macros put multiple assignment statements into
   compound expressions, delimited by ",", and enclosed in parens.
   This is just to make the macro invocation syntactically similar to a
   function call, terminated with the usual ";" */

/* v = (x,y,z) */
#define SPOT_V3_SET(v, x, y, z)                 \
  ((v)[0] = (x), (v)[1] = (y), (v)[2] = (z))

/* dot(v1,v2) */
#define SPOT_V3_DOT(v1, v2) \
  ((v1)[0]*(v2)[0] + (v1)[1]*(v2)[1] + (v1)[2]*(v2)[2])

#define SPOT_V3_LEN(v) sqrt(SPOT_V3_DOT(v,v))

/* v2 = v1;   v1, v2: 3-vector */
#define SPOT_V3_COPY(v2, v1)                    \
  ((v2)[0] = (v1)[0],                           \
   (v2)[1] = (v1)[1],                           \
   (v2)[2] = (v1)[2])

/* v3 = v1 x v2;   v1, v2, v3: 3-vector */
#define SPOT_V3_CROSS(v3, v1, v2)               \
  ((v3)[0] = (v1)[1]*(v2)[2] - (v1)[2]*(v2)[1], \
   (v3)[1] = (v1)[2]*(v2)[0] - (v1)[0]*(v2)[2], \
   (v3)[2] = (v1)[0]*(v2)[1] - (v1)[1]*(v2)[0])

/* v2 = a*v1;   v1, v2: 3-vector, a: scalar */
#define SPOT_V3_SCALE(v2, a, v1)                \
  ((v2)[0] = (a)*(v1)[0],                       \
   (v2)[1] = (a)*(v1)[1],                       \
   (v2)[2] = (a)*(v1)[2])

/* v2 = v1/|v1|; len = |v1|;   v1, v2: 3-vector, len: scalar tmp variable */
#define SPOT_V3_NORM(v2, v1, len) \
  ((len) = SPOT_V3_LEN(v1), SPOT_V3_SCALE(v2, 1.0f/len, v1))
  
/* v3 = v1 - v2;   v1, v2, v3: 3-vector */
#define SPOT_V3_SUB(v3, v1, v2)                 \
  ((v3)[0] = (v1)[0] - (v2)[0],                 \
   (v3)[1] = (v1)[1] - (v2)[1],                 \
   (v3)[2] = (v1)[2] - (v2)[2])

/* v3 = v1 + v2;   v1, v2, v3: 3-vector */
#define SPOT_V3_ADD(v3, v1, v2)                 \
  ((v3)[0] = (v1)[0] + (v2)[0],                 \
   (v3)[1] = (v1)[1] + (v2)[1],                 \
   (v3)[2] = (v1)[2] + (v2)[2])

/* v2 += s*v1;   v1, v2: 3-vector, a: scalar  */
#define SPOT_V3_SCALE_INCR(v2, a, v1)           \
  ((v2)[0] += (a)*(v1)[0],                      \
   (v2)[1] += (a)*(v1)[1],                      \
   (v2)[2] += (a)*(v1)[2])





/* v2 = m * v1;   v1, v2: 3-vector, m: 3x3 matrix */
#define SPOT_M3V3_MUL(v2, m, v1)                               \
  ((v2)[0] = (m)[0]*(v1)[0] + (m)[3]*(v1)[1] + (m)[6]*(v1)[2], \
   (v2)[1] = (m)[1]*(v1)[0] + (m)[4]*(v1)[1] + (m)[7]*(v1)[2], \
   (v2)[2] = (m)[2]*(v1)[0] + (m)[5]*(v1)[1] + (m)[8]*(v1)[2])

/* m = identity;   m: 3x3 matrix */
#define SPOT_M3_IDENTITY(m)                     \
  ((m)[0] = 1.0f, (m)[3] = 0.0f, (m)[6] = 0.0f, \
   (m)[1] = 0.0f, (m)[4] = 1.0f, (m)[7] = 0.0f, \
   (m)[2] = 0.0f, (m)[5] = 0.0f, (m)[8] = 1.0f)

/* m2 = transpose(m1);   m1, m2: 3x3 matrices */
#define SPOT_M3_TRANSPOSE(m2, m1)                                  \
  ((m2)[0] = (m1)[0], (m2)[3] = (m1)[1], (m2)[6] = (m1)[2],        \
   (m2)[1] = (m1)[3], (m2)[4] = (m1)[4], (m2)[7] = (m1)[5],        \
   (m2)[2] = (m1)[6], (m2)[5] = (m1)[7], (m2)[8] = (m1)[8])

/* n = l * m;   n, l, m: 3x3 matrices */
#define SPOT_M3_MUL(n, l, m)                               \
  ((n)[0] = (l)[0]*(m)[0] + (l)[3]*(m)[1] + (l)[6]*(m)[2], \
   (n)[1] = (l)[1]*(m)[0] + (l)[4]*(m)[1] + (l)[7]*(m)[2], \
   (n)[2] = (l)[2]*(m)[0] + (l)[5]*(m)[1] + (l)[8]*(m)[2], \
                                                           \
   (n)[3] = (l)[0]*(m)[3] + (l)[3]*(m)[4] + (l)[6]*(m)[5], \
   (n)[4] = (l)[1]*(m)[3] + (l)[4]*(m)[4] + (l)[7]*(m)[5], \
   (n)[5] = (l)[2]*(m)[3] + (l)[5]*(m)[4] + (l)[8]*(m)[5], \
                                                           \
   (n)[6] = (l)[0]*(m)[6] + (l)[3]*(m)[7] + (l)[6]*(m)[8], \
   (n)[7] = (l)[1]*(m)[6] + (l)[4]*(m)[7] + (l)[7]*(m)[8], \
   (n)[8] = (l)[2]*(m)[6] + (l)[5]*(m)[7] + (l)[8]*(m)[8])

/* helper function for determinants */
#define _SPOT_M2_DET(m,a,b,c,d) \
  ((m)[(a)]*(m)[(d)] - (m)[(c)]*(m)[(b)])

/* helper function for determinants */
#define _SPOT_M3_DET(m,a,b,c,d,e,f,g,h,i)         \
  (  (m)[(a)]*(m)[(e)]*(m)[(i)] \
   + (m)[(d)]*(m)[(h)]*(m)[(c)] \
   + (m)[(g)]*(m)[(b)]*(m)[(f)] \
   - (m)[(g)]*(m)[(e)]*(m)[(c)] \
   - (m)[(a)]*(m)[(h)]*(m)[(f)] \
   - (m)[(d)]*(m)[(b)]*(m)[(i)])

/* det(m);   m: 3x3 matrix */
#define SPOT_M3_DET(m) _SPOT_M3_DET(m,0,1,2,3,4,5,6,7,8)

/* m2 = inverse(m1);  m1, m2: 3x3 matrices, det: scalar tmp variable */
#define SPOT_M3_INVERSE(m2, m1, det)          \
  ((det) = SPOT_M3_DET(m1),                   \
   (m2)[0] =  _SPOT_M2_DET(m1,4,5,7,8)/(det), \
   (m2)[1] = -_SPOT_M2_DET(m1,1,2,7,8)/(det), \
   (m2)[2] =  _SPOT_M2_DET(m1,1,2,4,5)/(det), \
   (m2)[3] = -_SPOT_M2_DET(m1,3,5,6,8)/(det), \
   (m2)[4] =  _SPOT_M2_DET(m1,0,2,6,8)/(det), \
   (m2)[5] = -_SPOT_M2_DET(m1,0,2,3,5)/(det), \
   (m2)[6] =  _SPOT_M2_DET(m1,3,4,6,7)/(det), \
   (m2)[7] = -_SPOT_M2_DET(m1,0,1,6,7)/(det), \
   (m2)[8] =  _SPOT_M2_DET(m1,0,1,3,4)/(det))




/* v = (x,y,z,w) */
#define SPOT_V4_SET(v, x, y, z, w)                \
  ((v)[0] = (x), (v)[1] = (y), (v)[2] = (z), (v)[3] = (w))

/*  v2 = v1;   v1, v2: 4-vector */
#define SPOT_V4_COPY(v2, v1)                    \
  ((v2)[0] = (v1)[0],                           \
   (v2)[1] = (v1)[1],                           \
   (v2)[2] = (v1)[2],                           \
   (v2)[3] = (v1)[3])



/* sets all entries of 4x4 matrix M, with arguments ordered
   to help visually match the normal layout of matrix components */
#define SPOT_M4_SET(M,                                 \
                    m00, m04, m08, m12,                \
                    m01, m05, m09, m13,                \
                    m02, m06, m10, m14,                \
                    m03, m07, m11, m15)                \
  (M[ 0] = m00, M[ 4] = m04, M[ 8] = m08, M[12] = m12, \
   M[ 1] = m01, M[ 5] = m05, M[ 9] = m09, M[13] = m13, \
   M[ 2] = m02, M[ 6] = m06, M[10] = m10, M[14] = m14, \
   M[ 3] = m03, M[ 7] = m07, M[11] = m11, M[15] = m15)

/* sets column 0 of m with v;   m: 4x4 matrix; v: 4-vector */
#define SPOT_M4_COL0_SET(m, v)                  \
  ((m)[ 0] = (v)[0],                            \
   (m)[ 1] = (v)[1],                            \
   (m)[ 2] = (v)[2],                            \
   (m)[ 3] = (v)[3])

/* sets column 1 of m with v;   m: 4x4 matrix; v: 4-vector */
#define SPOT_M4_COL1_SET(m, v)                  \
  ((m)[ 4] = (v)[0],                            \
   (m)[ 5] = (v)[1],                            \
   (m)[ 6] = (v)[2],                            \
   (m)[7] = (v)[3])

/* sets column 2 of m with v;   m: 4x4 matrix; v: 4-vector */
#define SPOT_M4_COL2_SET(m, v)                  \
  ((m)[ 8] = (v)[0],                            \
   (m)[ 9] = (v)[1],                            \
   (m)[10] = (v)[2],                            \
   (m)[11] = (v)[3])

/* sets column 3 of m with v;   m: 4x4 matrix; v: 4-vector */
#define SPOT_M4_COL3_SET(m, v)                  \
  ((m)[12] = (v)[0],                            \
   (m)[13] = (v)[1],                            \
   (m)[14] = (v)[2],                            \
   (m)[15] = (v)[3])

/* sets row 0 of m with v;   m: 4x4 matrix; v: 4-vector */
#define SPOT_M4_ROW0_SET(m, v)                  \
  ((m)[ 0] = (v)[0],                            \
   (m)[ 4] = (v)[1],                            \
   (m)[ 8] = (v)[2],                            \
   (m)[12] = (v)[3])

/* sets row 1 of m with v;   m: 4x4 matrix; v: 4-vector */
#define SPOT_M4_ROW1_SET(m, v)                  \
  ((m)[ 1] = (v)[0],                            \
   (m)[ 5] = (v)[1],                            \
   (m)[ 9] = (v)[2],                            \
   (m)[13] = (v)[3])

/* sets row 2 of m with v;   m: 4x4 matrix; v: 4-vector */
#define SPOT_M4_ROW2_SET(m, v)                  \
  ((m)[ 2] = (v)[0],                            \
   (m)[ 6] = (v)[1],                            \
   (m)[10] = (v)[2],                            \
   (m)[14] = (v)[3])

/* sets row 3 of m with v;   m: 4x4 matrix; v: 4-vector */
#define SPOT_M4_ROW3_SET(m, v)                  \
  ((m)[ 3] = (v)[0],                            \
   (m)[ 7] = (v)[1],                            \
   (m)[11] = (v)[2],                            \
   (m)[15] = (v)[3])

/* m = identity;   m: 4x4 matrix */
#define SPOT_M4_IDENTITY(m)                                        \
  ((m)[ 0] = 1.0f, (m)[ 4] = 0.0f, (m)[ 8] = 0.0f, (m)[12] = 0.0f, \
   (m)[ 1] = 0.0f, (m)[ 5] = 1.0f, (m)[ 9] = 0.0f, (m)[13] = 0.0f, \
   (m)[ 2] = 0.0f, (m)[ 6] = 0.0f, (m)[10] = 1.0f, (m)[14] = 0.0f, \
   (m)[ 3] = 0.0f, (m)[ 7] = 0.0f, (m)[11] = 0.0f, (m)[15] = 1.0f)

/* v2 = m * v1;   v1, v2: 3-vector; m: 4x4 matrix */
#define SPOT_M4V3_MUL(v2, m, v1)                                  \
  ((v2)[0] = (m)[ 0]*(v1)[0] + (m)[ 4]*(v1)[1] + (m)[ 8]*(v1)[2], \
   (v2)[1] = (m)[ 1]*(v1)[0] + (m)[ 5]*(v1)[1] + (m)[ 9]*(v1)[2], \
   (v2)[2] = (m)[ 2]*(v1)[0] + (m)[ 6]*(v1)[1] + (m)[10]*(v1)[2])

/* v2 = m * v1;   v1, v2: 4-vector; m: 4x4 matrix */
#define SPOT_M4V4_MUL(v2, m, v1)                                  \
  ((v2)[0] = (m)[ 0]*(v1)[0] + (m)[ 4]*(v1)[1] + (m)[ 8]*(v1)[2] + (m)[12]*(v1)[3], \
   (v2)[1] = (m)[ 1]*(v1)[0] + (m)[ 5]*(v1)[1] + (m)[ 9]*(v1)[2] + (m)[13]*(v1)[3], \
   (v2)[2] = (m)[ 2]*(v1)[0] + (m)[ 6]*(v1)[1] + (m)[10]*(v1)[2] + (m)[14]*(v1)[3], \
   (v2)[3] = (m)[ 3]*(v1)[0] + (m)[ 7]*(v1)[1] + (m)[11]*(v1)[2] + (m)[15]*(v1)[3])

/* n = l * m;    n, l, m: 4x4 matrix */
#define SPOT_M4_MUL(n, l, m)                                                \
  ((n)[ 0] = (l)[ 0]*(m)[ 0] + (l)[ 4]*(m)[ 1] + (l)[ 8]*(m)[ 2] + (l)[12]*(m)[ 3], \
   (n)[ 4] = (l)[ 0]*(m)[ 4] + (l)[ 4]*(m)[ 5] + (l)[ 8]*(m)[ 6] + (l)[12]*(m)[ 7], \
   (n)[ 8] = (l)[ 0]*(m)[ 8] + (l)[ 4]*(m)[ 9] + (l)[ 8]*(m)[10] + (l)[12]*(m)[11], \
   (n)[12] = (l)[ 0]*(m)[12] + (l)[ 4]*(m)[13] + (l)[ 8]*(m)[14] + (l)[12]*(m)[15], \
                                                                            \
   (n)[ 1] = (l)[ 1]*(m)[ 0] + (l)[ 5]*(m)[ 1] + (l)[ 9]*(m)[ 2] + (l)[13]*(m)[ 3], \
   (n)[ 5] = (l)[ 1]*(m)[ 4] + (l)[ 5]*(m)[ 5] + (l)[ 9]*(m)[ 6] + (l)[13]*(m)[ 7], \
   (n)[ 9] = (l)[ 1]*(m)[ 8] + (l)[ 5]*(m)[ 9] + (l)[ 9]*(m)[10] + (l)[13]*(m)[11], \
   (n)[13] = (l)[ 1]*(m)[12] + (l)[ 5]*(m)[13] + (l)[ 9]*(m)[14] + (l)[13]*(m)[15], \
                                                                            \
   (n)[ 2] = (l)[ 2]*(m)[ 0] + (l)[ 6]*(m)[ 1] + (l)[10]*(m)[ 2] + (l)[14]*(m)[ 3], \
   (n)[ 6] = (l)[ 2]*(m)[ 4] + (l)[ 6]*(m)[ 5] + (l)[10]*(m)[ 6] + (l)[14]*(m)[ 7], \
   (n)[10] = (l)[ 2]*(m)[ 8] + (l)[ 6]*(m)[ 9] + (l)[10]*(m)[10] + (l)[14]*(m)[11], \
   (n)[14] = (l)[ 2]*(m)[12] + (l)[ 6]*(m)[13] + (l)[10]*(m)[14] + (l)[14]*(m)[15], \
                                                                            \
   (n)[ 3] = (l)[ 3]*(m)[ 0] + (l)[ 7]*(m)[ 1] + (l)[11]*(m)[ 2] + (l)[15]*(m)[ 3], \
   (n)[ 7] = (l)[ 3]*(m)[ 4] + (l)[ 7]*(m)[ 5] + (l)[11]*(m)[ 6] + (l)[15]*(m)[ 7], \
   (n)[11] = (l)[ 3]*(m)[ 8] + (l)[ 7]*(m)[ 9] + (l)[11]*(m)[10] + (l)[15]*(m)[11], \
   (n)[15] = (l)[ 3]*(m)[12] + (l)[ 7]*(m)[13] + (l)[11]*(m)[14] + (l)[15]*(m)[15])


/* m2 = upper 3x3 submatrix of 4x4 m1 */
#define SPOT_M3M4_EXTRACT(m2, m1)                               \
  ((m2)[0] = (m1)[ 0], (m2)[3] = (m1)[ 4], (m2)[6] = (m1)[ 8],  \
   (m2)[1] = (m1)[ 1], (m2)[4] = (m1)[ 5], (m2)[7] = (m1)[ 9],  \
   (m2)[2] = (m1)[ 2], (m2)[5] = (m1)[ 6], (m2)[8] = (m1)[10])
  
/* 
** Creates a rotation matrix for rotation around axis v.
** NOTE: v MUST BE UNIT_LENGTH
** The amount of rotation theta is given in terms of (presumably precomputed)
** c = cos(theta)
** s = sin(theta)
** from "Matrices and transformations." Ronald Goldman, in
** "Graphics Gems I", pp 472-475. Morgan Kaufmann, 1990
**
** m: 4x4 matrix, v: 3-vector; c, s: scalar 
*/
#define SPOT_M4_ROTATE_AROUND(m, v, c, s)                               \
  ((m)[ 0] = (1-(c))*(v)[0]*(v)[0] +    (c)    , (m)[ 4] = (1-(c))*(v)[0]*(v)[1] - (s)*(v)[2], (m)[ 8] = (1-(c))*(v)[0]*(v)[2] + (s)*(v)[1], (m)[12] = 0.0f, \
   (m)[ 1] = (1-(c))*(v)[0]*(v)[1] + (s)*(v)[2], (m)[ 5] = (1-(c))*(v)[1]*(v)[1] +    (c)    , (m)[ 9] = (1-(c))*(v)[1]*(v)[2] - (s)*(v)[0], (m)[13] = 0.0f, \
   (m)[ 2] = (1-(c))*(v)[0]*(v)[2] - (s)*(v)[1], (m)[ 6] = (1-(c))*(v)[1]*(v)[2] + (s)*(v)[0], (m)[10] = (1-(c))*(v)[2]*(v)[2] +    (c)    , (m)[14] = 0.0f, \
   (m)[ 3] = 0.0f                              , (m)[ 7] = 0.0f                              , (m)[11] = 0.0f                              , (m)[15] = 1.0f)

#ifdef __cplusplus
}
#endif

#endif /* SPOT_MACROS_HAS_BEEN_INCLUDED */

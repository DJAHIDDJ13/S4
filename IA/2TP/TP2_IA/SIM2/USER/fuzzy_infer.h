#ifndef FUZZY_INFER_H
#define FUZZY_INFER_H

#define MAX_NAME_LEN 64

typedef union fuzzy_set FUZZY_SET;

typedef struct trapezoidal_fuzzy_set TRAP_FUZZY_SET;
typedef struct triangular_fuzzy_set TRI_FUZZY_SET;
typedef struct rectangular_fuzzy_set RECT_FUZZY_SET;
typedef struct polygonal_fuzzy_set POLY_FUZZY_SET;
typedef struct fuzzy_set_list FUZZY_SET_LIST;

typedef struct fuzzy_rule FUZZY_RULE;

/*********************************************
 *             inference system              *
 *********************************************/

typedef struct fuzzy_logic_system {
   IN_LING_VAR x, y;
   OUT_LING_VAR z;
   FUZZY_SET a, b, c;
   
   FUZZY_RULE* rules;
} FUZZY_SYS;

/*********************************************
 *             Linguistic variable           *
 *********************************************/

/*
 * min_x and max_x define the range of the linguistic variables
 */
typedef struct input_linguistic_variable {
   char var_name[MAX_NAME_LEN]; // variable name
   int min_x;                   // start of the x range
   int max_x;                   // end of the x range

   int input;

   FUZZY_SET_LIST* fuzzy_sets;  // list of fuzzy sets
} IN_LING_VAR;


typedef struct output_linguistic_variable {
   char var_name[MAX_NAME_LEN]; // variable name
   int min_x;                   // start of the x range
   int max_x;                   // end of the x range

   FUZZY_SET_LIST* fuzzy_sets;  // list of fuzzy sets
} OUT_LING_VAR;

IN_LING_VAR createInputVar(const char* var_name, int min_x, int max_x, int input);
void addFuzzySet(IN_LING_VAR* ling, FUZZY_SET set);
void freeInputVar(IN_LING_VAR* ling);
float getInputMembership(IN_LING_VAR in, const char* set_name);

/*********************************************
 *                 Fuzzy sets                *
 *********************************************/

typedef enum fuzzy_set_type {TRAPEZOID, TRIANGLE, RECTANGLE, POLYGON} FUZZY_SET_TYPE;

/*
 * Trapezoid fuzzy set
 * h |      _______
 *   |     /       \
 *   |    /         \
 * 0 |___/___________\____
 *      x1 x2     x3  x4
 */
struct trapezoidal_fuzzy_set {
   FUZZY_SET_TYPE type;

   char set_name[MAX_NAME_LEN]; // the set's linguistic name eg: 'hot', 'far' etc..

   int x1;
   int x2;
   int x3;
   int x4;

   int height; // h
};

/* 
 * Triangle fuzzy set
 * h |      
 *   |     /\
 *   |    /  \
 * 0 |___/____\____
 *      x1 x2 x3
 */
struct triangular_fuzzy_set {
   FUZZY_SET_TYPE type;

   char set_name[MAX_NAME_LEN]; // the set's linguistic name eg: 'hot', 'far' etc..

   int x1;
   int x2;
   int x3;
   
   int height;
};

/*
 * Rectangular fuzzy set
 * h |   ______   
 *   |  |      |
 *   |  |      | 
 * 0 |__|______|__
 *     x1      x2 
 */
struct rectangular_fuzzy_set {
   FUZZY_SET_TYPE type;

   char set_name[MAX_NAME_LEN]; // the set's linguistic name eg: 'hot', 'far' etc..

   int x1;
   int x2;
   
   int height;
};


/*
 * List of polygon points
 */
typedef struct polygon_list {
   int x, y;

   struct poly_list* next;
} POLY_LIST;

/*
 * Polygonal fuzzy set
 *
 *y3 |     __ 
 *y2 |    /  \    ___
 *y1 |   /    \__/   \    ...
 * 0 |__/_____________\_
 *     x1 x2 x3 x4 x5  x6 ... 
 * points = [x1, y2]->[x2,y2]-> ...
 */
struct polygonal_fuzzy_set {
   FUZZY_SET_TYPE type;

   char set_name[MAX_NAME_LEN]; // the set's linguistic name eg: 'hot', 'far' etc..

   POLY_LIST points;
};

union fuzzy_set {
   TRAP_FUZZY_SET trapezoid;
   TRI_FUZZY_SET triangle;
   RECT_FUZZY_SET rectangle;
   POLY_FUZZY_SET polygon;
};

/*
 * Linked list of fuzzy sets
 */
struct fuzzy_set_list {
   FUZZY_SET current;
   FUZZY_SET_LIST* next;
};

FUZZY_SET_LIST* pushFuzzySet(FUZZY_SET_LIST* list, FUZZY_SET set);
FUZZY_SET createTrapezoidSet(const char* name, int x1, int x2, int x3, int x4, int height);
FUZZY_SET createTriangleSet(const char* name, int x1, int x2, int x3, int height);
FUZZY_SET createRectangleSet(const char* name, int x1, int x2, int height);

/**********************************************
 *                 Fuzzy rules                *
 **********************************************/

/*
 * Mamdani fuzzy rule of this form:
 * IF    x IS A    AND    y IS B    THEN   z IS C
 * where x, y, z are linguistic variables and
 * A, B, C are fuzzy_sets
 */
typedef struct fuzzy_rule {
   IN_LING_VAR  x, y; // linguistic variables of input and output
   OUT_LING_VAR z;
   FUZZY_SET A, B, C; // fuzzy sets of rule definition

   FUZZY_RULE* next; // next fuzzy rule
} FUZZY_RULE;



/* Utility functions */
float getLinearIntersection(int x1, int y1, int x2, int y2, int x);
float getTrapezoidArea(int a, int b, int c, int d, int height);
float getTrapezoidMembership(TRAP_FUZZY_SET set, int in);
float getSetMembership(FUZZY_SET set, int input);
#endif

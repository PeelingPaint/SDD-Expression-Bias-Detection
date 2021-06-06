#include <stdio.h>
#include <stdlib.h>
#include "sddapi.h"

typedef struct variable_list_t {
  SddLiteral** variables;
  int size;
} variable_list_t;

// sdd_imply taken from SDD Beginning-User Manual Version 2.0
SddNode* sdd_imply(SddNode* node1, SddNode* node2, SddManager* manager);

// sdd_equiv taken from SDD Beginning-User Manual Version 2.0
SddNode* sdd_equiv(SddNode* node1, SddNode* node2, SddManager* manager);

// returns 1 if var is relevant to the sdd rooted at root. Returns 0 otherwise.
int sdd_variable_is_relevant(SddNode* root, SddManager* manager,
                             SddLiteral var);

// Returns a variable_list_t struct containing a list of pointers to all
// variables in the input variables list which are relevant to the SDD rooted at
// root, and the size of that list. It is the responsibility of the caller to
// free the returned struct. variables should be a list of pointers to the
// SddLiterals which we wish to test for relevance. n_variables should be the
// number of variables in variables.
variable_list_t sdd_find_relevant_variables(SddNode* root,
                                                    SddManager* manager,
                                                    SddLiteral** variables,
                                                    int n_variables);

// Returns a variable_list_t struct containing a list of pointers to all
// variables in the input variables list which are irrelevant to the SDD rooted
// at root, and the size of that list. It is the responsibility of the caller to
// free the returned struct. manager  should be a pointer to root's manager.
// variables should be a list of pointers to the SddLiterals which we wish to
// test for relevance. n_variables should be the number of variables in
// variables.
variable_list_t sdd_find_irrelevant_variables(SddNode* root,
                                                    SddManager* manager,
                                                    SddLiteral** variables,
                                                    int n_variables);

// Returns 1 if the decision made by classifier about alpha is biased on the
// basis of varphi. Returns 0 otherwise. classifier is an sdd of the classifier.
// varphi is an sdd of the protected expression, alpha is an array of
// assignments representing an instance classifier_variables,
// classifier_variables is a list of pointers to all variables mentioned in
// classifier, n_classifier_variables is the size of classifier_variables.
int biased_on_varphi(SddNode* classifier, SddNode* varphi, SddLiteral* alpha,
                     SddManager* manager, SddLiteral** classifier_variables,
                     int n_classifier_variables);

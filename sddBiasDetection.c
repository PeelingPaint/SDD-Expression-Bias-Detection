#include <stdio.h>
#include <stdlib.h>
#include "sddapi.h"
#include "sddBiasDetection.h"

// sdd_imply taken from SDD Beginning-User Manual Version 2.0
SddNode* sdd_imply(SddNode* node1, SddNode* node2, SddManager* manager) {
  return sdd_disjoin(sdd_negate(node1,manager),node2,manager);
}

// sdd_equiv taken from SDD Beginning-User Manual Version 2.0
SddNode* sdd_equiv(SddNode* node1, SddNode* node2, SddManager* manager) {
  return sdd_conjoin(sdd_imply(node1,node2,manager),
                     sdd_imply(node2,node1,manager),manager);
}

int sdd_variable_is_relevant(SddNode* root, SddManager* manager,
                             SddLiteral var) {
  SddNode* cond_true = sdd_condition(var, root, manager);
  SddNode* cond_false = sdd_condition(-var, root, manager);
  SddNode* equiv = sdd_equiv(cond_true, cond_false, manager);
  return !sdd_node_is_true(equiv);
}

variable_list_t sdd_find_relevant_variables(SddNode* root,
                                                    SddManager* manager,
                                                    SddLiteral** variables,
                                                    int n_variables) {
  variable_list_t varset;
  // Initially allocate enough space to store all variables if they are
  // relevant. Realloc will later be used to reduce size if necessary.
  varset.variables = (SddLiteral**)malloc(n_variables * sizeof(SddNode*));
  varset.size = 0;
  for (int i = 0; i < n_variables; i++) {
    if (sdd_variable_is_relevant(root, manager, *variables[i])) {
      varset.variables[varset.size] = variables[i];
      varset.size++;
    }
  }
  varset.variables =  (SddLiteral**)realloc(varset.variables,
                                            varset.size * sizeof(SddNode*));
  return varset;
}

variable_list_t sdd_find_irrelevant_variables(SddNode* root,
                                                    SddManager* manager,
                                                    SddLiteral** variables,
                                                    int n_variables) {
  variable_list_t varset;
  // Initially allocate enough space to store all variables if they are
  // relevant. Realloc will later be used to reduce size if necessary.
  varset.variables = (SddLiteral**)malloc(n_variables * sizeof(SddNode*));
  varset.size = 0;
  for (int i = 0; i < n_variables; i++) {
    if (!sdd_variable_is_relevant(root, manager, *variables[i])) {
      varset.variables[varset.size] = variables[i];
      varset.size++;
    }
  }
  varset.variables =  (SddLiteral**)realloc(varset.variables,
                                            varset.size * sizeof(SddNode*));
  return varset;
}

int biased_on_varphi(SddNode* classifier, SddNode* varphi, SddLiteral* alpha,
                     SddManager* manager, SddLiteral** classifier_variables,
                     int n_classifier_variables) {
  variable_list_t classifier_rel_set;
  classifier_rel_set = sdd_find_relevant_variables(classifier, manager,
                                                   classifier_variables,
                                                   n_classifier_variables);
  variable_list_t fixed = sdd_find_irrelevant_variables(varphi, manager,
                                                        classifier_rel_set.variables,
                                                        classifier_rel_set.size);
  SddNode* conditioned_classifier = sdd_copy(classifier, manager);
  for (int i = 0; i < fixed.size; i++) {
    // x - 1 will be the index of variable x in alpha, since variable numbers
    // start at 1 and indices start at 0.
    conditioned_classifier = sdd_condition(alpha[*fixed.variables[i] - 1],
                                           conditioned_classifier,
                                           manager);
  }
  free(classifier_rel_set.variables);
  free(fixed.variables);
  SddNode* equiv = sdd_equiv(conditioned_classifier, varphi, manager);
  if (sdd_node_is_true(equiv)) {
    return 1;
  }
  SddNode* anti_equiv = sdd_equiv(conditioned_classifier,
                                 sdd_negate(varphi, manager), manager);
  if (sdd_node_is_true(anti_equiv)) {
    return 1;
  }
  return 0;
}

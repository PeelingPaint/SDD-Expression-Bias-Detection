// This file includes some tests we performed on our implementation.

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include "sddapi.h"
#include "sddBiasDetection.h"

int test_known_biased() {
  // initialize manager
  SddLiteral var_count = 4;
  int auto_gc_and_minimize = 0;
  SddManager* manager = sdd_manager_create(var_count,auto_gc_and_minimize);
  SddLiteral A = 1, B = 2, C = 3, D = 4;
  int n_vars = 4;
  SddLiteral* variables[4] = {&A, &B, &C, &D};

  // (A && C) || (B && D)
  SddNode* classifier;
  SddNode* c1 = sdd_conjoin(sdd_manager_literal(A, manager),
                            sdd_manager_literal(C, manager), manager);
  SddNode* c2 = sdd_conjoin(sdd_manager_literal(B, manager),
                            sdd_manager_literal(D, manager), manager);
  classifier = sdd_disjoin(c1, c2, manager);
  // (A || B)
  SddNode* varphi = sdd_disjoin(sdd_manager_literal(A, manager),
                                sdd_manager_literal(B, manager), manager);
  SddLiteral alpha[4] = {A, -B, C, D};
  int is_biased = biased_on_varphi(classifier, varphi, alpha, manager,
                                   variables, n_vars);
  sdd_manager_free(manager);
  // We exprect bias, so if bias is returned, the result is correct.
  return (is_biased);
}

int test_known_unbiased() {
  // initialize manager
  SddLiteral var_count = 3;
  int auto_gc_and_minimize = 0;
  SddManager* manager = sdd_manager_create(var_count,auto_gc_and_minimize);
  SddLiteral A = 1, B = 2, C = 3;
  int n_vars = 3;
  SddLiteral* variables[3] = {&A, &B, &C};

  // (A && C) || (B && D)
  SddNode* classifier;
  SddNode* c1 = sdd_conjoin(sdd_manager_literal(A, manager),
                            sdd_manager_literal(C, manager), manager);
  SddNode* c2 = sdd_conjoin(sdd_manager_literal(-A, manager),
                            sdd_manager_literal(-C, manager), manager);
  classifier = sdd_disjoin(c1, c2, manager);
  // (A || B)
  SddNode* varphi = sdd_disjoin(sdd_manager_literal(A, manager),
                                sdd_manager_literal(B, manager), manager);
  SddLiteral alpha[3] = {A, -B, C};
  int is_biased = biased_on_varphi(classifier, varphi, alpha, manager,
                                   variables, n_vars);
  sdd_manager_free(manager);
  // We exprect a lack of bias, so if bias is returned, the result is incorrect.
  return (1 - is_biased);
}

int test_double_implication_known_biased() {
  // initialize manager
  SddLiteral var_count = 4;
  int auto_gc_and_minimize = 0;
  SddManager* manager = sdd_manager_create(var_count,auto_gc_and_minimize);
  SddLiteral A = 1, B = 2, C = 3, D = 4;
  int n_vars = 4;
  SddLiteral* variables[4] = {&A, &B, &C, &D};

  // (!C && !D) || (!A && B) || (B && C && D)
  SddNode* classifier;
  SddNode* c1 = sdd_conjoin(sdd_manager_literal(-C, manager),
                            sdd_manager_literal(-D, manager), manager);
  SddNode* c2 = sdd_conjoin(sdd_manager_literal(-A, manager),
                            sdd_manager_literal(B, manager), manager);
  SddNode* c3 = sdd_conjoin(sdd_manager_literal(B, manager),
                            sdd_manager_literal(C, manager), manager);
  SddNode* c4 = sdd_conjoin(c3, sdd_manager_literal(D, manager), manager);
  classifier = sdd_disjoin(c1, c2, manager);
  classifier = sdd_disjoin(classifier, c4, manager);
  // (C <==> D)
  SddNode* varphi = sdd_equiv(sdd_manager_literal(C, manager),
                                sdd_manager_literal(D, manager), manager);
  SddLiteral alpha[4] = {A, B, C, -D};
  int is_biased = biased_on_varphi(classifier, varphi, alpha, manager,
                                   variables, n_vars);
  sdd_manager_free(manager);
  // We exprect bias, so if bias is returned, the result is correct.
  return (is_biased);
}

int test_double_implication_known_unbiased() {
  // initialize manager
  SddLiteral var_count = 4;
  int auto_gc_and_minimize = 0;
  SddManager* manager = sdd_manager_create(var_count,auto_gc_and_minimize);
  SddLiteral A = 1, B = 2, C = 3, D = 4;
  int n_vars = 4;
  SddLiteral* variables[4] = {&A, &B, &C, &D};

  // (!C && !D) || (!A && B) || (B && C && D)
  SddNode* classifier;
  SddNode* c1 = sdd_conjoin(sdd_manager_literal(-C, manager),
                            sdd_manager_literal(-D, manager), manager);
  SddNode* c2 = sdd_conjoin(sdd_manager_literal(-A, manager),
                            sdd_manager_literal(B, manager), manager);
  SddNode* c3 = sdd_conjoin(sdd_manager_literal(B, manager),
                            sdd_manager_literal(C, manager), manager);
  SddNode* c4 = sdd_conjoin(c3, sdd_manager_literal(D, manager), manager);
  classifier = sdd_disjoin(c1, c2, manager);
  classifier = sdd_disjoin(classifier, c4, manager);
  // (C <==> D)
  SddNode* varphi = sdd_equiv(sdd_manager_literal(C, manager),
                                sdd_manager_literal(D, manager), manager);
  SddLiteral alpha[4] = {A, -B, -C, D};
  int is_biased = biased_on_varphi(classifier, varphi, alpha, manager,
                                   variables, n_vars);
  sdd_manager_free(manager);
  // We exprect a lack of bias, so if bias is returned, the result is incorrect.
  return (1 - is_biased);
}

double test_proportion_biased(SddNode* classifier, SddNode* varphi,
                           SddManager* manager, SddLiteral** variables,
                           int n_variables, SddLiteral instances[][n_variables]) {
  int n_biased = 0;
  int n_instances = pow(2, n_variables);
  for (int i = 0; i < n_instances; i++) {
    if (biased_on_varphi(classifier, varphi, instances[i], manager, variables,
                         n_variables)) {
      printf("Instance %d was biased\n", i);
      n_biased++;
    }
  }
  return (double)n_biased / (double)n_instances;
}

int test_bias_proportion_no_opportunity() {
  // initialize manager
  SddLiteral var_count = 4;
  int auto_gc_and_minimize = 0;
  SddManager* manager = sdd_manager_create(var_count,auto_gc_and_minimize);
  SddLiteral A = 1, B = 2, C = 3, D = 4;
  int n_vars = 4;
  SddLiteral* variables[4] = {&A, &B, &C, &D};

  // ((A && C) || (B && D)) && (!C || !D)
  SddNode* classifier;
  SddNode* c1 = sdd_conjoin(sdd_manager_literal(A, manager),
                            sdd_manager_literal(C, manager), manager);
  SddNode* c2 = sdd_conjoin(sdd_manager_literal(B, manager),
                            sdd_manager_literal(D, manager), manager);
  SddNode* c3 = sdd_disjoin(sdd_manager_literal(-C, manager),
                            sdd_manager_literal(-D, manager), manager);
  classifier = sdd_disjoin(c1, c2, manager);
  classifier = sdd_conjoin(classifier, c3, manager);
  // (A || B)
  SddNode* varphi = sdd_disjoin(sdd_manager_literal(A, manager),
                                sdd_manager_literal(B, manager), manager);
  SddLiteral instances[16][4] = {
    {A, B, C, D},
    {A, B, C, -D},
    {A, B, -C, D},
    {A, B, -C, -D},
    {A, -B, C, D},
    {A, -B, C, -D},
    {A, -B, -C, D},
    {A, -B, -C, -D},
    {-A, B, C, D},
    {-A, B, C, -D},
    {-A, B, -C, D},
    {-A, B, -C, -D},
    {-A, -B, C, D},
    {-A, -B, C, -D},
    {-A, -B, -C, D},
    {-A, -B, -C, -D}
  };
  // In this set of instances, 12/16 satisfy varphi.
  // Of the instances that saitsfy varphi, 4 satisfy the classifier, giving a
  // positive rate of 33%
  // Of the instances that do not satirsfy varphi, none satisfy the classifier,
  // giving a positive rate of 0%.
  // No instances are counterfactually biased on the basis of varphi.
  double prop = test_proportion_biased(classifier, varphi, manager, variables,
                                       n_vars, instances);
  printf("%f\n", prop);
  return 0;
}

int main(int argc, char** argv) {
  if (!test_known_biased()) {
    printf("Known bias test failed\n");
  }
  if (!test_known_unbiased()) {
    printf("Known lack of bias test failed\n");
  }
  if (!test_double_implication_known_biased()) {
    printf("Double implication known bias test failed\n");
  }
  if (!test_double_implication_known_unbiased()) {
    printf("Double implication known lack of bias test failed\n");
  }

  test_bias_proportion_no_opportunity();

  return 0;
}

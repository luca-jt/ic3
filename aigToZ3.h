#pragma once
#include "z3++.h"
#include <optional>

struct SymbolicTransitionSystem {
  z3::expr_vector variables;
  z3::expr_vector primedVariables;
  z3::expr transitions;
  z3::expr init;
  z3::expr bad;
};

typedef SymbolicTransitionSystem TS;
typedef z3::expr_vector Cube;

inline z3::context context;

// returns expr where all variables are primed
z3::expr prime(const TS &ts, z3::expr expr);
// returns conjunction of expressions in cube
z3::expr expr(const Cube &cube);
// returns disjunction of expressions in cubes
z3::expr expr(const std::vector<Cube> &cubes);

z3::expr frameExpr(const std::vector<z3::expr> &exprs);

// returns model (as cube of literals) if satisfiable and nullopt if unsatisfiable
std::optional<z3::expr> SAT(const z3::expr &expr);
// returns SymbolicTransitionSystem for aig model defined in give file
SymbolicTransitionSystem parse(const std::string &filepath);

// returns true if an invariant is given for ts, can be used for verifying invariants
bool isInvariant(const TS &ts, const z3::expr &invariant);

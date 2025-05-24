#include "aigToZ3.h"

#include <iostream>
#include <sstream>
#include <string>
#include <optional>

extern "C" {
#include "aiger/aiger.h"
}

z3::expr prime(const TS &ts, z3::expr expr) {
  return expr.substitute(ts.variables, ts.primedVariables);
}

z3::expr expr(const Cube &cube) { return z3::mk_and(cube); }

z3::expr expr(const std::vector<Cube> &cubes) {
  z3::expr_vector cubesExprs(context);
  for (const auto &cube : cubes) {
    cubesExprs.push_back(expr(cube));
  }
  return z3::mk_or(cubesExprs);
}

z3::expr frameExpr(const std::vector<z3::expr> &exprs) {
  z3::expr_vector frameExprs(context);
  for (const auto &e : exprs) {
    frameExprs.push_back(e);
  }
  return z3::mk_or(frameExprs);
}

// returns cube that corresponds to model restricted to unprimed + stateful variables
z3::expr getExpr(const z3::model &model) {
  Cube cube(context);
  for (auto i = 0; i < model.size(); i++) {
    assert(model[i].arity() == 0);  // only constants/nullary predicates
    if (model[i].name().str().back() == '\'') { continue; } // do not include primed variables
    if(model[i].name().str()[0] == 'a') { continue; } // stateful vars are only inputs + latches
    auto variable = context.bool_const(model[i].name().str().c_str());
    auto literal = model.get_const_interp(model[i]).is_true() ? variable : !variable;
    cube.push_back(literal);
  }
  return expr(cube);
}

std::optional<z3::expr> SAT(const z3::expr &expr) {
  z3::solver z3(context);
  z3.add(expr);
  switch (z3.check()) {
    case z3::unsat:   return std::nullopt;
    case z3::sat:     return getExpr(z3.get_model());
    default: exit(0);
  }
}

// Parsing
// creates a named variable
z3::expr var(const aiger_symbol *symbols, const size_t i, const char prefix, const bool prime = false) {
  std::stringstream ss;
  if (symbols != nullptr && symbols[i].name) {
    ss << symbols[i].name;
  } else {
    ss << prefix << i;
  }
  if (prime) { ss << "'"; }
  return context.bool_const(ss.str().c_str());
}

// returns lit from integer, i.e. 3 is ~x1
z3::expr lit(const z3::expr_vector &vars, const unsigned int l) {
  if (l < 2) {
    return l == 0 ? z3::mk_or(context) : z3::mk_and(context);
  }
  return aiger_sign(l) ? !vars[l >> 1] : vars[l >> 1];
}

// transforms an aiger object into a SymbolicTransitionSystem
SymbolicTransitionSystem transitionSystemFromAiger(const aiger *aig) {
  if (aig->num_outputs != 1) {
    std::cout << "Only safety properties encoded as a single output are supported."<< std::endl;
    exit(0);
  }
  z3::expr_vector vars(context); // false, [inputs], [latches], [ands]
  z3::expr_vector primedVars(context);
  // Dummy var for false
  vars.push_back(context.bool_const("false"));
  primedVars.push_back(context.bool_const("false'"));
  // Vars for INPUTS
  for (size_t i = 0; i < aig->num_inputs; ++i) {
    vars.push_back(var(aig->inputs, i, 'i'));
    primedVars.push_back(var(aig->inputs, i, 'i', true));
  }
  // Vars for LATCHES
  for (size_t i = 0; i < aig->num_latches; ++i) {
    vars.push_back(var(aig->latches, i, 'l'));
    primedVars.push_back(var(aig->latches, i, 'l', true));
  }
  // Vars for ANDS
  for (size_t i = 0; i < aig->num_ands; ++i) {
    vars.push_back(var(nullptr, aig->ands[i].lhs, 'a'));
    primedVars.push_back(var(nullptr, aig->ands[i].lhs, 'a', true));
  }

  // Parse INIT
  Cube init(context);
  for (size_t i = 0; i < aig->num_latches; ++i) {
    const auto &latch = vars[1 + aig->num_inputs + i];

    init.push_back(!latch); // latches are initalized with 0
  }

  // Parse BAD
  // currently only support output as bad states
  // include ANDS in bad for CTI check
  Cube bad(context);
  for (size_t i = 0; i < aig->num_outputs; ++i) {
    bad.push_back(lit(vars, aig->outputs[i].lit));
  }
  for (size_t i = 0; i < aig->num_ands; ++i) { // ANDS
    const auto &lhs = vars[1 + aig->num_inputs + aig->num_latches + i];
    const auto rhs0 = lit(vars, aig->ands[i].rhs0);
    const auto rhs1 = lit(vars, aig->ands[i].rhs1);
    bad.push_back(lhs == (rhs0 && rhs1));
  }

  // Parse TRANSITIONS
  Cube transitions(context);
  for (size_t i = 0; i < aig->num_ands; ++i) { // ANDS
    const auto &lhs = vars[1 + aig->num_inputs + aig->num_latches + i];
    const auto rhs0 = lit(vars, aig->ands[i].rhs0);
    const auto rhs1 = lit(vars, aig->ands[i].rhs1);
    transitions.push_back(lhs == (rhs0 && rhs1));
    // transitions.push_back((lhs == (rhs0 && rhs1)).substitute(vars, primedVars));
  }
  for (size_t i = 0; i < aig->num_latches; ++i) { // LATCHES
    const auto &primedLatch = primedVars[1 + aig->num_inputs + i];

    transitions.push_back(primedLatch == lit(vars, aig->latches[i].next));
  }

  return {vars, primedVars, expr(transitions), expr(init), expr(bad)};
}

SymbolicTransitionSystem parse(const std::string &filepath) {
  // read AIGER file
  aiger *aig = aiger_init();
  const char *msg = aiger_open_and_read_from_file(aig, filepath.c_str());
  if (msg) {
    std::cout << msg << std::endl;
    exit(0);
  }
  // create the transition system from the obtained aiger model
  auto ts = transitionSystemFromAiger(aig);
#if(DEBUG)
  std::cout << "Init: " << ts.init << std::endl;
  std::cout << "Bad: " << ts.bad << std::endl;
  std::cout << "Transitions: " << ts.transitions << std::endl;
#endif
  aiger_reset(aig);
  return ts;
}

bool isInvariant(const TS &ts, const z3::expr &invariant) {
  return !SAT(ts.init && !invariant) && !SAT(invariant && ts.transitions && !prime(ts, invariant));
}

#pragma once
#include "aigToZ3.h"
#include <optional>
#include <initializer_list>

// IC3 algorithm

//typedef std::vector<Cube> Frame;

class BadFrame {
  z3::expr_vector frame;

public:
  template<typename ...Args>
  BadFrame(Args&&... exprs)
  : frame(z3::expr_vector(context))
  {
    static_assert((std::is_same_v<z3::expr, std::decay_t<Args>> && ...));

    (frame.push_back(std::forward<Args>(exprs)), ...);
  }

  z3::expr to_expr() const {
    return z3::mk_or(frame);
  }

  void add_cti(z3::expr cti) {
    frame.push_back(std::move(cti));
  }
};

// returns true if SAFE and false if UNSAFE
bool ic3(const TS &ts);

// returns true if cube is an initial state
bool isInitial(const TS &ts, const z3::expr &cube);

// returns cube in bad & frame or nullopt if no such cube exists
std::optional<z3::expr> getBadInFrame(const TS &ts, const z3::expr &frame);

// returns true if counterexample (a path to init) is found
// returns false otherwise + strengthens frames to exclude path
bool searchPathToInit(const TS &ts, std::vector<z3::expr> &frames, z3::expr &cti);

// strengthen frames by blocking cube at frame
void blockCubeAtFrame(z3::expr &frame, const z3::expr &cube);

#pragma once
#include "aigToZ3.h"
#include <optional>
#include <initializer_list>

// IC3 algorithm

//typedef std::vector<Cube> Frame;

struct BadFrame {
  z3::expr_vector _frame;

  BadFrame(const z3::expr &expr)
  : _frame(z3::expr_vector(context))
  {
    _frame.push_back(expr);
  }

  z3::expr to_expr() const {
    return z3::mk_or(_frame);
  }

  void add_cti(const BadFrame &cti) {
    for (const auto &expr : cti._frame) {
      _frame.push_back(expr);
    }
  }

  void add_bad(const z3::expr &expr) {
    _frame.push_back(expr);
  }
};

struct Frame {
  z3::expr_vector _frame;

  Frame(const z3::expr &expr)
  : _frame(z3::expr_vector(context))
  {
    _frame.push_back(expr);
  }

  Frame(const BadFrame &bad_frame)
  : _frame(z3::expr_vector(context))
  {
    for (const auto &expr : bad_frame._frame) {
      _frame.push_back(!expr);
    }
  }

  z3::expr to_expr() const {
    return z3::mk_and(_frame);
  }

  void block_cti(const BadFrame &cti) {
    for (const auto &expr : cti._frame) {
      _frame.push_back(!expr);
    }
  }
};

// returns true if SAFE and false if UNSAFE
bool ic3(const TS &ts);

// returns true if cube is an initial state
bool isInitial(const TS &ts, const BadFrame &cube);

// returns cube in bad & frame or nullopt if no such cube exists
std::optional<Cube> getBadInFrame(const Frame &frame, const z3::expr &bad_part);

// returns true if counterexample (a path to init) is found
// returns false otherwise + strengthens frames to exclude path
bool searchPathToInit(const TS &ts, std::vector<Frame> &frames, BadFrame &cti);

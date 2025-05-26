#include "ic3.h"
#include "aigToZ3.h"
#include <optional>
#include <iostream>
#include <cstdint>


bool ic3(const TS &ts) {
  auto init_break = SAT(ts.init && ts.bad);
  if (init_break) {
    std::cout << "Trivial counter example:" << std::endl;
    std::cout << expr(init_break.value()) << std::endl;
    return false;
  }

  std::vector<Frame> frames = { Frame{!ts.bad} };
  BadFrame bad_frame{ts.bad};

  uint64_t iteration = 0;
  while (1) {
    iteration++;
    std::cout << "Iteration: " << iteration << std::endl;

    auto pre = ts.transitions && prime(ts, bad_frame.to_expr());
    auto cti_cube = SAT(pre && frames.back().to_expr());

    if (!cti_cube) {
      std::cout << "Found inductive invariant:" << std::endl;
      std::cout << expr(SAT(frames.back().to_expr()).value()) << std::endl;
      return true;
    }

    BadFrame cti{expr(cti_cube.value())};

    bool is_reaching_init = searchPathToInit(ts, frames, cti);
    bad_frame.add_cti(cti);
    bad_frame = BadFrame{expr(SAT(bad_frame.to_expr()).value())};

    if (is_reaching_init) {
      std::cout << "Found CTI reaching Init starting in Bad with sequence of A's:" << std::endl;
      std::cout << bad_frame._frame << std::endl;
      return false;
    }

    frames.push_back(Frame{bad_frame});
  }
}

bool searchPathToInit(const TS &ts, std::vector<Frame> &frames, BadFrame &cti) {
  if (isInitial(ts, cti)) return true;
  auto new_bad = cti._frame[0];

  for (auto frame = frames.rbegin() + 1; frame != frames.rend(); frame++) { // da frames nie leer ist, ist das safe
    auto new_bad_opt = getBadInFrame(ts, *frame, new_bad);
    if (!new_bad_opt) break;
    new_bad = expr(new_bad_opt.value());

    cti.add_bad(new_bad);

    if (isInitial(ts, cti)) return true;
  }

  for (auto frame = frames.rbegin(); frame != frames.rend(); frame++) {
    frame->block_cti(cti);
    *frame = Frame{expr(SAT(frame->to_expr()).value())};
  }

  return false;
}

bool isInitial(const TS &ts, const BadFrame &cube) {
  z3::expr conj = ts.init && cube.to_expr();
  return SAT(conj).has_value();
}

std::optional<Cube> getBadInFrame(const TS &ts, const Frame &frame, const z3::expr &bad_part) {
  auto pre = ts.transitions && prime(ts, bad_part);
  return SAT(pre && frame.to_expr());
}

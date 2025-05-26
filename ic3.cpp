#include "ic3.h"
#include "aigToZ3.h"
#include <optional>
#include <iostream>


bool ic3(const TS &ts) {
  auto init_break = SAT(ts.init && ts.bad);
  if (init_break) {
    std::cout << "Trivial counter example:" << std::endl;
    std::cout << expr(init_break.value()) << std::endl;
    return false;
  }

  std::vector<Frame> frames = { Frame{!ts.bad} };
  BadFrame bad_frame{ts.bad};

  while (1) {
    auto pre = ts.transitions && prime(ts, bad_frame.to_expr());
    auto cti_cube = SAT(pre && frames.back().to_expr());
    if (!cti_cube) return true;
    BadFrame cti{expr(cti_cube.value())};

    if (searchPathToInit(ts, frames, cti)) {
      std::cout << "CTI reaching Init:" << std::endl;
      std::cout << cti.to_expr() << std::endl;
      return false;
    }

    bad_frame.add_cti(cti);
    frames.push_back(Frame{bad_frame});
  }
}

bool searchPathToInit(const TS &ts, std::vector<Frame> &frames, BadFrame &cti) {
  if (isInitial(ts, cti)) return true;
  auto new_bad = cti._frame[0];

  for (auto frame = frames.rbegin() + 1; frame != frames.rend(); frame++) { // da frames nie leer ist, ist das safe
    auto new_bad_opt = getBadInFrame(*frame, new_bad);
    if (!new_bad_opt) break;
    new_bad = expr(new_bad_opt.value());

    cti.add_bad(new_bad);

    if (isInitial(ts, cti)) return true;
  }

  for (auto frame = frames.rbegin(); frame != frames.rend(); frame++) {
    frame->block_cti(cti);
  }

  return false;
}

bool isInitial(const TS &ts, const BadFrame &cube) {
  z3::expr conj = ts.init && cube.to_expr();
  return SAT(conj).has_value();
}

std::optional<Cube> getBadInFrame(const Frame &frame, const z3::expr &bad_part) {
  z3::expr conj = bad_part && frame.to_expr();
  return SAT(conj);
}

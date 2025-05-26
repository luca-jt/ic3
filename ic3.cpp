#include "ic3.h"
#include "aigToZ3.h"
#include <optional>


bool ic3(const TS &ts) {
  if (SAT(ts.init && ts.bad)) return false;

  std::vector<Frame> frames = { Frame{!ts.bad} };
  BadFrame bad_frame{ts.bad};

  while (1) {
    auto pre = ts.transitions && prime(ts, bad_frame.to_expr());
    auto cti_cube = SAT(pre && frames.back().to_expr());
    if (!cti_cube) return true;
    BadFrame cti{expr(cti_cube.value())};

    if (searchPathToInit(ts, frames, cti)) return false;

    bad_frame.add_cti(cti);
    frames.push_back(Frame{bad_frame});
  }
}

bool searchPathToInit(const TS &ts, std::vector<Frame> &frames, BadFrame &cti) {
  if (isInitial(ts, cti)) return true;
  frames.back().block_cti(cti);

  for (auto frame = frames.rbegin() + 1; frame != frames.rend(); frame++) { // da frames nie leer ist, ist das safe
    auto new_bad = getBadInFrame(ts, *frame);
    if (!new_bad) break;

    cti.add_bad(expr(new_bad.value()));

    if (isInitial(ts, cti)) return true;

    frame->block_cti(cti);
  }

  return false;
}

bool isInitial(const TS &ts, const BadFrame &cube) {
  z3::expr conj = ts.init && cube.to_expr();
  return SAT(conj).has_value();
}

std::optional<Cube> getBadInFrame(const TS &ts, const Frame &frame) {
  z3::expr conj = ts.bad && frame.to_expr();
  return SAT(conj);
}

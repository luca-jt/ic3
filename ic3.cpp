#include "ic3.h"
#include "aigToZ3.h"
#include <optional>


bool ic3(const TS &ts) {
  if (SAT(ts.init && ts.bad)) return false;

  std::vector<z3::expr> frames = { !ts.bad };
  BadFrame bad_frame{ts.bad};

  while (1) {
    auto pre = ts.transitions && prime(ts, bad_frame.to_expr()); // müssen wir hier noch die ganzen geschachtelten pre-Berechnungen machen?
    auto cti = SAT(pre && frames.back());
    if (!cti) return true;
    auto cti_value = cti.value();

    if (searchPathToInit(ts, frames, cti_value)) return false;

    bad_frame.add_cti(cti_value);
    frames.push_back(!bad_frame.to_expr());
  }
}

bool searchPathToInit(const TS &ts, std::vector<z3::expr> &frames, z3::expr &cti) {
  if (isInitial(ts, cti)) return true;
  blockCubeAtFrame(frames.back(), cti);

  for (auto frame = frames.rbegin() + 1; frame != frames.rend(); frame++) { // da frames nie leer ist, ist das safe
    auto new_bad = getBadInFrame(ts, *frame);
    if (!new_bad) break;

    cti = cti && new_bad.value();

    if (isInitial(ts, cti)) return true;

    blockCubeAtFrame(*frame, cti);
  }

  return false;
}

bool isInitial(const TS &ts, const z3::expr &cube) {
  z3::expr conj = ts.init && cube;
  return SAT(conj).has_value();
}

std::optional<z3::expr> getBadInFrame(const TS &ts, const z3::expr &frame) {
  z3::expr conj = ts.bad && frame;
  return SAT(conj);
}

void blockCubeAtFrame(z3::expr &frame, const z3::expr &cube) {
  frame = frame && !cube;
}

#include "ic3.h"
#include "aigToZ3.h"
#include <optional>


bool ic3(const TS &ts) {
  if (SAT(ts.init && ts.bad)) return false;

  Cube init_cube(context);
  init_cube.push_back(!ts.bad);
  Cube bad_cube(context);
  bad_cube.push_back(ts.bad);

  std::vector<Frame> frames = { Frame{init_cube} };
  Frame bad_frame = Frame{bad_cube};

  while (1) {
    auto pre = preF(ts, bad_frame);
    if (!pre) return true;

    auto cti = SAT(pre.value() && expr(frames.back()));
    if (!cti) return true;
    auto cti_value = cti.value();

    if (searchPathToInit(ts, frames, cti_value)) return false;

    bad_frame.push_back(cti_value);
    newFrame(frames, bad_frame);
  }
}

std::optional<Cube> preF(const TS &ts, const Frame &bad_frame) {
  auto pre = SAT(ts.transitions && prime(ts, expr(bad_frame)));
  // müssen wir hier noch die ganzen geschachtelten pre-Berechnungen machen?
  return pre;
}

bool searchPathToInit(const TS &ts, std::vector<Frame> &frames, Cube &cti) {
  if (isInitial(ts, cti)) return true;
  blockCubeAtFrame(frames.back(), cti);

  for (auto frame = frames.rbegin() + 1; frame != frames.rend(); frame++) { // da frames nie leer ist, ist das safe
    auto new_bad = getBadInFrame(ts, *frame);
    if (!new_bad) break;

    cti.push_back(expr(new_bad.value()));

    if (isInitial(ts, cti)) return true;

    blockCubeAtFrame(*frame, cti);
  }

  return false;
}

bool isInitial(const TS &ts, const Cube &cube) {
  z3::expr conj = ts.init && expr(cube);
  return SAT(conj).has_value();
}

std::optional<Cube> getBadInFrame(const TS &ts, const Frame &frame) {
  z3::expr conj = ts.bad && expr(frame);
  return SAT(conj);
}

void blockCubeAtFrame(Frame &frame, const Cube &cube) {
  for (auto &c : frame) {
    c.push_back(!expr(cube));
  }
}

bool isInductive(const TS &ts, const std::vector<Frame> &frames) {
  const auto &frame = frames.back();
  return isInvariant(ts, expr(frame));
}

void newFrame(std::vector<Frame> &frames, const Frame &bad_frame) {
  Frame new_frame = frames[frames.size() - 1];
  Cube new_frame_cube(context);
  new_frame_cube.push_back(!expr(bad_frame));
  new_frame.push_back(new_frame_cube);
  frames.push_back(new_frame);
}

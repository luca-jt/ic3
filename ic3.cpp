#include "ic3.h"
#include "aigToZ3.h"
#include <optional>


bool ic3(const TS &ts) {
  if SAT(ts.init && ts.bad) return false;

  std::vector<Frame> frames = { Frame{ts.init} };
  Frame bad_frame = Frame{ts.bad};

  return true;

  while (1) {
    frames.push_back(~frameExpr(bad_frame));

    // checke Bedingungen von der aktuellen frame combi, falls ja return true
    // sonst suche das CTI
    // calle searchPathToInit() wo dann das CTI in den frames geblockt wird, falls der return hier false ist, return false
    // checke nach relative inductiveness
    // erstelle neuen frame und bad frame mit der blocking clause -> hier generalize() verwenden?
  }
}

bool isInitial(const TS &ts, const z3::expr &cube) {
  z3::expr conj = ts.init && ~cube;
  return !SAT(conj).has_value();
}

std::optional<z3::expr> getBadInFrame(const TS &ts, const Frame &frame) {
  z3::expr conj = ts.bad && frameExpr(frame);
  return SAT(conj);
}

bool searchPathToInit(const TS &ts, std::vector<Frame> &frames, z3::expr &cti) {
  // TODO: Implement me
}

std::optional<z3::expr> getPre(const TS &ts, const std::vector<Frame> &frames, const z3::expr &cube, int relative_to) {
  // TODO: Implement me
}

z3::expr generalize(const TS &ts, const std::vector<Frame> &frames, const z3::expr &cube, int frame_index) {
  Frame &frame = frames[frame_index];
  frame.push_back(cube);
}

void blockCubeAtFrame(std::vector<Frame> &frames, int frame_index, const z3::expr &cube) {
  Frame &frame = frames[frame_index];
  for (auto &expr : frame) {
    expr = expr && ~cube;
  }
}

bool isInductive(const TS &ts, const std::vector<Frame> &frames) {
  const auto &frame = *frames.crbegin();
  return isInvariant(ts, frameExpr(frame));
}

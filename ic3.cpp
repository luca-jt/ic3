#include "ic3.h"
#include "aigToZ3.h"
#include <optional>


bool ic3(const TS &ts) {
  if (SAT(ts.init && ts.bad)) return false;

  std::vector<Frame> frames = { Frame{~ts.bad} };
  Frame bad_frame = Frame{ts.bad};

  return true; // REMOVE

  while (1) {
    auto pre = ts.transitions && prime(ts, frameExpr(bad_frame));
    auto cti = SAT(pre && frameExpr(frames.back()));

    if (!cti) {
      return true;
    }

    auto cti_value = cti.value();

    if (searchPathToInit(ts, frames, cti_value)) return false;

    bad_frame.push_back(cti_value);
    frames.push_back(Frame{~frameExpr(bad_frame)});
  }
}

bool searchPathToInit(const TS &ts, std::vector<Frame> &frames, z3::expr &cti) {
  // TODO: Implement me
}

bool isInitial(const TS &ts, const z3::expr &cube) {
  z3::expr conj = ts.init && cube;
  return SAT(conj).has_value();
}

std::optional<z3::expr> getBadInFrame(const TS &ts, const Frame &frame) {
  z3::expr conj = ts.bad && frameExpr(frame);
  return SAT(conj);
}

void blockCubeAtFrame(std::vector<Frame> &frames, int frame_index, const z3::expr &cube) {
  Frame &frame = frames[frame_index];
  for (auto &expr : frame) {
    expr = expr && ~cube;
  }
}

bool isInductive(const TS &ts, const std::vector<Frame> &frames) {
  const auto &frame = frames.back();
  return isInvariant(ts, frameExpr(frame));
}

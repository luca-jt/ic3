#include "ic3.h"
#include "aigToZ3.h"
#include <optional>


bool ic3(const TS &ts) {
  Frames frames;
  newFrame(frames);
  frames[0]; //wie konvertiert man eine expr zu einem Cube???
  return true;
}

bool isInitial(const TS &ts, const Cube &cube) {
  //die Implementierung wäre nur richtig, wenn in einem Cube nur ein State wäre
  Cube cubesExprs(context);
  cubesExprs.push_back(ts.init);
  cubesExprs.push_back(expr(cube));
  if (SAT(z3::mk_and(cubesExprs)) != std::nullopt) {
    return true;
  }
  return false;
}

std::optional<Cube> getBad(const TS &ts, const Frame &frame) {
  Cube cubesExprs(context);
  cubesExprs.push_back(ts.bad);
  cubesExprs.push_back(expr(frame));
  return SAT(z3::mk_and(cubesExprs));
}

bool searchPathToInit(const TS &ts, Frames &frames, Cube &cti) {
  // TODO: Implement me
}

std::optional<Cube> getPre(const TS &ts, const Frames &frames, const Cube &cube, int relativeTo) {
  // TODO: Implement me
}

Cube generalize(const TS &ts, const Frames &frames, const Cube &cube, int frameIndex) {
  // TODO: Implement me
}

void blockCubeAtFrame(Frames &frames, int frameIndex, const Cube &cube) {
  // TODO: Implement me
}

bool isInductive(const TS &ts, const Frames &frames) {
  // TODO: Implement me
}

void newFrame(Frames &frames) {
  Frame new_frame;
  frames.push_back(new_frame);
}

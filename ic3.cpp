#include "ic3.h"
#include <optional>

bool ic3(const TS &ts) {
  // TODO: Implement me
  return true;
}

bool isInitial(const TS &ts, const Cube &cube) {
  // TODO: Implement me
}

std::optional<Cube> getBad(const TS &ts, const Frame &frame) {
  // TODO: Implement me
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
  // TODO: Implement me
}

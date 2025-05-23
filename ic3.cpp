#include "ic3.h"
#include "aigToZ3.h"
#include <optional>


bool ic3(const TS &ts) {
  Cube init_cube(context);
  init_cube.push_back(~ts.bad);
  Cube bad_cube(context);
  bad_cube.push_back(ts.bad);
  std::vector<Frame> frames = { Frame{std::move(init_cube)} };
  std::vector<Frame> badFrames = { Frame{std::move(bad_cube)} }; // dass in c++ moven manuell gemacht werden muss ist schon echt bad

  return true;

  while (1) {
    // checke Bedingungen von der aktuellen frame combi, falls ja return true
    // sonst suche das CTI
    // calle searchPathToInit() wo dann das CTI in den frames geblockt wird, falls der return hier false ist, return false (?)
    // checke nach relative inductiveness
    // erstelle neuen frame und bad frame mit der blocking clause -> hier generalize() verwenden?
  }
}

bool isInitial(const TS &ts, const Cube &cube) {
  z3::expr conj = ts.init && expr(cube);
  return SAT(conj).has_value(); // ich glaube das reicht (sollte zumindest äquivalent sein zu dem was davor da war), vielleicht kann man den SAT call vermeiden, indem man die expression nur vereinfacht und schaut ob sie false ist?
  // ich glaube das sollte so auch gehen wenn 'cube' nicht nur ein zustand ist, sondern nur init enthält
}

std::optional<Cube> getBadInFrame(const TS &ts, const Frame &frame) {
  z3::expr conj = ts.bad && expr(frame);
  return SAT(conj);
}

bool searchPathToInit(const TS &ts, std::vector<Frame> &frames, Cube &cti) {
  // TODO: Implement me
}

std::optional<Cube> getPre(const TS &ts, const std::vector<Frame> &frames, const Cube &cube, int relative_to) {
  // TODO: Implement me
}

Cube generalize(const TS &ts, const std::vector<Frame> &frames, const Cube &cube, int frame_index) {
  // TODO: Implement me
}

void blockCubeAtFrame(std::vector<Frame> &frames, int frame_index, const Cube &cube) {
  Frame &frame = frames[frame_index];
  for (auto &cube : frame) {
    cube.push_back(~expr(cube)); // hier zB sollte man das auch einfach mit z3::expr verwenden können ohne cubes
  }
}

bool isInductive(const TS &ts, const std::vector<Frame> &frames) {
  const auto &frame = *frames.crbegin();
  return isInvariant(ts, expr(frame));
}

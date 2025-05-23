#pragma once
#include "aigToZ3.h"
#include <optional>

// IC3 algorithm

typedef std::vector<Cube> Frame;

// returns true if SAFE and false if UNSAFE
bool ic3(const TS &ts);

// returns true if cube is an initial state
bool isInitial(const TS &ts, const Cube &cube);

// returns cube in bad & frame or nullopt if no such cube exists
std::optional<Cube> getBad(const TS &ts, const Frame &frame);

// returns true if counterexample (a path to init) is found
// returns false otherwise + strengthens frames to exclude path
bool searchPathToInit(const TS &ts, std::vector<Frame> &frames, Cube &cti);

// returns predecessor of cube or nullopt if no predecessor exists
// IC3 uses relative inductiveness
// restrict the set of possible predecessors by overapproximation Fi and ~cube
std::optional<Cube> getPre(const TS &ts, const std::vector<Frame> &frames, const Cube &cube, int relative_to);

// returns subcube that is still unreachable from init (disjoint with init + not reachable by previous overapprox.)
Cube generalize(const TS &ts, const std::vector<Frame> &frames, const Cube &cube, int frame_index);

// strengthen frames by blocking cube at frame
void blockCubeAtFrame(std::vector<Frame> &frames, int frame_index, const Cube &cube);

// returns true if inductive and false otherwise
bool isInductive(const TS &ts, const std::vector<Frame> &frames);

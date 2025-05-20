#pragma once
#include "aigToZ3.h"
#include <optional>

// IC3 algorithm

typedef std::vector<Cube> Frame;
typedef std::vector<Frame> Frames;

// returns true if SAFE and false if UNSAFE
bool ic3(const TS &ts);

// returns true if cube is an initial state
bool isInitial(const TS &ts, const Cube &cube);

// returns cube in bad & frame or nullopt if no such cube exists
std::optional<Cube> getBad(const TS &ts, const Frame &frame);

// returns true if counterexample (a path to init) is found
// returns false otherwise + strengthens frames to exclude path
bool searchPathToInit(const TS &ts, Frames &frames, Cube &cti);

// returns predecessor of cube or nullopt if no predecessor exists
// IC3 uses relative inductiveness
// restrict the set of possible predecessors by overapproximation Fi and ~cube
std::optional<Cube> getPre(const TS &ts, const Frames &frames, const Cube &cube, int relativeTo);

// returns subcube that is still unreachable from init (disjoint with init + not reachable by previous overapproximation)
Cube generalize(const TS &ts, const Frames &frames, const Cube &cube, int frameIndex);

// strengthen frames by blocking cube at frame
void blockCubeAtFrame(Frames &frames, int frameIndex, const Cube &cube);

// returns true if inductive and false otherwise
bool isInductive(const TS &ts, const Frames &frames);

// inserts new frame in frames
void newFrame(Frames &frames);

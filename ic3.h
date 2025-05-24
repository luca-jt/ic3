#pragma once
#include "aigToZ3.h"
#include <optional>

// IC3 algorithm

typedef std::vector<z3::expr> Frame;

// returns true if SAFE and false if UNSAFE
bool ic3(const TS &ts);

// returns true if cube is an initial state
bool isInitial(const TS &ts, const z3::expr &cube);

// returns cube in bad & frame or nullopt if no such cube exists
std::optional<z3::expr> getBad(const TS &ts, const Frame &frame);

// returns true if counterexample (a path to init) is found
// returns false otherwise + strengthens frames to exclude path
bool searchPathToInit(const TS &ts, std::vector<Frame> &frames, z3::expr &cti);

// strengthen frames by blocking cube at frame
void blockCubeAtFrame(std::vector<Frame> &frames, int frame_index, const z3::expr &cube);

// returns true if inductive and false otherwise
bool isInductive(const TS &ts, const std::vector<Frame> &frames);

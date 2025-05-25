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
std::optional<Cube> getBadInFrame(const TS &ts, const Frame &frame);

// returns true if counterexample (a path to init) is found
// returns false otherwise + strengthens frames to exclude path
bool searchPathToInit(const TS &ts, std::vector<Frame> &frames, Cube &cti);

// strengthen frames by blocking cube at frame
void blockCubeAtFrame(Frame &frame, const Cube &cube);

// returns true if inductive and false otherwise
bool isInductive(const TS &ts, const std::vector<Frame> &frames);

// calculates the pre^F
std::optional<Cube> preF(const TS &ts, const Frame &bad_frame);

// creates a new frame with the updated bad frame
void newFrame(std::vector<Frame> &frames, const Frame &bad_frame);

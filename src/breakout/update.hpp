#ifndef BREAKOUT_UPDATE_HPP
#define BREAKOUT_UPDATE_HPP

#include "../tl-application.hpp"

void InitializeGameState(int clientX, int clientY);
GameState& UpdateGameState(const tl::Input& input, float dt);
int startSound();

#endif

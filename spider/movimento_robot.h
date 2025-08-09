#ifndef MOVIMENTO_ROBOT_H
#define MOVIMENTO_ROBOT_H

void rest();
void move_forward();
void move_backward();
void turn_right();
void turn_left();
void head_down();
void attach_servos();
void head_right();
void head_left();
void head_straight();

// New functions for spider-like movement
void tripod_gait_forward();
void body_up();
void body_down();
void scared_animation();
void observe_animation();

#endif
//
// Created by ljh on 25. 9. 19..
//

#ifndef MYPROJECT_SCULPTOR_ACT_HPP
#define MYPROJECT_SCULPTOR_ACT_HPP
#include "event_manager_mode.hpp"
#include "sculptor.hpp"

struct SculptorMode : Actor{
  SculptorMode(Camera *camera, GLFWwindow *window);
  virtual void keyEvent(int key, int scancode, int action, int mods) override;
  virtual void cursorPosCallBack(float deltaX, float deltaY) override;
  virtual void getKey() override;
  virtual void getMouseEvent() override;
};

#endif //MYPROJECT_SCULPTOR_ACT_HPP
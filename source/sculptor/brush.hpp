//
// Created by ljh on 25. 9. 18..
//

#ifndef MYPROJECT_BRUSH_HPP
#define MYPROJECT_BRUSH_HPP

enum class BrushType{
  Inflate,
  Flatten,
  Smooth,
  Pinch
};

struct Brush{
  float radius;
  float strength;
  BrushType type;
};

struct BrushInflate : public Brush{};

struct BrushFlatten : public Brush{};

struct BrushFlattenSmooth : public Brush{};

struct BrushPinch : public Brush{};

struct BrushPinchSmooth : public Brush{};

#endif //MYPROJECT_BRUSH_HPP
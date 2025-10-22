#pragma once
namespace Engine {
class Color {
public:
  Color() : r(0), g(0), b(0), a(1) {}
  Color(float red, float green, float blue, float alpha = 1.0f)
      : r(red), g(green), b(blue), a(alpha) {}

  float r, g, b, a;
};
} // namespace Engine

#pragma once

namespace Engine {
struct Dirty {};
struct InUse {
  int users = 0;
  void addUser() { users++; }
  bool removeUser() {
    if (users > 0) {
      users--;
    }
    return users == 0;
  }
};
} // namespace Engine

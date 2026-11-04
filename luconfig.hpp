#pragma once
#include <sol/sol.hpp>

class WindowManager;
namespace Hero {
class LuConfig {
public:
  LuConfig(WindowManager *wm);
  void LoadConfig();

private:
  sol::state lua;
  ::WindowManager *wm_instance_;
};

} // namespace Hero

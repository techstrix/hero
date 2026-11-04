#include "luconfig.hpp"
#include "window_manager.hpp"
#include <glog/logging.h>
#include <sol/error.hpp>
namespace Hero {
LuConfig::LuConfig(::WindowManager *wm) : wm_instance_(wm) {

  lua.open_libraries(sol::lib::base, sol::lib::io, sol::lib::os);
}

void LuConfig::LoadConfig() {
  lua.set_function("set_wallpaper", [this](const std::string &image_path) {
    return wm_instance_->SetWallpaper(image_path);
  });
  sol::optional<sol::error> result = lua.script_file("./rc.lua");

  if (result) {

    sol::error error = *result;
    LOG(ERROR) << "Error in lua configuration " << error.what();
  } else {
    LOG(INFO) << "Lua configuration loaded successfully.";
  }
}

} // namespace Hero
// namespace Hero

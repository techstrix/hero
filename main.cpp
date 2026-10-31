#include "window_manager.hpp"
#include <X11/Xlib.h>
#include <cstdlib>
#include <glog/logging.h>
#include <memory>

int main(int argc, char **argv) {
  google::InitGoogleLogging(argv[0]);
  FLAGS_logtostderr = true;
  std::unique_ptr<WindowManager> window_manager(WindowManager::Create());

  if (!window_manager) {
    LOG(ERROR) << "Failed to initialize  window manager.";
    return EXIT_FAILURE;
  }
  window_manager->Run();
  return EXIT_SUCCESS;
  return 0;
}

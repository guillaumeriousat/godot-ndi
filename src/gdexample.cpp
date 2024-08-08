#include "gdexample.h"
#include <godot_cpp/core/class_db.hpp>
#include <Processing.NDI.Lib.h>

#ifdef _WIN32
#ifdef _WIN64
#pragma comment(lib, "Processing.NDI.Lib.x64.lib")
#else // _WIN64
#pragma comment(lib, "Processing.NDI.Lib.x86.lib")
#endif // _WIN64
#endif // _WIN32

using namespace godot;

void GDExample::_bind_methods() {
}

GDExample::GDExample() {
  // Initialize any variables here.
  time_passed = 0.0;
  // Not required, but "correct" (see the SDK documentation).
  if (!NDIlib_initialize())
    return 0;

  // We create the NDI sender
  ndi_sender = NDIlib_send_create();
  if (!ndi_sender)
    return 0;

}

GDExample::~GDExample() {
  NDIlib_send_destroy(ndi_sender);
  NDIlib_destroy();
}

void GDExample::_process(double delta) {

}

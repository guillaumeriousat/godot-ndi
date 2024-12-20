#include "gdndi.h"
#include <godot_cpp/variant/utility_functions.hpp>
#include <godot_cpp/core/class_db.hpp>
#include <godot_cpp/classes/viewport.hpp>
#include <godot_cpp/classes/viewport_texture.hpp>
#include <godot_cpp/classes/image.hpp>
#include <Processing.NDI.Lib.h>
#include <Processing.NDI.Send.h>
#include <Processing.NDI.DynamicLoad.h>

#ifdef _WIN32
#ifdef _WIN64
#pragma comment(lib, "Processing.NDI.Lib.x64.lib")
#else // _WIN64
#pragma comment(lib, "Processing.NDI.Lib.x86.lib")
#endif // _WIN64
#endif // _WIN32

using namespace godot;

void GDNDI::_bind_methods() {
  ClassDB::bind_method(D_METHOD("set_framerate"), &GDNDI::set_framerate);
  ClassDB::bind_method(D_METHOD("get_framerate"), &GDNDI::get_framerate);
}

GDNDI::GDNDI() {
  if (!NDIlib_initialize()) {
    return;
  }
  NDIlib_send_create_t NDI_send_create_desc;
  NDI_send_create_desc.p_ndi_name = "GODOT NDI PLZ";
  ndi_sender = NDIlib_send_create(&NDI_send_create_desc);
  if (!ndi_sender) {
    return;
  }
}

GDNDI::~GDNDI() {
  NDIlib_send_destroy(ndi_sender);
  // NDIlib_destroy();
  // NDIlib_destroy();
}

void GDNDI::_process(double delta) {
  time_since_last_frame +=delta;
  if (time_since_last_frame < frame_interval) {
    return;
  }
  // if (!NDIlib_send_get_no_connections(ndi_sender, 0)) {
  //   // don't send anything if nothing is connected to this to avoid congesting
  //   // the network.
  //   return;
  // }
  Ref<Image> img = get_viewport()->get_texture()->get_image();
  auto heigth = img->get_height();
  auto width = img->get_width();
  const unsigned char *p = (const unsigned char *) img->get_data().ptr();
  NDIlib_video_frame_v2_t NDI_video_frame;
  NDI_video_frame.xres = width;
  NDI_video_frame.yres = heigth;
  NDI_video_frame.FourCC = NDIlib_FourCC_type_BGRA;
  NDI_video_frame.p_data = (uint8_t*)malloc(NDI_video_frame.xres * NDI_video_frame.yres * 4);
  NDI_video_frame.line_stride_in_bytes = width * 4;

  time_since_last_frame -= frame_interval;
}

void GDNDI::set_framerate(double framerate) {
  this->framerate = framerate;
  this->frame_interval = 1/framerate;
}

double GDNDI::get_framerate() const {
  return this->framerate;
}

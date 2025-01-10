#include "gdndi.h"
#include <godot_cpp/variant/utility_functions.hpp>
#include <godot_cpp/core/class_db.hpp>
#include <godot_cpp/classes/viewport.hpp>
#include <godot_cpp/classes/viewport_texture.hpp>
#include <godot_cpp/classes/image.hpp>
#include <godot_cpp/classes/engine.hpp>
#include <Processing.NDI.Lib.h>
#include <Processing.NDI.Send.h>
#include <Processing.NDI.DynamicLoad.h>
#include <cstring>

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
  if (Engine::get_singleton()->is_editor_hint()) {
    return;
  }
  NDIlib_send_create_t NDI_send_create_desc;

  NDI_send_create_desc.p_ndi_name = "GODOT NDI PLZ";
  ndi_sender = NDIlib_send_create(&NDI_send_create_desc);
  if (!ndi_sender) {
    return;
  }
  // maybe RGBX since I think we're pretty much guaranteed to not have alpha
  NDI_video_frame.FourCC = NDIlib_FourCC_type_RGBA;
}

GDNDI::~GDNDI() {
  NDIlib_send_destroy(ndi_sender);
  // NDIlib_destroy();
  // NDIlib_destroy();
}

void GDNDI::_process(double delta) {
  // don't process when the call is coming from the editor side, only during gameplay.
  if (Engine::get_singleton()->is_editor_hint()) {
    return;
  }
  time_since_last_frame +=delta;
  if (time_since_last_frame < frame_interval) {
    return;
  }
  // if (!NDIlib_send_get_no_connections(ndi_sender, 1)) {
    // don't send anything if nothing is connected to this to avoid congesting
    // the network.
    // return;
  // }
  Ref<Image> img = get_viewport()->get_texture()->get_image();
  auto height = img->get_height();
  auto width = img->get_width();
  img->convert(Image::Format::FORMAT_RGBA8);
  const uint8_t* p = (const uint8_t *) img->get_data().ptr();
  NDI_video_frame.xres = width;
  NDI_video_frame.yres = height;
  // line stride is simple since we use a straightforward color format.
  NDI_video_frame.line_stride_in_bytes = width * 4;
  // copy the image in the frame
  std::memcpy(this->frames[this->frame_index].data(), p, NDI_video_frame.xres * NDI_video_frame.yres * 4);
  NDI_video_frame.p_data = this->frames[this->frame_index].data();
  // send the frame
  NDIlib_send_send_video_async_v2(ndi_sender, &NDI_video_frame);
  time_since_last_frame -= frame_interval;
  // point to the other frame buffer
  this->frame_index = !this->frame_index;
}

void GDNDI::set_framerate(double framerate) {
  this->framerate = framerate;
  this->frame_interval = 1/framerate;
}

double GDNDI::get_framerate() const {
  return this->framerate;
}

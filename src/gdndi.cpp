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
#include <godot_cpp/variant/string.hpp>

#ifdef _WIN32
#ifdef _WIN64
#pragma comment(lib, "Processing.NDI.Lib.x64.lib")
#else // _WIN64
#pragma comment(lib, "Processing.NDI.Lib.x86.lib")
#endif // _WIN64
#endif // _WIN32

using namespace godot;

void GDNDI::_bind_methods() {
  ClassDB::bind_method(D_METHOD("set_stream_name"), &GDNDI::set_stream_name);
  ClassDB::bind_method(D_METHOD("get_stream_name"), &GDNDI::get_stream_name);
	ADD_PROPERTY(PropertyInfo(Variant::STRING, "stream name"), "set_stream_name", "get_stream_name");
}

GDNDI::GDNDI() {
  if (!NDIlib_initialize()) {
    return;
  }
  if (Engine::get_singleton()->is_editor_hint()) {
    return;
  }
  NDIlib_send_create_t NDI_send_create_desc;

  NDI_send_create_desc.p_ndi_name = stream_name.utf8().get_data();
  ndi_sender = NDIlib_send_create(&NDI_send_create_desc);
  if (!ndi_sender) {
    return;
  }
  // maybe RGBX since I think we're pretty much guaranteed to not have alpha
  NDI_video_frame.FourCC = NDIlib_FourCC_type_RGBA;
}

GDNDI::~GDNDI() {
  NDIlib_send_destroy(ndi_sender);
  NDIlib_destroy();
}

// apparently, the only(?) way to change an ndi stream's name is by destroying it
// and recreating it.
void GDNDI::set_stream_name(const String name) {
  stream_name = name;
  if (Engine::get_singleton()->is_editor_hint()) {
    return;
  }
  // send an empty frame to ensure the sender does not own memory
  NDIlib_send_send_video_async_v2(ndi_sender, NULL);
  // violently destroy and recreate the sender
  UtilityFunctions::print("wat");
  UtilityFunctions::print(name);
  NDIlib_send_destroy(ndi_sender);
  NDIlib_send_create_t NDI_send_create_desc;
  NDI_send_create_desc.p_ndi_name = name.utf8().get_data();
  ndi_sender = NDIlib_send_create(&NDI_send_create_desc);
}

String GDNDI::get_stream_name() const {
  return stream_name;
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
  // this is the magic godot line that tells the engine "give me the screen texture from the
  // nearest viewport as an image"
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


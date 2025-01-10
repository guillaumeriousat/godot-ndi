#pragma once
#include <cstddef>
#include <array>
#include "Processing.NDI.Lib.h"
#include <godot_cpp/classes/node.hpp>

namespace godot {

class GDNDI : public Node {
  GDCLASS(GDNDI, Node)

private:
  NDIlib_send_instance_t ndi_sender;
  NDIlib_video_frame_v2_t NDI_video_frame;
  double time_since_last_frame = 0.0;
  double framerate = 60.0;
  double frame_interval = 1/60.0;
  // lets say 6k by 6k is the highest we want to go
  static constexpr int MAX_SIZE = 6144*6144*sizeof(uint8_t);
  // preallocated frames
  std::array<std::array<uint8_t, MAX_SIZE>, 2> frames;
  int frame_index = 0;
protected:
  static void _bind_methods();

public:
  GDNDI();
  ~GDNDI();

  void _process(double delta) override;
  void set_framerate(const double framerate);
  double get_framerate() const;
};

}

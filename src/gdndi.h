#pragma once
#include <cstddef>
#include "Processing.NDI.Lib.h"
#include <godot_cpp/classes/node.hpp>

namespace godot {

class GDNDI : public Node {
  GDCLASS(GDNDI, Node)

private:
  NDIlib_send_instance_t ndi_sender;
  double time_since_last_frame = 0.0;
  double framerate = 60.0;
  double frame_interval = 1/60.0;
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

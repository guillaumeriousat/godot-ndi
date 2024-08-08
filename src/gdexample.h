#ifndef GDEXAMPLE_H
#define GDEXAMPLE_H

#include <godot_cpp/classes/sprite2d.hpp>
#include <Processing.NDI.Lib.h>

namespace godot {

class GDExample : public Node {
  GDCLASS(GDExample, Node)

private:
  double time_passed;
  NDIlib_send_instance_t ndi_sender

protected:
  static void _bind_methods();

public:
  GDExample();
  ~GDExample();

  void _process(double delta) override;
};

}

#endif

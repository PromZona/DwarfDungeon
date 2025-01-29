#pragma once

#include <godot_cpp/classes/ref.hpp>
#include <godot_cpp/classes/sprite2d.hpp>
#include <godot_cpp/variant/typed_array.hpp>

namespace godot {

class BehaviourManager : public Node
{
  GDCLASS(BehaviourManager, Node)

private:
  double time_passed;
  TypedArray<Node> m_enemies;

protected:
  static void _bind_methods();

public:
  BehaviourManager();
  ~BehaviourManager();

  void _process(double delta) override;
  void _ready() override;
};

} // namespace godot

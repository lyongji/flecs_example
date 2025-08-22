#include <simple_module.h>

namespace simple {

module::module(flecs::world &ecs) {
  // 向世界注册模块。模块实体将与 C++ 命名空间具有相同的层次结构（例如
  // simple::module）
  ecs.module<module>();

  // 模块中的所有内容都在模块的命名空间内创建，因此
  // Position 组件将被创建为 simple::module::Position

  // 组件注册是可选的，但是通过在模块构造函数中注册组件，
  // 它们将在模块的作用域内创建。
  ecs.component<Position>();
  ecs.component<Velocity>();

  ecs.system<Position, const Velocity>("Move").each(
      [](Position &p, const Velocity &v) {
        p.x += v.x;
        p.y += v.y;
      });
}

} // namespace simple

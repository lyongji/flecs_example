#include <custom_event.h>
#include <iostream>

// 观察者可以用来匹配自定义事件。自定义事件可以使用 ecs_emit 函数发出。
// 该函数也用于内置事件，因此内置事件和自定义事件使用相同的规则来与观察者匹配。
//
// 一个事件由三部分数据组成，用于与观察者匹配：
//  - 单个事件类型 (EcsOnAdd, EcsOnRemove, ...)
//  - 一个或多个事件 ID (Position, Velocity, ...)
//  - 来源（实体或表）

struct 位置 {
  double x, y;
};

// 创建标签类型以用作事件（也可以使用实体）
struct MyEvent {};

int main(int, char *[]) {
  flecs::world ecs;

  // 为自定义事件创建观察者
  ecs.observer<位置>().event<MyEvent>().each(
      [](flecs::iter &it, size_t i, 位置 &) {
        std::cout << " - " << it.event().name() << ": " << it.event_id().str()
                  << ": " << it.entity(i).name() << "\n";
      });

  // 观察者查询可以与实体匹配，因此在发出事件之前确保它具有 Position 组件。
  // 这尚未触发观察者。
  flecs::entity e = ecs.entity("e").set<位置>({10, 20});

  // 触发自定义事件
  ecs.event<MyEvent>().id<位置>().entity(e).emit();

  // 输出
  //   - MyEvent: Position: e
}

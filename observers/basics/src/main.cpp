#include <basics.h>
#include <iostream>

// 观察者提供了一种响应内置和用户定义事件的机制。它们类似于系统，
// 因为它们具有相同的回调签名并使用相同的查询接口来匹配实体，但是
// 它们有一个事件类型而不是阶段。
//
// 最常用的内置事件是：
//  - flecs::OnAdd：添加了组件
//  - flecs::OnRemove：移除了组件
//  - flecs::OnSet：组件的值已更改
//
// 只有在组件实际上被添加或移除时才会触发 OnAdd 和 OnRemove 事件。
// 如果应用程序调用 add 并且实体已经有了该组件，则不会发出事件。
// 同样，如果应用程序调用 remove 来移除实体没有的组件，也不会发出事件。
// 这与 OnSet 不同，OnSet 每次调用 set 或 modified 时都会被触发。
//
// 观察者在许多方面与组件钩子不同：
//  - 一个组件只能有一个钩子，而它可以匹配许多观察者
//  - 钩子匹配一个组件，而观察者可以匹配复杂的查询
//  - 钩子用于 add/set/remove 事件，观察者可以匹配自定义事件。

struct 位置 {
  double x, y;
};

int main(int, char *[]) {
  flecs::world ecs;

  // 创建一个观察者来响应三个事件
  ecs.observer<位置>()
      .event(flecs::OnAdd)
      .event(flecs::OnRemove)
      .event(flecs::OnSet)
      .each([](flecs::iter &it, size_t i, 位置 &p) {
        if (it.event() == flecs::OnAdd) {
          // 在这里不应对接组件的值做任何假设。如果
          // 注册了组件的构造函数，它会在 EcsOnAdd 事件之前被调用，
          // 但通过 set 分配的值在这里不可见。
          std::cout << " - OnAdd: " << it.event_id().str().c_str() << ": "
                    << it.entity(i).name().c_str() << "\n";
        } else {
          // EcsOnSet 或 EcsOnRemove 事件
          std::cout << " - " << it.event().name() << ": " << it.event_id().str()
                    << ": " << it.entity(i).name() << ": {" << p.x << ", "
                    << p.y << "}\n";
        }
      });

  // 创建实体，设置 Position（发出 EcsOnAdd 和 EcsOnSet）
  flecs::entity e = ecs.entity("e").set<位置>({10, 20});

  // 移除组件（发出 EcsOnRemove）
  e.remove<位置>();

  // 再次移除组件（不发出事件）
  e.remove<位置>();

  // 输出示例
  //   - OnAdd: Position: e
  //   - OnSet: Position: e: {10, 20}
  //   - OnRemove: Position: e: {10, 20}
}

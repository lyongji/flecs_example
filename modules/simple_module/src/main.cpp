#include <iostream>
#include <simple_module.h>

int main(int, char *[]) {
  flecs::world ecs;

  ecs.import <simple::module>();

  // 创建一个使用模块中组件的系统
  ecs.system<const simple::Position>("PrintPosition")
      .each([](const simple::Position &p) {
        std::cout << "p = {" << p.x << ", " << p.y << "} (system)\n";
      });

  // 使用导入的组件创建实体
  flecs::entity e =
      ecs.entity().set<simple::Position>({10, 20}).set<simple::Velocity>(
          {1, 1});

  // 调用progress函数以运行导入的Move系统
  ecs.progress();

  // 在操作中使用模块中的组件
  e.get([](const simple::Position &p) {
    std::cout << "p = {" << p.x << ", " << p.y << "} (get)\n";
  });

  // 输出：
  //   p = {11.000000, 22.000000} (system)
  //   p = {11.000000, 22.000000} (get)
}

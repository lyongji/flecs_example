#include <iostream>
#include <systems.h>

struct 位置 {
  double x, y;
};

struct 速度 {
  double x, y;
};

int main(int, char *[]) {
  flecs::world ecs;

  // 为位置和速度创建一个系统。系统类似于查询（参见查询），但具有可以运行或调度（参见管道）的函数。
  flecs::system s = ecs.system<位置, const 速度>().each(
      [](flecs::entity e, 位置 &p, const 速度 &v) {
        p.x += v.x;
        p.y += v.y;
        std::cout << e.name() << ": {" << p.x << ", " << p.y << "}\n";
      });

  // 为位置和速度查询创建几个测试实体
  ecs.entity("e1").set<位置>({10, 20}).set<速度>({1, 2});

  ecs.entity("e2").set<位置>({10, 20}).set<速度>({3, 4});

  // 这个实体不匹配，因为它没有位置和速度组件
  ecs.entity("e3").set<位置>({10, 20});

  // 运行系统
  s.run();
}

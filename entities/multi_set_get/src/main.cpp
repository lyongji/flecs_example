#include <iostream>
#include <multi_set_get.h>

/* 这个代码展示了如何在单个命令中获取和设置多个组件 */

struct 位置 {
  double x, y;
};

struct 质量 {
  double value;
};

int main(int, char *[]) {
  flecs::world ecs;

  // 创建新实体，设置 Position 和 Mass 组件
  flecs::entity e = ecs.entity().insert([](位置 &p, 质量 &m) {
    p.x = 10;
    p.y = 20;
    m.value = 100;
  });

  // 打印 Position 和 Mass 组件的值
  bool found = e.get([](const 位置 &p, const 质量 &m) {
    std::cout << "位置: {" << p.x << ", " << p.y << "}\n"
              << "质量: {" << m.value << "}\n";
  });

  std::cout << "寻找组件: " << (found ? "true" : "false") << "\n";

  // 输出:
  //  Position: {10, 20}
  //  Mass: {100}
  //  Components found: true

  return 0;
}

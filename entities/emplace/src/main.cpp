#include <emplace.h>
#include <iostream>

// emplace 方法直接在存储中构造一个组件，而不是使用常规的
// set，后者将值移动到存储中已经构造的值。 这减少了创建新组件的开销。
//
// 没有默认构造函数的组件需要使用 emplace 来创建，因为它们不能被 flecs
// 自动构造。 使用 emplace 可以避免创建新组件的开销。
// emplace 安置,放置 在容器指定位置直接构造对象，节省了对象的复制或移动步骤

struct 无默认构造函数 {
  无默认构造函数(double x_, double y_) : x(x_), y(y_) {}
  double x, y;
};

int main(int, char *[]) {
  flecs::world ecs;

  flecs::entity e = ecs.entity().emplace<无默认构造函数>(10.0, 20.0);

  const 无默认构造函数 &c = e.get<无默认构造函数>();
  std::cout << "{" << c.x << ", " << c.y << "}" << "\n";

  // 输出
  //  {10, 20}
}

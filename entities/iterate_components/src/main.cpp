#include <iostream>
#include <iterate_components.h>
#include <print>

// 普通组件
struct 位置 {
  double x, y;
};

struct 速度 {
  double x, y;
};

// 标签
struct 人类 {};

// 用于创建对的两个标签
struct 吃 {};
struct 苹果 {};

void 迭代组件(flecs::entity e) {
  // 1. 打印组件的最简单方法是使用 type::str
  // std::cout << e.type().str() << "\n\n";
  std::println("{}\n", e.type().str().c_str());
  // 2. 要获取单个组件 id，请使用 entity::each
  int32_t i = 0;
  e.each([&](flecs::id id) { std::println("{}: {}", i++, id.str().c_str()); });
  std::println("");

  // 3. 我们也可以用自己的方式检查并打印
  // id。这有点复杂，因为我们需要处理可以编码在 id
  // 中的边缘情况，但这提供了最大的灵活性。
  i = 0;
  e.each([&](flecs::id id) {
    std::cout << i++ << ": ";

    if (id.is_pair()) {
      // 如果 id 是一个对，提取并打印对的两部分
      flecs::entity rel = id.first();
      flecs::entity tgt = id.second();
      std::println("关系: {}, 目标: {}", rel.name().c_str(),
                   tgt.name().c_str());
    } else {
      // Id 包含一个常规实体。在打印前去掉角色。
      flecs::entity comp = id.entity();
      std::print("实体: {}", comp.name().c_str());
    }
    std::println("");
  });

  std::println("\n");
}

int main(int, char *[]) {
  flecs::world ecs;

  flecs::entity bob = ecs.entity()
                          .set<位置>({10, 20})
                          .set<速度>({1, 1})
                          .add<人类>()
                          .add<吃, 苹果>();

  std::println("角色组件构成: ");
  迭代组件(bob);

  // 我们可以使用相同的函数来遍历组件的组件
  std::println("位置组件构成: ");
  迭代组件(ecs.component<位置>());
}

// 输出:

// Bob's components:
// Position, Velocity, Human, (Eats,苹果)
//
// 0: Position
// 1: Velocity
// 2: Human
// 3: (Eats,苹果)
//
// 0: entity: Position
// 1: entity: Velocity
// 2: entity: Human
// 3: rel: Eats, tgt: 苹果
//
//
// Position's components:
// Component, (Identifier,Name), (Identifier,Symbol), (OnDelete,Panic)
//
// 0: Component
// 1: (Identifier,Name)
// 2: (Identifier,Symbol)
// 3: (OnDelete,Panic)
//
// 0: entity: Component
// 1: rel: Identifier, tgt: Name
// 2: rel: Identifier, tgt: Symbol
// 3: rel: OnDelete, tgt: Panic

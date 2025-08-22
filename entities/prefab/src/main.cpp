#include <iostream>
#include <prefab.h> //预制件

// 预制件是可以用作其他实体模板的实体。它们
// 使用内置的 Prefab 标签创建，该标签默认将它们从
// 查询和系统中排除。

struct 攻击 {
  double value;
};
struct 防御 {
  double value;
};
struct 装载容量 {
  double value;
};
struct 脉冲速度 {
  double value;
};
struct HasFTL {};

struct 位置 {
  double x = 0;
  double y = 0;
};

int main(int, char *[]) {
  flecs::world ecs;

  // 创建预制件层次结构。
  flecs::entity 宇宙飞船 =
      ecs.prefab("宇宙飞船")
          // 像往常一样向预制件实体添加组件
          .set<脉冲速度>({50})
          .set<防御>({50})

          // 默认情况下，继承层次结构中的组件在实体之间是共享的。
          // 覆盖函数确保实例具有
          // 组件的私有副本。
          .auto_override<位置>();

  flecs::entity 货船 = ecs.prefab("货船")
                           // 等同于 .add(flecs::IsA, 宇宙飞船)。这确保实体
                           // 继承自 宇宙飞船 的所有组件。
                           .is_a(宇宙飞船)
                           .add<HasFTL>()
                           .set<装载容量>({100})
                           .set<防御>({100});

  flecs::entity 巨型货船 =
      ecs.prefab("巨型货船").is_a(货船).set<装载容量>({500}).set<防御>({300});

  ecs.prefab("护卫舰")
      .is_a(宇宙飞船)
      .add<HasFTL>()
      .set<攻击>({100})
      .set<防御>({75})
      .set<脉冲速度>({125});

  // 从预制件创建常规实体。
  // 由于在 spaceship 实体中的覆盖，实例将拥有
  // 位置组件的私有副本，其他所有组件都是共享的。
  flecs::entity inst = ecs.entity("我的巨型货船").is_a(巨型货船);

  // 检查实体的类型。这输出：
  //    Position,(Identifier,Name),(IsA,MammothFreighter)
  std::cout << "Instance type: [" << inst.type().str() << "]\n";

  // 即使实例没有 ImpulseSpeed 的私有副本，我们
  // 仍然可以使用常规 API 获取它（输出 50）
  const 脉冲速度 &ptr = inst.get<脉冲速度>();
  std::cout << "Impulse speed: " << ptr.value << "\n";

  // 预制件组件可以像常规组件一样迭代：
  ecs.each([](flecs::entity e, 脉冲速度 &is, 位置 &p) {
    p.x += is.value;
    std::cout << e.name() << ": {" << p.x << ", " << p.y << "}\n";
  });
}

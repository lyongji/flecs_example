#include <basics.h>
#include <print>

struct 位置 {
  double x, y;
};

struct 行走 {};

int main(int, char *[]) {
  flecs::world 世界;

  // 创建一个名为 Bob 的实体
  flecs::entity bob =
      世界.entity("Bob")
          // set 操作查找或创建组件，并设置其值。
          // 组件会自动注册到世界中。
          .set<位置>({10, 20})
          // add
          // 操作添加组件而不设置值。这在添加标签或添加具有默认值的组件时很有用。
          .add<行走>();

  // 获取 Position 组件的值
  const 位置 &c = bob.get<位置>();
  std::println("{{{},{}}}", c.x, c.y);

  // 覆盖 Position 组件的值
  bob.set<位置>({20, 30});

  // 创建另一个名为 Alice 的实体
  flecs::entity alice = 世界.entity("Alice").set<位置>({10, 20});

  // 在实体创建后添加标签
  alice.add<行走>();

  // 打印实体具有的所有组件。这将输出：
  //    Position, Walking, (Identifier,Name)
  std::println("[{}]", alice.type().str().c_str());

  // 移除标签
  alice.remove<行走>();

  // 遍历所有具有 Position 组件的实体
  世界.each([](flecs::entity e, 位置 &p) {
    std::println("{{{}:{},{}}}", e.name().c_str(), p.x, p.y);
  });

  // 输出
  //  {10, 20}
  //  [Position, Walking, (Identifier,Name)]
  //  Alice: {10, 20}
  //  Bob: {20, 30}
}

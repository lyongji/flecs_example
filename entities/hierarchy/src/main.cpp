// 包含头文件
#include <hierarchy.h>
#include <print>

// 定义位置结构体
struct 位置 {
  double x, y;
};

// 定义星体、行星和月球的空结构体,空结构在flecs中视作 标签
struct 恒星 {};
struct 行星 {};
struct 卫星 {};

// 深度优先遍历实体树
void 迭代实体树(flecs::entity 实体, 位置 父级位置 = {0, 0}) {
  // 打印实体的层次名称和实体类型
  std::println("{}[{}]", 实体.path().c_str(), 实体.type().str().c_str());

  // 获取实体的位置
  const 位置 &实体位置 = 实体.get<位置>();

  // 计算实际位置
  位置 实际位置 = {实体位置.x + 父级位置.x, 实体位置.y + 父级位置.y};
  std::println("{{{},{}}}", 实际位置.x, 实际位置.y);

  // 递归遍历子实体
  实体.children([&](flecs::entity 子级) { 迭代实体树(子级, 实际位置); });
}

int main(int, char *[]) {
  // 创建 ECS 世界
  flecs::world 世界;

  // 创建简单的实体层次结构
  // 层次结构使用 ECS 关系和内置的 flecs::ChildOf 关系
  // 将实体作为其他实体的子实体创建。

  flecs::entity 太阳 = 世界.entity("太阳").add<恒星>().set<位置>({1, 1});

  世界.entity("水星")
      .child_of(太阳) // 添加 flecs::ChildOf, sun 的快捷方式
      .add<行星>()
      .set<位置>({1, 1});

  世界.entity("金星").child_of(太阳).add<行星>().set<位置>({2, 2});

  flecs::entity 地球 =
      世界.entity("地球").child_of(太阳).add<行星>().set<位置>({3, 3});

  flecs::entity 月球 =
      世界.entity("月球").child_of(地球).add<卫星>().set<位置>({0.1, 0.1});

  // 月球是地球的子实体吗？
  std::println("是地球的子级? {} ", 月球.has(flecs::ChildOf, 地球));

  // 通过名称查找月球
  flecs::entity e = 世界.lookup("太阳::地球::月球");
  std::println("月亮位于-> {} ", e.path().c_str());

  // 对树进行深度优先遍历
  迭代实体树(太阳);

  // 输出:
  //   Child of Earth? 1
  //
  //   Moon found: ::Sun::Earth::Moon
  //
  //   ::Sun [Star, Position, (Identifier,Name)]
  //   {1, 1}
  //
  //   ::Sun::Mercury [Position, Planet, (Identifier,Name), (ChildOf,Sun)]
  //   {2, 2}
  //
  //   ::Sun::Venus [Position, Planet, (Identifier,Name), (ChildOf,Sun)]
  //   {3, 3}
  //
  //   ::Sun::Earth [Position, Planet, (Identifier,Name), (ChildOf,Sun)]
  //   {4, 4}
  //
  //   ::Sun::Earth::Moon [Position, Moon, (Identifier,Name),
  //   (ChildOf,Sun.Earth)] {4.1, 4.1}
}

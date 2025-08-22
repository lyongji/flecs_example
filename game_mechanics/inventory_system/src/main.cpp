#include <inventory_system.h>
#include <iostream>
#include <print>

// 此示例展示了使用ECS关系实现库存系统的可能方式。
// ECS关系示例。

// 库存标签/关系
struct Item {};        // 基础物品类型
struct Container {};   // 容器标签
struct Inventory {};   // 库存标签
struct ContainedBy {}; // 包含关系

// 物品/单位属性
struct 激活 {}; // 物品处于激活/穿戴状态

struct 总数 {
  int value; // 实例代表的物品数量
};

struct 健康值 {
  int value; // 物品的健康值
};

struct 攻击 {
  int value; // 物品每次使用造成的伤害量
};

// 物品
struct 刀 : Item {};
struct 盔甲 : Item {};
struct 金币 : Item {};

// 物品预制类型
struct 木刀 {};
struct 铁刀 {};
struct 木甲 {};
struct 铁甲 {};

// 查找物品类型的实体
flecs::entity 查找物品项(flecs::entity 物品项) {
  flecs::world world = 物品项.world();
  flecs::entity 结果;

  物品项.each([&](flecs::id id) {
    if (id.is_entity()) {
      // 如果id是一个普通实体（组件），检查组件是否继承自Item
      if (id.entity().has(flecs::IsA, world.id<Item>())) {
        结果 = id.entity();
      }
    } else if (id.is_pair()) {
      // 如果物品有一个基实体，检查基实体是否有一个属性是Item。
      if (id.first() == flecs::IsA) {
        flecs::entity base_kind = 查找物品项(id.second());
        if (base_kind) {
          结果 = base_kind;
        }
      }
    }
  });

  return 结果;
}

// 与item_kind几乎相同，但返回预制名称而不是物品类型。这
// 返回一个更用户友好的名称，如"WoodenSword"而不是"Sword"
const char *物品项名称(flecs::entity 物品项) {
  flecs::world world = 物品项.world();
  const char *结果 = NULL;

  物品项.each([&](flecs::id id) {
    if (id.is_entity()) {
      if (id.entity().has(flecs::IsA, world.id<Item>())) {
        结果 = id.entity().name();
      }
    } else if (id.is_pair()) {
      if (id.first() == flecs::IsA) {
        flecs::entity base_kind = 查找物品项(id.second());
        if (base_kind) {
          结果 = id.second().name();
        }
      }
    }
  });

  return 结果;
}

// 如果实体不是容器，获取其库存
flecs::entity get_container(flecs::entity 容器) {
  if (容器.has<Container>()) {
    return 容器;
  }
  return 容器.target<Inventory>();
}

// 遍历库存中的所有物品
// template <typename Func>
void for_each_item(flecs::entity container, const auto &func) {
  container.world().query_builder().with<ContainedBy>(container).each(func);
}

// 在指定类型的库存中查找物品
flecs::entity find_item_w_kind(flecs::entity container, flecs::entity kind,
                               bool active_required = false) {
  flecs::entity result;

  container = get_container(container);

  for_each_item(container, [&](flecs::entity item) {
    // 检查是否只应返回激活的物品。这在搜索需要装备的物品时很有用。
    if (active_required) {
      if (!item.has<激活>()) {
        return;
      }
    }

    flecs::entity ik = 查找物品项(item);
    if (ik == kind) {
      result = item;
    }
  });

  return result;
}

// 将物品转移到容器中
void transfer_item(flecs::entity 容器, flecs::entity 物品项) {
  const 总数 *amt = 物品项.try_get<总数>();
  if (amt) {
    // 如果物品有数量，我们需要检查容器是否已经有了这种类型的物品，并增加数量。
    flecs::world ecs = 容器.world();
    flecs::entity ik = 查找物品项(物品项);
    flecs::entity dst_item = find_item_w_kind(容器, ik);
    if (dst_item) {
      // 如果找到了匹配的物品，增加其数量
      总数 &dst_amt = dst_item.ensure<总数>();
      dst_amt.value += amt->value;
      物品项.destruct(); // 移除源物品
      return;
    } else {
      // 如果没有找到匹配的物品，继续执行，将物品从源容器移动到目标容器
    }
  }

  // 将物品移动到目标容器（替换之前的ContainedBy，如果有的话）
  物品项.add<ContainedBy>(容器);
}

// 将物品从一个容器移动到另一个容器
void transfer_items(flecs::entity dst, flecs::entity src) {

  std::println(">> 将物品从 {} 转移到 {}\n", src.name().c_str(),
               dst.name().c_str());

  // 延迟执行，因为我们正在迭代时添加/移除组件
  dst.world().defer([&] {
    dst = get_container(dst); // 确保用容器替换玩家
    src = get_container(src);

    for_each_item(src, [&](flecs::entity item) { transfer_item(dst, item); });
  });
}

// 攻击玩家
void attack(flecs::entity 玩家, flecs::entity 武器) {
  flecs::world ecs = 玩家.world();

  std::println(">> {} 被一个 {} 攻击", 玩家.name().c_str(), 物品项名称(武器));

  const 攻击 *att = 武器.try_get<攻击>();
  if (!att) {
    // 没有攻击力的武器？奇怪。
    std::println(" - 这把武器是个哑弹");
    return;
  }

  int att_value = att->value;

  // 获取玩家装备的盔甲物品（如果有）
  flecs::entity armor = find_item_w_kind(玩家, ecs.entity<盔甲>(), true);
  if (armor) {
    健康值 *armor_health = armor.try_get_mut<健康值>();
    if (!armor_health) {
      // 没有防御力的盔甲？奇怪。
      std::println(" - {}  盔甲是个哑弹\n", 物品项名称(armor));
    } else {
      std::println(" - {} 用 {} 防御\n", 玩家.name().c_str(),
                   物品项名称(armor));

      // 从盔甲健康值中减去攻击力。如果盔甲健康值降至零以下，删除盔甲并携带剩余的攻击点数。
      armor_health->value -= att_value;
      if (armor_health->value <= 0) {
        att_value += armor_health->value;
        armor.destruct();
        std::println(" - {} 被摧毁！\n", 物品项名称(armor));
      } else {
        std::println(" - {} 在受到 {} 点伤害后还剩下 {} 点健康值\n",
                     物品项名称(armor), att_value, armor_health->value);
        att_value = 0;
      }
    }
  } else {
    // 勇敢但愚蠢
    std::println(" - {} 不穿盔甲战斗！\n", 玩家.name().c_str());
  }

  // 对于武器的每次使用，减少其健康值
  健康值 &weapon_health = 武器.ensure<健康值>();
  if (!--weapon_health.value) {
    std::cout << " - " << 物品项名称(武器) << " 被摧毁！\n";
    武器.destruct();
  } else {
    std::cout << " - " << 物品项名称(武器) << " 还剩下 " << weapon_health.value
              << " 次使用机会";
  }

  // 如果盔甲没有完全抵消攻击，从玩家健康值中减去
  if (att_value) {
    健康值 &player_health = 玩家.ensure<健康值>();
    if (!(player_health.value -= att_value)) {
      std::cout << " - " << 玩家.name() << " 死亡！\n";
      玩家.destruct();
    } else {
      std::cout << " - " << 玩家.name() << " 在受到 " << att_value
                << " 点伤害后还剩下 " << player_health.value << " 点健康值\n";
    }
  }

  std::cout << "\n";
}

// 打印库存中的物品
void print_items(flecs::entity container) {
  std::cout << "-- " << container.name() << " 的库存：\n";

  // 如果提供了玩家实体，确保我们使用的是其库存实体。
  container = get_container(container);

  int32_t count = 0;
  for_each_item(container, [&](flecs::entity item) {
    // 带有Amount组件的物品占用一个库存槽，但代表多个实例，如硬币。
    int32_t amount = 1;
    item.get([&](const 总数 &amt) { amount = amt.value; });

    std::cout << " - " << amount << " " << 物品项名称(item);
    if (amount > 1)
      std::cout << "s";
    std::cout << " (" << 查找物品项(item).name() << ")\n";

    count++;
  });

  if (!count) {
    std::cout << " - << 空 >>\n";
  }

  std::cout << "\n";
}

int main(int, char *[]) {
  flecs::world ecs;

  // 注册ContainedBy关系
  ecs.component<ContainedBy>().add(flecs::Exclusive); // 物品只能被一个容器包含

  // 注册物品类型
  ecs.component<刀>().is_a<Item>();
  ecs.component<盔甲>().is_a<Item>();
  ecs.component<金币>().is_a<Item>();

  // 注册物品预制
  ecs.prefab<木刀>().add<刀>().set<攻击>({1}).set_auto_override<健康值>(
      {5}); // 复制到实例，不共享

  ecs.prefab<铁刀>().add<刀>().set<攻击>({2}).set_auto_override<健康值>({10});

  ecs.prefab<木甲>().add<盔甲>().set_auto_override<健康值>({10});

  ecs.prefab<铁甲>().add<盔甲>().set_auto_override<健康值>({20});

  // 创建一个带有物品的宝箱
  flecs::entity loot_box =
      ecs.entity("Chest").add<Container>().with<ContainedBy>([&] {
        ecs.entity().is_a<铁刀>();
        ecs.entity().is_a<木甲>();
        ecs.entity().add<金币>().set<总数>({30});
      });

  // 创建一个带有库存的玩家实体
  flecs::entity player = ecs.entity("Player").set<健康值>({10}).add<Inventory>(
      ecs.entity().add<Container>().with<ContainedBy>(
          [&] { ecs.entity().add<金币>().set<总数>({20}); }));

  // 打印宝箱中的物品
  print_items(loot_box);

  // 打印玩家库存中的物品
  print_items(player);

  // 将宝箱中的物品复制到玩家库存中
  transfer_items(player, loot_box);

  // 打印转移后的玩家库存中的物品
  print_items(player);

  // 打印转移后的宝箱中的物品
  print_items(loot_box);

  // 查找盔甲实体并装备
  flecs::entity armor = find_item_w_kind(player, ecs.entity<盔甲>());
  if (armor) {
    armor.add<激活>();
  }

  // 创建一个武器来攻击玩家
  flecs::entity my_sword = ecs.entity().is_a<铁刀>();

  // 攻击玩家
  attack(player, my_sword);

  std::cout << "\n";

  // 输出
  // -- 宝箱的库存：
  //  - 1 IronSword (Sword)
  //  - 1 WoodenArmor (Armor)
  //  - 30 Coins (Coin)

  // -- 玩家的库存：
  //  - 20 Coins (Coin)

  // >> 将物品从宝箱转移到玩家

  // -- 玩家的库存：
  //  - 50 Coins (Coin)
}
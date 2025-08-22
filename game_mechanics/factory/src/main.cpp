#include <factory.h>
#include <iostream>

// 此示例展示了如何在 ECS 中模拟简单的生产层次结构。
// 示例包含以下概念：
//
// - 配方描述了生产资源所需的要求和时间
// - 工厂根据分配的配方生产资源
// - 仓库存储资源
//
// 工厂需要连接到提供资源的输入。一旦输入连接正确，
// 工厂将在从输入收集到所有所需资源后开始生产资源。
//
// 工厂可以作为其他工厂的输入。仓库可以用于
// 组合多个工厂的输出。
//
// 资源定义和场景在运行时从 resource.flecs
// 和 scene.flecs 加载。
//
// 几点说明：
// - 该演示没有图形，但可以通过资源管理器查看。
// - 组件将实体作为 entity_t 的引用存储。它们也可以是
//   flecs::entity 的引用，但这需要更少的内存。
// - 一些数据在资源组件和工厂组件之间是重复的，以便
//   系统主要可以在本地数据上操作
//

using namespace flecs;

// 工厂模块
struct factories {
  // 配方可以依赖的最大资源数量
  static constexpr int MaxInputs = 3;

  // -- 配方组件

  // 配方的单个资源要求
  struct Requirement {
    entity_t resource;
    int32_t amount;
  };

  // 配方的所有资源要求
  struct Requires {
    Requirement items[MaxInputs];
  };

  // 生产资源所需的时间
  struct TimeToProduce {
    float value;
  };

  // 限制特定资源的堆叠数量
  struct StackCount {
    int32_t amount;
  };

  // -- 仓库 & 工厂组件

  // 资源存储
  struct Stores {
    int32_t amount;
  };

  // 工厂配置
  struct Factory {
    // 要生产的资源的配方
    entity_t recipe;

    // 必须提供配方指定的资源
    entity_t inputs[MaxInputs];

    // 可选的输出，允许工厂将物品转发到仓库
    entity_t output;
  };

  // 工厂状态
  enum class FactoryState {
    // 工厂尚未连接到（正确）输入
    Idle,

    // 工厂已连接，但尚未拥有所需的资源
    WaitingForResources,

    // 工厂正在生产资源
    Producing,

    // 工厂已完成生产
    TransferResource
  };

  // 跟踪已提供给工厂的资源
  struct FactorySupply {
    int32_t required[MaxInputs];
    int32_t collected[MaxInputs];
    flecs::ref<Stores> inputs[MaxInputs];
    flecs::ref<Stores> output;
  };

  // 跟踪生产进度
  struct FactoryProduction {
    float value;
    float duration;
    int32_t max_stack;
  };

  // 模块导入函数
  factories(world &world) {
    // 单位改进了资源管理器中组件值的可视化。
    world.import <flecs::units>();

    // -- 组件注册
    // 反射数据已注册，以便我们可以从资源管理器中查看组件，
    // 并从 .flecs 文件中分配它们。

    world.component<Stores>().member<int32_t>("amount");

    world.component<Requirement>()
        .member(flecs::Entity, "resource")
        .member<int32_t>("amount");

    world.component<Requires>().array<Requirement>(3);

    world.component<FactorySupply>()
        .member<int32_t>("required", 3)
        .member<int32_t>("collected", 3);

    world.component<TimeToProduce>().member<float, units::duration::Seconds>(
        "value");

    world.component<StackCount>().member<int32_t>("amount");

    world.component<FactoryProduction>()
        .member<float, units::Percentage>("value")
        .member<float, units::duration::Seconds>("duration")
        .member<int32_t>("max_stack");

    world
        .component<Factory>()
        // 工厂一开始是空闲状态
        .on_add(
            [](entity factory, Factory) { factory.add(FactoryState::Idle); })
        // 当组件值被赋值时检查工厂是否正确设置
        .on_set(factory_init)
        .member(flecs::Entity, "recipe")
        .member(flecs::Entity, "inputs", 3)
        .member(flecs::Entity, "output");

    // 从输入收集资源的系统
    world.system<FactorySupply>("Collect")
        .with(FactoryState::WaitingForResources)
        .interval(1.0f)
        .each([](entity factory, FactorySupply &s) {
          bool satisfied = true;

          for (int i = 0; i < MaxInputs; i++) {
            int32_t needed = s.required[i] - s.collected[i];
            if (needed) {
              Stores *p = s.inputs[i].get();
              if (p->amount >= needed) {
                s.collected[i] += needed;
                p->amount -= needed;
              } else {
                s.collected[i] += p->amount;
                p->amount = 0;
                satisfied = false;
              }
            }
          }

          // 如果所有资源都满足，将工厂状态更改为生产
          if (satisfied) {
            // 消耗已收集的资源
            for (int i = 0; i < MaxInputs; i++) {
              s.collected[i] = 0;
            }

            factory.add(FactoryState::Producing);
          }
        });

    // 一旦所有输入要求都满足，生产资源的系统
    world.system<FactoryProduction>("Produce")
        .with(FactoryState::Producing)
        .interval(0.1f)
        .each([](flecs::iter &it, size_t i, FactoryProduction &p) {
          p.value += it.delta_system_time() / p.duration;
          if (p.value >= 1) {
            p.value = 1;
            it.entity(i).add(FactoryState::TransferResource);
          }
        });

    // 转移资源并重置工厂以生产下一个项目的系统
    world.system<FactorySupply, FactoryProduction, Stores>("Transfer")
        .term_at(2)
        .second(Wildcard)
        .with(FactoryState::TransferResource)
        .interval(1.0f)
        .each([](entity factory, FactorySupply &s, FactoryProduction &p,
                 Stores &out) {
          // 重置生产进度
          p.value = 0;

          // 如果配置了仓库，先转移本地资源
          Stores *depot = s.output.try_get();
          if (depot) {
            int32_t available_space = p.max_stack - depot->amount;
            if (out.amount > available_space) {
              depot->amount += available_space;
              out.amount -= available_space;
            } else {
              depot->amount += out.amount;
              out.amount = 0;
            }
          }

          // 现在检查将新资源项输出到哪个存储。如果配置了
          // 仓库，尝试将其存储在那里，否则存储在工厂中。
          Stores *store = &out;
          if (depot) {
            if (!p.max_stack || (depot->amount < p.max_stack)) {
              store = depot;
            }
          }

          if (store->amount < p.max_stack) {
            // 将资源添加到存储，并返回收集资源
            store->amount++;
            factory.add(FactoryState::WaitingForResources);
          } else {
            // 输出无空间，不做任何操作
          }
        });
  }

private:
  // Factory 组件的 on_set 钩子。当 Factory 组件值被写入时，
  // 检查工厂是否有配方以及工厂输入是否满足配方要求。
  static void factory_init(entity factory, Factory &config) {
    world world = factory.world();
    entity recipe = world.entity(config.recipe);

    recipe.get([&](const Requires &r) {
      entity output = world.entity(config.output);
      if (output) {
        if (!output.has<Stores>(recipe)) {
          std::cout << factory.path() << ": output doesn't provide resource "
                    << recipe.path() << "\n";
          return;
        }
      }

      // 确保每个配方要求都连接了正确的输入
      bool satisfied = true;
      for (int i = 0; i < MaxInputs; i++) {
        entity resource = world.entity(r.items[i].resource);
        entity input = world.entity(config.inputs[i]);
        if (!resource) {
          if (input) {
            std::cout << factory.path()
                      << ": input connected to empty input slot\n";
          }
          continue;
        }

        if (resource && !input) {
          satisfied = false;
          break;
        }

        if (!input.has<Stores>(resource)) {
          std::cout << factory.path() << ": input doesn't provide resource "
                    << recipe.path() << "\n";
          satisfied = false;
          break;
        }
      }

      // 如果我们通过了所有要求而没有问题，工厂就准备好
      // 开始收集资源
      if (satisfied) {
        factory.add(FactoryState::WaitingForResources);

        // 初始化供应组件
        factory.insert([&](FactorySupply &s) {
          for (int i = 0; i < MaxInputs; i++) {
            entity resource = world.entity(r.items[i].resource);
            entity input = world.entity(config.inputs[i]);
            int32_t amount = r.items[i].amount;

            s.required[i] = amount;
            s.collected[i] = 0;

            if (!resource) {
              continue;
            }

            // 获取工厂输入的资源提供者的引用。引用
            // 提供了快速方便地重复访问同一实体的组件的访问方式。
            s.inputs[i] = input.get_ref<Stores>(resource);
          }

          if (output) {
            // 如果工厂配置了输出，也获取其引用，以便
            // 快速将生产的资源添加到其中。
            s.output = output.get_ref<Stores>(recipe);
          }
        });

        // 添加跟踪生产资源所需时间的组件
        recipe.get([&](const TimeToProduce &ttp, const StackCount &sc) {
          factory.set<FactoryProduction>({0, ttp.value, sc.amount});
        });

        // 设置工厂的输出资源
        factory.add<Stores>(recipe);
      }
    });
  }
};

int main(int argc, char *argv[]) {
  flecs::world world(argc, argv);

  world.import <factories>();

  ecs_script_run_file(world, "resources.flecs");
  ecs_script_run_file(world, "scene.flecs");

  return world.app()
      .enable_rest()
      .target_fps(60)
      .delta_time(1.0f / 60.0f) // 以固定时间步运行
      .run();
}

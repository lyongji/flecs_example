#include <iostream>
#include <scene_management.h>
#include <string>

// 这个示例展示了使用 pipelines 实现场景管理的可能方式。
// 使用 pipelines 实现场景管理。

// 场景关系/标签
struct ActiveScene {}; // 表示当前场景
struct SceneRoot {};   // 场景中所有唯一实体的父级

// 场景
using Pipeline = flecs::entity;
struct MenuScene {
  Pipeline pip;
};
struct GameScene {
  Pipeline pip;
};

// 示例组件
struct 位置 {
  float x, y;
};
struct Button {
  std::string text;
};
struct Character {
  bool alive;
};
struct 健康值 {
  int amount;
};

// 删除当前场景根级下的所有实体。
// (注意：应使用 defer_begin() / defer_end())
void reset_scene(flecs::world &ecs) {
  ecs.delete_with(flecs::ChildOf, ecs.entity<SceneRoot>());
}


void menu_scene(flecs::iter &it, size_t, ActiveScene) {
  std::cout << "\n>> ActiveScene has changed to `MenuScene`\n\n";

  flecs::world ecs = it.world();
  flecs::entity scene = ecs.component<SceneRoot>();

  reset_scene(ecs);

  // 进入菜单场景时创建一个开始菜单按钮。
  // when we enter the menu scene.
  ecs.entity("Start Button")
      .set(Button{"Play the Game!"})
      .set(位置{50, 50})
      .child_of(scene);

  ecs.set_pipeline(ecs.get<MenuScene>().pip);
}

void game_scene(flecs::iter &it, size_t, ActiveScene) {
  std::cout << "\n>> ActiveScene has changed to `GameScene`\n\n";

  flecs::world ecs = it.world();
  flecs::entity scene = ecs.component<SceneRoot>();

  reset_scene(ecs);

  // 进入游戏场景时创建一个玩家角色。
  // when we enter the game scene.
  ecs.entity("Player")
      .set(Character{})
      .set(健康值{2})
      .set(位置{0, 0})
      .child_of(scene);

  ecs.set_pipeline(ecs.get<GameScene>().pip);
}

void init_scenes(flecs::world &ecs) {
  // 游戏中只能有一个活动场景。
  // in a game at a time.
  ecs.component<ActiveScene>().add(flecs::Exclusive);

  // 每个场景都有一个 pipeline，它运行
  // 与之关联的系统以及所有其他与场景无关的系统。
  flecs::entity menu =
      ecs.pipeline()
          .with(flecs::System)
          .without<GameScene>() // Use "without()" of the other scenes
                                // so that we can run every system that
                                // doesn't have a scene attached to it.
          .build();

  flecs::entity game =
      ecs.pipeline().with(flecs::System).without<MenuScene>().build();

  // 在场景上设置 pipeline 实体，以便稍后使用 get() 容易找到它们。
  // to easily find them later with get().
  ecs.set<MenuScene>({menu});
  ecs.set<GameScene>({game});

  // 当 ActiveScene 添加 MenuScene 时调用场景更改逻辑的观察者。
  // Observer to call scene change logic for
  // MenuScene when added to the ActiveScene.
  ecs.observer<ActiveScene>("Scene Change to Menu")
      .event(flecs::OnAdd)
      .second<MenuScene>()
      .each(menu_scene);

  // 当 ActiveScene 添加 GameScene 时调用场景更改逻辑的观察者。
  // Observer to call scene change logic for
  // GameScene when added to the ActiveScene.
  ecs.observer<ActiveScene>("Scene Change to Game")
      .event(flecs::OnAdd)
      .second<GameScene>()
      .each(game_scene);
}

void init_systems(flecs::world &ecs) {
  // 无论我们当前处于哪个场景，都会运行。
  // Will run every time regardless of the
  // current scene we're in.
  ecs.system<const 位置>("Print Position")
      .each([](flecs::entity e, const 位置 &p) {
        // 打印实体的位置。
        std::cout << e.name() << ": {" << p.x << ", " << p.y << "}\n";
      });

  // 仅在游戏场景当前处于活动状态时运行。
  // Will only run when the game scene is
  // currently active.
  ecs.system<健康值>("Characters Lose Health")
      .kind<GameScene>()
      .each([](健康值 &h) {
        // 打印角色的健康值，然后将其减一。
        std::cout << h.amount << " health remaining\n";
        h.amount--;
      });

  // 仅在菜单场景当前处于活动状态时运行。
  // Will only run when the menu scene is
  // currently active.
  ecs.system<const Button>("Print Menu Button Text")
      .kind<MenuScene>()
      .each([](const Button &b) {
        // 打印菜单按钮的文本。
        std::cout << "Button says \"" << b.text << "\"\n";
      });
}

int main(int, char *[]) {
  flecs::world ecs;

  init_scenes(ecs);
  init_systems(ecs);

  ecs.add<ActiveScene, MenuScene>();
  ecs.progress();

  ecs.add<ActiveScene, GameScene>();
  ecs.progress();
  ecs.progress();
  ecs.progress();

  ecs.add<ActiveScene, MenuScene>();
  ecs.progress();

  ecs.add<ActiveScene, GameScene>();
  ecs.progress();
  ecs.progress();
  ecs.progress();

  // 输出
  // >> ActiveScene has changed to `MenuScene`

  // Start Button: {50, 50}
  // Button says "Play the Game!"

  // >> ActiveScene has changed to `GameScene`

  // Player: {0, 0}
  // 2 health remaining
  // Player: {0, 0}
  // 1 health remaining
  // Player: {0, 0}
  // 0 health remaining

  // >> ActiveScene has changed to `MenuScene`

  // Start Button: {50, 50}
  // Button says "Play the Game!"

  // >> ActiveScene has changed to `GameScene`

  // Player: {0, 0}
  // 2 health remaining
  // Player: {0, 0}
  // 1 health remaining
  // Player: {0, 0}
  // 0 health remaining
}


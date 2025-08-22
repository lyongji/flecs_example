#include <iostream>
#include <variant.h>

/* Prefabs can inherit from each other, which creates prefab variants. With
 * variants applications can reuse a common set of components and specialize it
 * by adding or overriding components on the variant. */

struct 攻击 {
  double value;
};

struct 防御 {
  double value;
};

struct 装载容量 {
  double value;
};

struct 比冲值 {
  double value;
};

struct 位置 {
  double x;
  double y;
};

int main() {
  flecs::world ecs;

  // Create a base prefab for SpaceShips.
  flecs::entity SpaceShip =
      ecs.prefab("SpaceShip").set<比冲值>({50}).set<防御>({25});

  // Create a Freighter variant which inherits from SpaceShip
  flecs::entity Freighter = ecs.prefab("Freighter")
                                .is_a(SpaceShip)
                                .set<装载容量>({100})
                                .set<防御>({50});

  // Create a MammothFreighter variant which inherits from Freighter
  flecs::entity MammothFreighter =
      ecs.prefab("MammothFreighter").is_a(Freighter).set<装载容量>({500});

  // Create a Frigate variant which inherits from SpaceShip
  ecs.prefab("Frigate")
      .is_a(SpaceShip)
      .set<攻击>({100})
      .set<防御>({75})
      .set<比冲值>({125});

  // Create an instance of the MammothFreighter. This entity will inherit the
  // ImpulseSpeed from SpaceShip, Defense from Freighter and FreightCapacity
  // from MammothFreighter.
  flecs::entity inst = ecs.entity("my_freighter").is_a(MammothFreighter);

  // Add a private Position component.
  inst.set<位置>({10, 20});

  // Instances can override inherited components to give them a private copy
  // of the component. This freighter got an armor upgrade:
  inst.set<防御>({100});

  // Queries can match components from multiple levels of inheritance
  ecs.each([](flecs::entity e, 位置 &p, 比冲值 &s, 防御 &d, 装载容量 &c) {
    std::cout << e.name() << ":\n";
    std::cout << " - position: " << p.x << ", " << p.y << "\n";
    std::cout << " - impulse speed: " << s.value << "\n";
    std::cout << " - defense: " << d.value << "\n";
    std::cout << " - capacity: " << c.value << "\n";
  });

  // Output:
  //   my_freighter:
  //    - position: 10, 20
  //    - impulse speed: 50
  //    - defense: 100
  //    - capacity: 500
}

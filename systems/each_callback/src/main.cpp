#include <each_callback.h>
#include <iostream>

// This example shows how to write a system with the each callback, which
// provides the easiest API for iterating over matched components.

struct 位置 {
  double x, y;
};

struct 速度 {
  double x, y;
};

int main(int, char *[]) {
  flecs::world ecs;

  flecs::system s =
      ecs.system<位置, const 速度>()
          // Arguments passed to each match components passed to system
          .each([](位置 &p, const 速度 &v) {
            p.x += v.x;
            p.y += v.y;
            std::cout << "{" << p.x << ", " << p.y << "}\n";
          });

  // Create a few test entities for a Position, Velocity query
  ecs.entity("e1").set<位置>({10, 20}).set<速度>({1, 2});

  ecs.entity("e2").set<位置>({10, 20}).set<速度>({3, 4});

  // This entity will not match as it does not have Position, Velocity
  ecs.entity("e3").set<位置>({10, 20});

  // Run the system
  s.run();

  // Output
  //  {11, 22}
  //  {13, 24}
}

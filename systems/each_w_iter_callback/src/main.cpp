#include <each_w_iter_callback.h>
#include <iostream>

// This example is the same as the each example, but in addition also shows how
// to get access to the flecs::iter object. Flecs iterators provide access to a
// lot of information about the currently iterated result, such as the matched
// entities, table, component ids, whether fields are set or not and more.

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
          .each([](flecs::iter &it, size_t row, 位置 &p, const 速度 &v) {
            flecs::entity e = it.entity(row);
            p.x += v.x;
            p.y += v.y;
            std::cout << e.name() << ": {" << p.x << ", " << p.y << "}\n";
          });

  // Create a few test entities for a Position, Velocity query
  ecs.entity("e1").set<位置>({10, 20}).set<速度>({1, 2});

  ecs.entity("e2").set<位置>({10, 20}).set<速度>({3, 4});

  // This entity will not match as it does not have Position, Velocity
  ecs.entity("e3").set<位置>({10, 20});

  // Run the system
  s.run();

  // Output
  //  e1: {11, 22}
  //  e2: {13, 24}
}

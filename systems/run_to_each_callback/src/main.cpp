#include <iostream>
#include <run_to_each_callback.h>

// This example shows how a system that uses the run callback can forward to an
// each callback. This is often useful for installing generic behavior (like
// profiling, tracing) that wraps around the each callback.

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
          // Forward each result from the run callback to the each callback.
          .run(
              [](flecs::iter &it) {
                std::cout << "Move begin\n";

                // Walk over the iterator, forward to the system callback
                while (it.next()) {
                  it.each();
                }

                std::cout << "Move end\n";
              },
              [](flecs::entity e, 位置 &p, const 速度 &v) {
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
  //  Move begin
  //  e1: {11, 22}
  //  e2: {13, 24}
  //  Move end
}

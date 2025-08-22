#include <iostream>
#include <run_callback.h>

// This example shows how to use the run callback, which provides more direct
// control over the iteration of the system. It also makes it easier to run code
// before and after the iteration.

struct 位置 {
  double x, y;
};

struct 速度 {
  double x, y;
};

int main(int, char *[]) {
  flecs::world ecs;

  flecs::system s = ecs.system<位置, const 速度>().run([](flecs::iter &it) {
    std::cout << "Move begin\n";

    while (it.next()) {
      auto p = it.field<位置>(0);
      auto v = it.field<const 速度>(1);

      for (auto i : it) {
        flecs::entity e = it.entity(i);
        p[i].x += v[i].x;
        p[i].y += v[i].y;
        std::cout << e.name() << ": {" << p[i].x << ", " << p[i].y << "}\n";
      }
    }

    std::cout << "Move end\n";
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

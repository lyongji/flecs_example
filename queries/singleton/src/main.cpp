#include <iostream>
#include <singleton.h>

// This example shows how to use singleton components in queries.

// Singleton component
struct Gravity {
  double value;
};

// Entity component
struct 速度 {
  double x;
  double y;
};

int main(int, char *[]) {
  flecs::world world;

  // Mark Gravity as singleton flecs::Singleton
  world.component<Gravity>().add(EcsSingleton);

  // Set singleton
  world.set<Gravity>({9.81});

  // Set Velocity
  world.entity("e1").set<速度>({0, 0});
  world.entity("e2").set<速度>({0, 1});
  world.entity("e3").set<速度>({0, 2});

  // Create query that matches Gravity singleton
  flecs::query<速度, const Gravity> q = world.query<速度, const Gravity>();

  q.each([](flecs::entity e, 速度 &v, const Gravity &g) {
    v.y += g.value;
    std::cout << e.path() << " velocity is {" << v.x << ", " << v.y << "}"
              << std::endl;
  });

  // Output
  //   ::e1 velocity is {0, 9.81}
  //   ::e2 velocity is {0, 10.81}
  //   ::e3 velocity is {0, 11.81}
}

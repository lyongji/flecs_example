#include <iostream>
#include <world_query.h>

struct 位置 {
  double x, y;
};

struct 速度 {
  double x, y;
};

int main(int, char *[]) {
  flecs::world ecs;

  // Create a few test entities for a Position, Velocity query
  ecs.entity("e1").set<位置>({10, 20}).set<速度>({1, 2});

  ecs.entity("e2").set<位置>({10, 20}).set<速度>({3, 4});

  // This entity will not match as it does not have Position, Velocity
  ecs.entity("e3").set<位置>({10, 20});

  // world::each is a quick way to run simple component queries.
  ecs.each([](flecs::entity e, 位置 &p, 速度 &v) {
    p.x += v.x;
    p.y += v.y;
    std::cout << e.name() << ": {" << p.x << ", " << p.y << "}\n";
  });
}

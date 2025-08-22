#include <each_callback.h>
#include <iostream>

// This example shows how to write a query with the each callback, which
// provides the easiest API for iterating over matched components.

struct 位置 {
  double x, y;
};

struct 速度 {
  double x, y;
};

int main(int, char *[]) {
  flecs::world ecs;

  // Create a query for Position, Velocity.
  flecs::query<位置, const 速度> q = ecs.query<位置, const 速度>();

  // Create a few test entities for a Position, Velocity query
  ecs.entity("e1").set<位置>({10, 20}).set<速度>({1, 2});

  ecs.entity("e2").set<位置>({10, 20}).set<速度>({3, 4});

  // This entity will not match as it does not have Position, Velocity
  ecs.entity("e3").set<位置>({10, 20});

  // Arguments passed to each match query type
  q.each([](位置 &p, const 速度 &v) {
    p.x += v.x;
    p.y += v.y;
    std::cout << "{" << p.x << ", " << p.y << "}\n";
  });

  // Output
  //  {11, 22}
  //  {13, 24}
}

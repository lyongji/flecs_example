#include <basics.h>
#include <iostream>

struct 位置 {
  float x;
  float y;
};

int main(int, char *[]) {
  flecs::world ecs;

  // Register component with reflection data
  ecs.component<位置>().member<float>("x").member<float>("y");

  // Create entity with Position as usual
  flecs::entity e = ecs.entity().set<位置>({10, 20});

  // Convert position component to flecs expression string
  const 位置 &p = e.get<位置>();
  std::cout << ecs.to_expr(&p).c_str() << "\n"; // {x: 10, y: 20}
}

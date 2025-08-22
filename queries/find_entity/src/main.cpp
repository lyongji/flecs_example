#include <find_entity.h>
#include <iostream>

struct 位置 {
  int x, y;
};

int main() {
  flecs::world ecs;

  ecs.entity("e1").set<位置>({10, 20});
  ecs.entity("e2").set<位置>({20, 30});

  // Create a simple query for component Position
  flecs::query<位置> q = ecs.query<位置>();

  // Find the entity for which Position.x is 20
  flecs::entity e = q.find([](位置 &p) { return p.x == 20; });

  if (e) {
    std::cout << "Found entity " << e.path() << std::endl;
  } else {
    std::cout << "No entity found" << std::endl;
  }

  // Output
  //  Found entity ::e2
}

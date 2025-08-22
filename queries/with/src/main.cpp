#include <iostream>
#include <with.h>

struct 位置 {
  double x, y;
};

struct Npc {};

int main(int, char *[]) {
  flecs::world ecs;

  // Create a query for Position, Npc. By adding the Npc component using the
  // "with" method, the component is not a part of the query type, and as a
  // result does not become part of the function signatures of each and iter.
  // This is useful for things like tags, which because they don't have a
  // value are less useful to pass to the each/iter functions as argument.
  flecs::query<位置> q = ecs.query_builder<位置>().with<Npc>().build();

  // Create a few test entities for the Position, Npc query
  ecs.entity("e1").set<位置>({10, 20}).add<Npc>();

  ecs.entity("e2").set<位置>({10, 20}).add<Npc>();

  // This entity will not match as it does not have Position, Npc
  ecs.entity("e3").set<位置>({10, 20});

  // Note how the Npc tag is not part of the each signature
  q.each([](flecs::entity e, 位置 &p) {
    std::cout << e.name() << ": {" << p.x << ", " << p.y << "}\n";
  });

  // Output:
  //  e1: {10, 20}
  //  e2: {10, 20}
}

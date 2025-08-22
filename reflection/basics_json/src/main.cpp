#include <basics_json.h>
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
  flecs::entity e = ecs.entity("ent").set<位置>({10, 20});

  // Convert position component to JSON string
  const 位置 &p = e.get<位置>();
  std::cout << ecs.to_json(&p) << "\n"; // {"x":10, "y":20}

  // Convert entity to JSON
  std::cout << e.to_json() << "\n";

  // {
  //     "path":"ent",
  //     "ids":[["Position"]],
  //     "values":[{
  //         "x":10,
  //         "y":20
  //     }]
  // }
}

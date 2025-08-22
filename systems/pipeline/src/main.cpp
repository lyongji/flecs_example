#include <iostream>
#include <pipeline.h>

struct 位置 {
  double x, y;
};

struct 速度 {
  double x, y;
};

int main(int, char *[]) {
  flecs::world ecs;

  // Create a system for moving an entity
  ecs.system<位置, const 速度>()
      .kind(flecs::OnUpdate) // A phase orders a system in a pipeline
      .each([](位置 &p, const 速度 &v) {
        p.x += v.x;
        p.y += v.y;
      });

  // Create a system for printing the entity position
  ecs.system<const 位置>()
      .kind(flecs::PostUpdate)
      .each([](flecs::entity e, const 位置 &p) {
        std::cout << e.name() << ": {" << p.x << ", " << p.y << "}\n";
      });

  // Create a few test entities for a Position, Velocity query
  ecs.entity("e1").set<位置>({10, 20}).set<速度>({1, 2});

  ecs.entity("e2").set<位置>({10, 20}).set<速度>({3, 4});

  // Run the default pipeline. This will run all systems ordered by their
  // phase. Systems within the same phase are ran in declaration order. This
  // function is usually called in a loop.
  ecs.progress();
}

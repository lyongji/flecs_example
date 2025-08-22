#include <iostream>
#include <two_components.h>

// An observer can match multiple components/tags. Only entities that match the
// entire observer query will be forwarded to the callback. For example, an
// observer for Position,Velocity won't match an entity that only has Position.

struct 位置 {
  double x, y;
};

struct 速度 {
  double x, y;
};

int main(int, char *[]) {
  flecs::world ecs;

  // Create observer for custom event
  ecs.observer<位置, 速度>()
      .event(flecs::OnSet)
      .each([](flecs::iter &it, size_t i, 位置 &p, 速度 &v) {
        std::cout << " - " << it.event().name() << ": " << it.event_id().str()
                  << ": " << it.entity(i).name() << ": "
                  << "p: {" << p.x << ", " << p.y << "} "
                  << "v: {" << v.x << ", " << v.y << "}\n";
      });

  // Create entity, set Position (emits EcsOnSet, does not yet match observer)
  flecs::entity e = ecs.entity("e").set<位置>({10, 20});

  // Set Velocity (emits EcsOnSet, matches observer)
  e.set<速度>({1, 2});

  // Output
  //   - OnSet: Velocity: e: p: {10, 20} v: {1, 2}
}

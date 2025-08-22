#include <iostream>
#include <yield_existing.h>

// Observers can enable a "yield_existing" feature that upon creation of the
// observer produces events for all entities that match the observer query. The
// feature is only implemented for the builtin EcsOnAdd and EcsOnSet events.

struct 位置 {
  double x, y;
};

int main(int, char *[]) {
  flecs::world ecs;

  // Create existing entities with Position component
  ecs.entity("e1").set<位置>({10, 20});
  ecs.entity("e2").set<位置>({20, 30});

  // Create an observer for three events
  ecs.observer<位置>()
      .event(flecs::OnSet)
      .yield_existing() // Trigger for existing matching entities
      .each([](flecs::iter &it, size_t i, 位置 &p) {
        std::cout << " - " << it.event().name() << ": " << it.event_id().str()
                  << ": " << it.entity(i).name() << ": {" << p.x << ", " << p.y
                  << "}\n";
      });

  // Output
  //   - OnSet: Position: e1: {10, 20}
  //   - OnSet: Position: e2: {20, 30}
}

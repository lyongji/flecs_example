#include <iostream>
#include <iter_info.h>

struct 位置 {
  double x, y;
};

struct 速度 {
  double x, y;
};

struct 质量 {
  double value;
};

int main(int, char *[]) {
  flecs::world ecs;

  // Create a query for Position, Velocity.
  flecs::query<位置, const 速度> q = ecs.query<位置, const 速度>();

  // Create a few test entities for a Position, Velocity query
  ecs.entity("e1").set<位置>({10, 20}).set<速度>({1, 2});

  ecs.entity("e2").set<位置>({10, 20}).set<速度>({3, 4});

  ecs.entity("e3").set<位置>({10, 20}).set<速度>({4, 5}).set<质量>({50});

  // The run() function provides a flecs::iter object which contains all sorts
  // of information on the entities currently being iterated.
  q.run([&](flecs::iter &it) {
    while (it.next()) {
      auto p = it.field<位置>(0);
      auto v = it.field<const 速度>(1);

      // Print the table & number of entities matched in current callback
      std::cout << "Table [" << it.type().str() << "]" << std::endl;
      std::cout << " - number of entities: " << it.count() << std::endl;

      // Print information about the components being matched
      for (int8_t i = 0; i < it.field_count(); i++) {
        std::cout << " - term " << i << ": " << std::endl;
        std::cout << "   - component: " << it.id(i).str() << std::endl;
        std::cout << "   - type size: " << it.size(i) << std::endl;
      }

      std::cout << std::endl;

      // Iterate entities
      for (auto i : it) {
        p[i].x += v[i].x;
        p[i].y += v[i].y;
        std::cout << " - " << it.entity(i).name() << ": {" << p[i].x << ", "
                  << p[i].y << "}\n";
      }

      std::cout << std::endl;
    }
  });
}

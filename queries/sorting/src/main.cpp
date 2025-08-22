#include <iostream>
#include <sorting.h>

struct 位置 {
  double x, y;
};

// Order by x member of Position
int compare_position(flecs::entity_t e1, const 位置 *p1, flecs::entity_t e2,
                     const 位置 *p2) {
  (void)e1;
  (void)e2;
  return (p1->x > p2->x) - (p1->x < p2->x);
}

// Iterate query, printed values will be ordered
void print_query(flecs::query<const 位置> &q) {
  q.each([](flecs::entity, const 位置 &p) {
    std::cout << "{" << p.x << "," << p.y << "}" << std::endl;
  });
}

int main(int argc, char *argv[]) {
  flecs::world ecs(argc, argv);

  // Create entities, set Position in random order
  flecs::entity e = ecs.entity().set<位置>({1, 0});
  ecs.entity().set<位置>({6, 0});
  ecs.entity().set<位置>({2, 0});
  ecs.entity().set<位置>({5, 0});
  ecs.entity().set<位置>({4, 0});

  // Create a sorted system
  flecs::system sys = ecs.system<const 位置>()
                          .order_by(compare_position)
                          .each([](const 位置 &p) {
                            std::cout << "{" << p.x << "," << p.y << "}"
                                      << std::endl;
                          });

  // Create a sorted query
  flecs::query<const 位置> q =
      ecs.query_builder<const 位置>().order_by(compare_position).build();

  // Iterate query, print values of Position
  std::cout << "-- First iteration\n";
  print_query(q);

  // Change the value of one entity, invalidating the order
  e.set<位置>({7, 0});

  // Iterate query again, printed values are still ordered
  std::cout << "\n-- Second iteration";
  print_query(q);

  // Create new entity to show that data is also sorted for system
  ecs.entity().set<位置>({3, 0});

  // Run system, output will be sorted
  std::cout << "\n-- System iteration";
  sys.run();

  // Output
  //  -- First iteration
  //  {1,0}
  //  {2,0}
  //  {4,0}
  //  {5,0}
  //  {6,0}
  //
  //  -- Second iteration{2,0}
  //  {4,0}
  //  {5,0}
  //  {6,0}
  //  {7,0}
  //
  //  -- System iteration{2,0}
  //  {3,0}
  //  {4,0}
  //  {5,0}
  //  {6,0}
  //  {7,0}
}

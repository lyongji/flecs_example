#include <iostream>
#include <wildcards.h>

// Queries can have wildcard terms that can match multiple instances of a
// relationship or relationship target.

struct 吃 {
  int amount;
};

struct Apples {};
struct Pears {};

int main(int, char *[]) {
  flecs::world ecs;

  // Create a query that matches edible components
  flecs::query<吃> q =
      ecs.query_builder<吃>()
          .term_at(0)
          .second(flecs::Wildcard) // Change first argument to (Eats, *)
          .build();

  // Create a few entities that match the query
  ecs.entity("Bob").set<吃, Apples>({10}).set<吃, Pears>({5});

  ecs.entity("Alice").set<吃, Apples>({4});

  // Iterate the query with a flecs::iter. This makes it possible to inspect
  // the pair that we are currently matched with.
  q.each([](flecs::iter &it, size_t index, 吃 &eats) {
    flecs::entity e = it.entity(index);
    flecs::entity food = it.pair(0).second();

    std::cout << e.name() << " eats " << eats.amount << " " << food.name()
              << std::endl;
  });

  // Output:
  //   Bob eats 10 Apples
  //   Bob eats 5 Pears
  //   Alice eats 4 Apples
}

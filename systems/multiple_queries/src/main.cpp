#include <iostream>
#include <multiple_queries.h>

// This example shows how to pass one or more existing queries to a system,
// which allows for systems that iterate queries multiple times and/or iterate
// multiple queries.

struct 位置 {
  float x, y;
};

struct Bee {
  static const int MaxRange = 10;
  flecs::entity flower;
};

struct Flower {};

float randf(int n) { return static_cast<float>(rand() % n); }

float pow2(float n) { return n * n; }

int main(int, char *[]) {
  flecs::world ecs;

  // Create two queries. Since they'll stick around until the end of the ECS
  // world (or until we delete them) give them a name which makes them easier
  // to find in the explorer.
  // Giving the query a name also causes the caching policy to default to
  // cached. Named queries are (very) likely candidates for being evaluated
  // many times, which is the sweet spot for caching.

  flecs::query<const 位置, Bee> bees = ecs.query<const 位置, Bee>("BeeQuery");

  flecs::query<const 位置> flowers =
      ecs.query_builder<const 位置>("FlowerQuery").with<Flower>().build();

  // Find the closest in range flower for each bee.
  ecs.system("FlowersAndTheBees")
      // Capture queries by value. This prevents common bugs caused by local
      // variables going out of scope before the system runs.
      .run([bees, flowers](flecs::iter &) {
        bees.each([&](flecs::entity bee, const 位置 &p_bee, Bee &b) {
          float dsqr_min = 2 * pow2(Bee::MaxRange);

          b.flower = flecs::entity();

          flowers.each([&](flecs::entity flower, const 位置 &p_flower) {
            float dsqr =
                pow2(p_bee.x - p_flower.x) + pow2(p_bee.y - p_flower.y);

            if (dsqr < dsqr_min) {
              dsqr_min = dsqr;
              b.flower = flower;
            }
          });

          if (b.flower) {
            std::cout << "Bee " << bee.path() << " picked flower "
                      << b.flower.path() << std::endl;
          }
        });
      });

  // Find bees that picked the same flower.
  ecs.system("BumpingBees").run([bees](flecs::iter &) {
    bees.each([&](flecs::entity bee1, const 位置 &, Bee &b1) {
      bees.each([&](flecs::entity bee2, const 位置 &, Bee &b2) {
        if (bee1 > bee2) { // Don't handle the same collision twice
          if (b1.flower && b1.flower == b2.flower) {
            std::cout << "Bee " << bee1.path() << " and bee " << bee2.path()
                      << " bumped into each other" << std::endl;
          }
        }
      });
    });
  });

  // Create flowers and bees
  for (int i = 0; i < 5; i++) {
    ecs.entity()
        .add<Bee>() // Add with default value
        .set(位置{randf(20), randf(20)});
  }

  for (int i = 0; i < 10; i++) {
    ecs.entity().add<Flower>().set(位置{randf(20), randf(20)});
  }

  ecs.progress();

  // Output
  //  Bee #544 picked flower #552
  //  Bee #545 picked flower #555
  //  Bee #546 picked flower #552
  //  Bee #547 picked flower #555
  //  Bee #548 picked flower #554
  //  Bee #546 and bee #544 bumped into each other
  //  Bee #547 and bee #545 bumped into each other
}

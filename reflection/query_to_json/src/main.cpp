#include <iostream>
#include <query_to_json.h>

struct 位置 {
  float x;
  float y;
};

struct 速度 {
  float x;
  float y;
};

struct 质量 {
  float value;
};

int main(int, char *[]) {
  flecs::world ecs;

  // Register components with reflection data
  ecs.component<位置>().member<float>("x").member<float>("y");

  ecs.component<速度>().member<float>("x").member<float>("y");

  ecs.component<质量>().member<float>("value");

  ecs.entity("a").set<位置>({10, 20}).set<速度>({1, 2});
  ecs.entity("b").set<位置>({20, 30}).set<速度>({2, 3});
  ecs.entity("c").set<位置>({30, 40}).set<速度>({3, 4}).set<质量>({10});
  ecs.entity("d").set<位置>({30, 40}).set<速度>({4, 5}).set<质量>({20});

  // Query for components
  flecs::query<位置, const 速度> q = ecs.query<位置, const 速度>();

  // Serialize query to JSON. Note that this works for any iterable object.
  std::cout << q.iter().to_json().c_str() << "\n";

  // Iterator returns 2 sets of results, one for each table.

  // {
  //   "ids": ["Position", "Velocity"],
  //   "results": [{
  //     "ids": ["Position", "Velocity"],
  //     "sources": [0, 0],
  //     "is_set": [true, true],
  //     "entities": ["a", "b"],
  //     "values": [
  //       [{
  //         "x": 10.00,
  //         "y": 20.00
  //       }, {
  //         "x": 20.00,
  //         "y": 30.00
  //       }],
  //       [{
  //         "x": 1.00,
  //         "y": 2.00
  //       }, {
  //         "x": 2.00,
  //         "y": 3.00
  //       }]
  //     ]
  //   }, {
  //     "ids": ["Position", "Velocity"],
  //     "sources": [0, 0],
  //     "is_set": [true, true],
  //     "entities": ["c", "d"],
  //     "values": [
  //       [{
  //         "x": 30.00,
  //         "y": 40.00
  //       }, {
  //         "x": 30.00,
  //         "y": 40.00
  //       }],
  //       [{
  //         "x": 3.00,
  //         "y": 4.00
  //       }, {
  //         "x": 4.00,
  //         "y": 5.00
  //       }]
  //     ]
  //   }]
  // }
}

#include <iostream>
#include <query_to_custom_json.h>

// Same example as query_to_json, but with customized serializer parameters

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

  // Serialize query to JSON. Only serialize entity names.
  flecs::iter_to_json_desc_t desc = ECS_ITER_TO_JSON_INIT;
  desc.serialize_fields = false;
  std::cout << q.iter().to_json(&desc).c_str() << "\n";

  // Iterator returns 2 sets of results, one for each table.

  // {
  //   "results": [
  //     {
  //       "name": "a"
  //     },
  //     {
  //       "name": "b"
  //     },
  //     {
  //       "name": "c"
  //     },
  //     {
  //       "name": "d"
  //     }
  //   ]
  // }
}

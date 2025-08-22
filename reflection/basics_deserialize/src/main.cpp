#include <basics_deserialize.h>
#include <iostream>

struct 位置 {
  float x;
  float y;
};

int main(int, char *[]) {
  flecs::world ecs;

  // Register component with reflection data
  ecs.component<位置>().member<float>("x").member<float>("y");

  // Create entity, set value of position using reflection API
  flecs::entity e = ecs.entity();
  位置 &ptr = e.ensure<位置>();

  flecs::cursor cur = ecs.cursor<位置>(&ptr);
  cur.push();          // {
  cur.set_float(10.0); //   10
  cur.next();          //   ,
  cur.set_float(20.0); //   20
  cur.pop();           // }

  std::cout << ecs.to_expr(&ptr).c_str() << "\n"; // {x: 10.00, y: 20.00}

  // Use member names before assigning values
  cur = ecs.cursor<位置>(&ptr);
  cur.push();        // {
  cur.member("y");   //   y:
  cur.set_float(10); //   10
  cur.member("x");   //   x:
  cur.set_float(20); //   20
  cur.pop();         // }

  std::cout << ecs.to_expr(&ptr).c_str() << "\n"; // {x: 20.00, y: 10.00}
}

#include <enqueue_entity_event.h>
#include <iostream>

// 这与entity_event的示例相同，但此示例使用enqueue()方法而不是emit()方法。
// 与emit()同步调用观察者不同，enqueue()将事件添加到命令队列，
// 这会延迟调用观察者，直到下一次刷新命令队列时才会调用。

// 一个没有负载的事件
struct Click {};

// 一个有负载的事件
struct Resize {
  double width, height;
};

int main(int, char *[]) {
  flecs::world ecs;

  // 创建一个widget实体
  flecs::entity widget = ecs.entity("MyWidget");

  // 在widget实体上观察Click事件。
  widget.observe<Click>([]() { std::cout << "clicked!\n"; });

  // 观察者可以有一个实体参数，表示事件的来源。
  // 这允许相同的函数在不同的实体上重用。
  widget.observe<Click>([](flecs::entity src) {
    std::cout << "clicked on " << src.path() << "!\n";
  });

  // 观察Resize事件。有负载的事件会作为参数传递给观察者回调函数。
  widget.observe([](Resize &p) {
    std::cout << "resized to {" << p.width << ", " << p.height << "}!\n";
  });

  // 只有在世界处于延迟模式下时，才能调用enqueue事件。
  ecs.defer_begin();

  // 发出Click事件
  widget.enqueue<Click>();

  // 发出Resize事件
  widget.enqueue<Resize>({100, 200});

  std::cout << "Events enqueued!\n";

  // 刷新队列并调用观察者
  ecs.defer_end();

  // 输出
  //   Events enqueued!
  //   clicked!
  //   clicked on ::MyWidget!
  //   resized to {100, 200}!
}

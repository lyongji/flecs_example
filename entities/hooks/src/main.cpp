#include <hooks.h>
#include <iostream>

// 组件钩子是在组件生命周期的不同部分可以注册的回调函数。
// 这些回调函数在C++ API中自动注册为组件钩子。
//
// 在C++ API中，类型的构造函数、析构函数、拷贝构造函数和移动构造函数
// 自动注册为组件钩子。

struct String {
    String(const char *v = nullptr) {
        flecs::log::trace("Ctor(const char*)"); // 构造函数(const char*)
        value = ecs_os_strdup(v);
    }

    ~String() {
        flecs::log::trace("Dtor"); // 析构函数
        ecs_os_free(value);
    }

    String(const String& obj) {
        flecs::log::trace("Copy"); // 拷贝构造函数
        value = ecs_os_strdup(obj.value);
    }

    String(String&& obj) {
        flecs::log::trace("Move"); // 移动构造函数
        value = obj.value;
        obj.value = nullptr;
    }

    String& operator=(const String& obj) {
        flecs::log::trace("Copy assign"); // 拷贝赋值运算符
        ecs_os_free(value);
        value = ecs_os_strdup(obj.value);
        return *this;
    }

    String& operator=(String&& obj) {
        flecs::log::trace("Move assign"); // 移动赋值运算符
        ecs_os_free(value);
        value = obj.value;
        obj.value = nullptr;
        return *this;
    }

    char *value;
};

struct Tag { }; // 标签结构体

int main(int, char *[]) {
    flecs::world ecs;

    // 为类型注册额外的添加、移除和设置钩子
    ecs.component<String>()
        .on_add([](flecs::entity e, String&) {
            flecs::log::trace("OnAdd: %s", e.name().c_str()); // 添加钩子
        })
        .on_remove([](flecs::entity e, String&) {
            flecs::log::trace("OnRemove: %s", e.name().c_str()); // 移除钩子
        })
        .on_set([](flecs::entity e, String&) {
            flecs::log::trace("OnSet: %s", e.name().c_str()); // 设置钩子
        });

    // 预先注册以保持输出跟踪的清洁
    ecs.component<Tag>();

    flecs::log::set_level(0);

    flecs::entity e = ecs.entity("Entity");

    flecs::log::push("e.add<String>()");
    e.add<String>();
    flecs::log::pop();

    flecs::log::push("e.set<String>({\"Hello World\"})");
    e.set<String>({"Hello World"});
    flecs::log::pop();

    // 此操作会更改实体的原型，这将调用移动操作
    flecs::log::push("e.add<Tag>()");
    e.add<Tag>();
    flecs::log::pop();

    flecs::log::push("e.destruct()");
    e.destruct();
    flecs::log::pop();

    flecs::log::set_level(-1);

    // 输出:
    //  info: e.add<String>()
    //  info: | Ctor
    //  info: | OnAdd: Entity
    //  info: e.set<String>({"Hello World"})
    //  info: | Ctor
    //  info: | Move assign
    //  info: | OnSet: Entity
    //  info: | Dtor
    //  info: e.add<Tag>()
    //  info: | Move
    //  info: | Dtor
    //  info: e.destruct()
    //  info: | OnRemove: Entity
    //  info: | Dtor
}


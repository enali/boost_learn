//
// Created by enali on 3/21/2020.
//

#include "common.hpp"
#include <boost/pool/pool.hpp>
#include <boost/pool/object_pool.hpp>
#include <boost/pool/singleton_pool.hpp>

using namespace std;

void test_pool() {
    boost::pool<> pl(sizeof(int));
    int *p = static_cast<int *>(pl.malloc());  // 分配内存, void *
    assert(pl.is_from(p));  // 判断内存是否从pl分布

    pl.free(p);  // 通常不需要手动free
    for (int i = 0; i < 100; ++i) {
        pl.ordered_malloc(10);  // 分配的同时合并空闲块链表
    }
}

struct demo_class {
public:
    int a, b, c;

    explicit demo_class(int x = 1, int y = 2, int z = 3) : a(x), b(y), c(z) {}
};

void test_object_pool() {
    boost::object_pool<demo_class> pl;
    demo_class *p = pl.malloc();  // 分配原始内存, 不构造实例
    assert(pl.is_from(p));

    assert(p->a != 1 || p->b != 2 || p->c != 3);

    p = pl.construct(7, 8, 9);  // 分配对象, 并构造实例, 最多3个参数
    assert(p->a == 7);

    boost::object_pool<string> pls;
    for (int i = 0; i < 10; ++i) {
        string *ps = pls.construct("hello, object_pool");
        cout << *ps << endl;
    }
}

struct pool_tag {
};

void test_singleton_pool() {
    using spl = boost::singleton_pool<pool_tag, sizeof(int)>;  // 别名简化
    int *p = static_cast<int *>(spl::malloc());
    assert(spl::is_from(p));
    spl::release_memory();  // 释放所有未被分配的内存, 已分配的不受影响
}

int main() {
    test_pool();
    test_object_pool();
    test_singleton_pool();
}
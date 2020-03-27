//
// Created by enali on 3/21/2020.
//

#include "common.hpp"
#include <boost/noncopyable.hpp>
#include <boost/core/ignore_unused.hpp>
#include <boost/optional.hpp>
#include <boost/logic/tribool.hpp>
#include <boost/logic/tribool_io.hpp>
#include <boost/operators.hpp>
#include <boost/uuid/uuid.hpp>
#include <boost/uuid/uuid_io.hpp>
#include <boost/uuid/uuid_generators.hpp>

// noncopyable没有拷贝构造和拷贝赋值, 因此, 其所有子类也无法拷贝
class do_not_copy : boost::noncopyable {
};

void test_noncopyable() {
    do_not_copy d1;
//    do_not_copy d2(d1);  // 拷贝构造
    do_not_copy d3;
//    d3 = d1;  // 拷贝赋值
}

void test_ignore_unused() {
    int i = 3;
    boost::ignore_unused(i);  // 忽略无用的变量定义

    using vint = std::vector<int>;
    boost::ignore_unused<vint>();  // 忽略无用的类型定义
}

boost::optional<double> calc(int x) {
    return {x != 0, 1.0 / x};  // 初始化列表
}

boost::optional<double> sqrt_op(double x) {
    return {x >= 0, std::sqrt(x)};
}

void test_optional() {
    boost::optional<int> op0;  // 未初始化
    boost::optional<int> op1(boost::none);

    assert(!op0);
    assert(op0 == op1);

    assert(op1.value_or(253) == 253);  // 有则取值, 无则253
    std::cout << op1.value_or_eval([] { return 874; }) << std::endl;

    boost::optional<std::string> ops("test");
    std::cout << *ops << std::endl;  // 类似指针, 解引用

    ops.emplace("monado", 3);  // 用字串的前3个字符初始化string
    assert(*ops == "mon");

    std::vector<int> v(10);
    boost::optional<std::vector<int> &> opv(v);
    assert(opv);

    opv->push_back(5);  // 通过->代理操作
    assert(opv->size() == 11);

    opv = boost::none;
    assert(!opv);

    boost::optional<double> d = calc(10);
    if (d) {
        std::cout << *d << std::endl;
    }

    d = sqrt_op(-10);
    if (!d) {
        std::cout << "no result" << std::endl;
    }
}

void test_tribool() {
    using namespace boost::logic;
    tribool tb(true);  // true, false, indeterminate
    tribool tb2(!tb);
    tribool tb3 = indeterminate;

    std::cout << "true is: " << tb << std::endl;
    std::cout << "false is: " << tb2 << std::endl;
    std::cout << "indeterminate is: " << tb3 << std::endl;

    if (tb) {
        std::cout << "true" << std::endl;
    }
    tb2 = indeterminate;
    assert(indeterminate(tb2));
    std::cout << std::boolalpha << tb2 << std::endl;

    if (tb2 == indeterminate) {  // 不会执行, 只能通过indeterminate函数判断
        std::cout << "indeterminate" << std::endl;
    }
    if (indeterminate(tb2)) {
        std::cout << "indeterminate2" << std::endl;
    }
    std::cout << (tb2 || true) << std::endl;
    std::cout << (tb2 && false) << std::endl;
}

// 实现<, 自动实现>, >=, <=
// 实现==, 自动实现!=
// 基类链
class point : boost::less_than_comparable<point, boost::equality_comparable<point>> {
    int x, y, z;
public:
    explicit point(int a = 0, int b = 0, int c = 0) : x(a), y(b), z(c) {}

    void print() const {
        std::cout << x << ", " << y << ", " << z << std::endl;
    }

    friend bool operator<(const point &l, const point &r) {
        return (l.x * l.x + l.y * l.y + l.z * l.z < r.x * r.x + r.y * r.y + r.z * r.z);
    }

    friend bool operator==(const point &l, const point &r) {
        return r.x == l.x && r.y == l.y && r.z == l.z;
    }
};

void test_operators() {
    point p0, p1(1, 2, 3), p2(3, 0, 5), p3(3, 2, 1);
    assert(p0 < p1 && p1 < p2);
    assert(p2 > p0);
    assert(p1 <= p3);
    assert(!(p1 < p3) && !(p1 > p3));
    assert(p0 != p1);
}

void test_uuid() {
    using namespace boost::uuids;
    uuid u;  // 可以视为16元素的unsign char的数组
    std::cout << "u is nil? " << u.is_nil() << std::endl;
    std::cout << u << std::endl;  // 非全零
    assert(uuid::static_size() == 16);
    assert(u.size() == 16);

    std::vector<unsigned char> v(16, 32);
    std::copy(v.begin(), v.end(), u.begin());
    assert(u.data[0] == u.data[1] && u.data[15] == 32);

    std::cout << u << std::endl;  // 把数组输出为十六进制, uint8_t转为十六进制, 占2位
    std::fill_n(u.data + 10, 6, 8);

    std::cout << u << std::endl;

    std::cout << u.variant() << std::endl;  // 变体号, 用于表示布局类型
    std::cout << u.version() << std::endl;  // 使用的生成算法

    std::memset(u.data, 0, 16);
    std::cout << "u is nil? " << u.is_nil() << std::endl;

    // nil生成器
    uuid uu = nil_generator()();  // 先实例化得到函数对象, 再调用生成uuid
    assert(uu.is_nil());
    uu = nil_uuid();  // 内联函数
    assert(uu.is_nil());

    // 字符串生成器
    string_generator sgen;
    uuid u1 = sgen("0123456789abcdef0123456789abcdef");
    std::cout << u1 << std::endl;
    uuid u2 = sgen("01234567-89ab-cdef-0123-456789abcdef");
    std::cout << u2 << std::endl;
    uuid u3 = sgen(L"{01234567-89ab-cdef-0123-456789abcdef}");
    std::cout << u3 << std::endl;
    assert(u3.version() == uuid::version_unknown);

    // 名字生成器
    uuid www_xxx_com = string_generator()("0123456789abcdef0123456789abcdef");
    name_generator ngen(www_xxx_com);
    uuid uu1 = ngen("mario");
    assert(!uu1.is_nil() && uu1.version() == uuid::version_name_based_sha1);
    uuid uu2 = ngen("links");
    std::cout << uu2 << std::endl;
    uuid uu3 = ngen("links");
    assert(uu2 == uu3);  // 同一name生成的uuid相同

    // 随机生成器
    random_generator rgen;
    uuid uu4 = rgen();
    assert(uu4.version() == uuid::version_random_number_based);
    std::cout << uu4 << std::endl;

    std::string uuid_str = to_string(u3);
    std::cout << uuid_str << std::endl;

    detail::sha1 sha;
    const char *szMsg = "hello, world";
    sha.process_block(szMsg, szMsg + strlen(szMsg));
    unsigned int digest[5];
    sha.get_digest(digest);
    for (int i = 0; i < 5; ++i) {
        std::cout << std::hex << digest[i];
    }
}

int main() {
    test_noncopyable();
    test_ignore_unused();
    test_optional();
    test_tribool();
    test_operators();
    test_uuid();
}
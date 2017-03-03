#include<cassert>
#include<iostream>
#include<memory>
#include<experimental/memory_resource> // Not in g++ yet
#include<tuple>
#include<optional>
#include<variant>
#include<vector>
#include<scoped_allocator>
#include<functional>
#include<chrono>
#include<string_view>
#include<locale>
#include<codecvt>
#include<type_traits>
#include<array>
#include<deque>
#include<forward_list>
#include<list>
#include<map>
#include<unordered_map>
#include<set>
#include<unordered_set>
#include<queue>
#include<stack>
#include<iterator>
#include<complex>
#include<random>
#include<valarray>
#include<sstream>
#include<fstream>
#include<regex>
#include<atomic>
#include<thread>
#include<mutex>
#include<shared_mutex>
#include<future>

#include<typeinfo>
#include<cxxabi.h>
using namespace std;
using namespace __cxxabiv1;

template<typename T>
struct X {};
template<typename T> struct S { S(T t) {} };
// template<typename T> S(T, int = 7) -> S<T>; // Why doesn't this work

template<typename T> struct my_hash {
  bool operator()(const T &t) const { return hash<T>()(t); }
};

template<typename T> struct my_pred {
  bool operator()(const T &t1, const T &t2) const { return equal_to<T>()(t1, t2); }
};


int r; // For __cxa_demangle
template<typename T>
struct A {
  int foo() { return 7; }
  int bar(int i) noexcept { return i; }
  A() {}
  A(T t) : t{t} {}
  using pointer = T *;
  T t;
};

bool operator !=(const A<int> &, nullptr_t) { return true; }

struct B : public A<int> {};

struct HasExplicitCopyConstructor
{
  HasExplicitCopyConstructor() {}
  explicit HasExplicitCopyConstructor(const HasExplicitCopyConstructor &) {}
};

template<typename T> struct IL {  IL(initializer_list<T> il) {}};
void test_initializer_list()
{
  IL<int> il = { 1, 2, 3}; // Works
  // IL il2 = { 1, 2, 3};     // Error: substitution failed. Jason says in process of being fixed
  IL il3({1, 2, 3});       // Works
  static_assert(is_same_v<decltype(il3), IL<int>>);
}

void test_pair() // Explicit  20.4
{
  // Some odd seeming tests are to make sure N4387 is not causing any trouble
  pair p(3, 7.5);  // explicit (Don't label most of the following as they use same constructor)
  int r;
  cout << __cxa_demangle(typeid(p).name(), nullptr, nullptr, &r) << endl;
  static_assert(is_same_v<decltype(p), pair<int, double>>);
  pair p2 = { 10, -15};   // See N4387
  static_assert(is_same_v<decltype(p2), pair<int, int>>);
  pair p3 = { A(5), 7.2 }; 
  static_assert(is_same_v<decltype(p3), pair<A<int>, double>>);
  int ia[] = {1, 2, 3};
  pair p4 = {ia, 2};
  static_assert(is_same_v<decltype(p4), pair<int *, int>>);
  pair p5 = { unique_ptr(new int), 2.2 }; // Check for non-copyable movable types
  static_assert(is_same_v<decltype(p5), pair<unique_ptr<int>, double>>);
  pair p6 = p4; // implicit
  static_assert(is_same_v<decltype(p6), decltype(p4)>);
  pair p7 = move(p5);   //  implicit. p7 and p8 test pair's move constructor
  static_assert(is_same_v<decltype(p7), decltype(p5)>);
  pair p8{ HasExplicitCopyConstructor(), 2 };
  static_assert(is_same_v<decltype(p8), pair<HasExplicitCopyConstructor, int>>);
}


void test_tuple()  // Explicit 20.5
{
  // Some odd seeming tests are to make sure N4387 is not causing any trouble
  tuple p1(3, 7.5, 1); // explicit
  static_assert(is_same_v<decltype(p1), tuple<int, double, int>>);
  tuple p2 = { 10, -15, 2};   // See N4387
  static_assert(is_same_v<decltype(p2), tuple<int, int, int>>);
  tuple p3 = { A(5), 7.2, 1 };
  static_assert(is_same_v<decltype(p3), tuple<A<int>, double, int>>);
  int ia[] = {1, 2, 3};
  tuple p4 = {ia, 2, 3};
  static_assert(is_same_v<decltype(p4), tuple<int *, int, int>>);
  tuple p5 = { unique_ptr(new int), 2.2, 1 }; // Check for non-copyable movable types
  static_assert(is_same_v<decltype(p5), tuple<unique_ptr<int>, double, int>>);
  tuple p6 = p4;
  static_assert(is_same_v<decltype(p6), decltype(p4)>);
  tuple p7 = move(p5);   // Test tuple's move constructor
  static_assert(is_same_v<decltype(p7), decltype(p5)>);
  tuple p8{ HasExplicitCopyConstructor(), 2, 1 };
  static_assert(is_same_v<decltype(p8), tuple<HasExplicitCopyConstructor, int, int>>);
  pair p = {3, 5.2};
  tuple p9{p}; // explicit
  static_assert(is_same_v<decltype(p9), tuple<int, double>>);
  tuple p10{pair(3, 5.2)};
  static_assert(is_same_v<decltype(p10), tuple<int, double>>);
  tuple p11{};
  static_assert(is_same_v<decltype(p11), tuple<>>);
  
  tuple pa1(allocator_arg, allocator<int>(), 3, 7.5, 1); // explicit
  static_assert(is_same_v<decltype(pa1), tuple<int, double, int>>);
  tuple pa2 = {allocator_arg, allocator<int>(), 10, -15, 2};   // See N4387
  static_assert(is_same_v<decltype(pa2), tuple<int, int, int>>);
  tuple pa3 = {allocator_arg, allocator<int>(), A(5), 7.2, 1 };
  static_assert(is_same_v<decltype(pa3), tuple<A<int>, double, int>>);
  tuple pa4 = {allocator_arg, allocator<int>(), ia, 2, 3};
  static_assert(is_same_v<decltype(pa4), tuple<int *, int, int>>);
  tuple pa5 = {allocator_arg, allocator<int>(), unique_ptr(new int), 2.2, 1 }; // Check for non-copyable movable types
  static_assert(is_same_v<decltype(pa5), tuple<unique_ptr<int>, double, int>>);
  tuple pa6 = {allocator_arg, allocator<int>(), pa4};
  static_assert(is_same_v<decltype(pa6), decltype(pa4)>);
  tuple pa7 = {allocator_arg, allocator<int>(), move(pa5)};   // Test tuple's move constructor
  static_assert(is_same_v<decltype(pa7), decltype(pa5)>);
  tuple pa8{allocator_arg, allocator<int>(), HasExplicitCopyConstructor(), 2, 1 };
  static_assert(is_same_v<decltype(pa8), tuple<HasExplicitCopyConstructor, int, int>>);
  tuple pa9{allocator_arg, allocator<int>(), p}; // explicit
  static_assert(is_same_v<decltype(pa9), tuple<int, double>>);
  tuple pa10{allocator_arg, allocator<int>(), pair(3, 5.2)}; // explicit
  static_assert(is_same_v<decltype(pa10), tuple<int, double>>);
  tuple pa11{allocator_arg, allocator<int>()}; 
  static_assert(is_same_v<decltype(p11), tuple<>>);
}

void test_optional() // Explicit  20.6
{
  optional o(7);
  static_assert(is_same_v<decltype(o), optional<int>>);
  optional o2 = o;
  static_assert(is_same_v<decltype(o2), optional<int>>);
  optional o3(optional("foo"));
  static_assert(is_same_v<decltype(o3), optional<const char *>>);
  // optional o4(in_place); // Expect compile error
}

// Bizarre deleter to help touch all the bases
struct FancyDeleter {
  using pointer = A<int>;
  void operator()(pointer & i) { cout << "deleting i" << endl; }
  void operator()(B *i) { cout << "deleting i for shared pointer test" << endl; }
};

// Test handling of pointer typedef
template<typename T> struct def_del_noptr : public default_delete<T> {};
template<typename T> struct def_del_ptr : public default_delete<T> { using pointer = T *; };

void test_unique_ptr() // Implicit (after changes suggested by STL
{
  unique_ptr up(new A(3));
  static_assert(is_same_v<decltype(up), unique_ptr<A<int>>>);
  unique_ptr up2(move(up));
  static_assert(is_same_v<decltype(up2), unique_ptr<A<int>>>);
  unique_ptr up3(unique_ptr(new int));
  static_assert(is_same_v<decltype(up3), unique_ptr<int>>);
  unique_ptr up4(new B(), default_delete<A<int>>());
  static_assert(is_same_v<decltype(up4), unique_ptr<A<int>, default_delete<A<int>>>>);
  unique_ptr up5(new int[5], default_delete<int[]>());
  static_assert(is_same_v<decltype(up5), unique_ptr<int[]>>);
  unique_ptr up6{A<int>(), FancyDeleter()};
  static_assert(is_same_v<decltype(up6), unique_ptr<int, FancyDeleter>>);
  unique_ptr up7{new B(), def_del_noptr<A<int>>()};
  static_assert(is_same_v<decltype(up7), unique_ptr<B, def_del_noptr<A<int>>>>);
  unique_ptr up8{new B(), def_del_ptr<A<int>>()};
  static_assert(is_same_v<decltype(up8), unique_ptr<A<int>, def_del_ptr<A<int>>>>);
}

void test_shared_ptr() // Explicit
{
  shared_ptr sp1(new int);  // explicit
  static_assert(is_same_v<decltype(sp1), shared_ptr<int>>);
  // shared_ptr<int> sp1e = new int; // Expect compile error because shared_ptr<T>::shared_ptr(U*) is explicit
  shared_ptr sp2(sp1);
  static_assert(is_same_v<decltype(sp2), decltype(sp1)>);
  shared_ptr sp3(move(sp1));
  static_assert(is_same_v<decltype(sp3), decltype(sp1)>);
  shared_ptr sp4{new B(), default_delete<A<int>>()};
  static_assert(is_same_v<decltype(sp4), shared_ptr<A<int>>>);
  shared_ptr sp5(new int[5], default_delete<int[]>());
  static_assert(is_same_v<decltype(sp5), shared_ptr<int[]>>);
  shared_ptr sp6{new B(), FancyDeleter()};
  static_assert(is_same_v<decltype(sp6), shared_ptr<B>>);
  shared_ptr sp7{new B(), default_delete<A<int>>(), allocator<A<int>>()};
  static_assert(is_same_v<decltype(sp7), shared_ptr<A<int>>>);
  shared_ptr sp8(new int[5], default_delete<int[]>(), allocator<int>());
  static_assert(is_same_v<decltype(sp8), shared_ptr<int[]>>);
  shared_ptr sp9{new B(), FancyDeleter(), allocator<B>()};
  static_assert(is_same_v<decltype(sp9), shared_ptr<B>>);
  int *ip = new int{};
  shared_ptr sp10(sp9, ip);
  static_assert(is_same_v<decltype(sp10), shared_ptr<int>>);
  weak_ptr wp(sp10);
  shared_ptr sp11(wp);
  static_assert(is_same_v<decltype(sp11), shared_ptr<int>>);
  // shared_ptr sp11e = wp; // Error expected because shared_ptr<T>::shared_ptr(weak_ptr<U>) is explicit
  shared_ptr sp12(unique_ptr(new int));
  static_assert(is_same_v<decltype(sp12), shared_ptr<int>>);
}

void test_weak_ptr() //
{
  shared_ptr sp(new int);
  weak_ptr wp1(sp);  // explicit
  static_assert(is_same_v<decltype(wp1), weak_ptr<int>>);
  weak_ptr wp2(wp1);  // implicit
  static_assert(is_same_v<decltype(wp2), decltype(wp1)>);
  weak_ptr wp3(move(wp1));  // implicit
  static_assert(is_same_v<decltype(wp3), decltype(wp1)>);
}

void test_owner_less() // Implicit
{
  owner_less ol{};  // implicit
  static_assert(is_same_v<decltype(ol), owner_less<>>);
  owner_less<shared_ptr<int>> oli;
  owner_less ol2 = oli; // implicit
  static_assert(is_same_v<decltype(ol2), decltype(oli)>);
  owner_less ol3{owner_less<shared_ptr<int>>{}};  // implicit
  static_assert(is_same_v<decltype(ol3), owner_less<shared_ptr<int>>>);
}

void test_polymorphic_allocator() { // Implicit
  experimental::pmr::memory_resource *mr = experimental::pmr::new_delete_resource();
  experimental::pmr::polymorphic_allocator<int> pai(mr);
  experimental::pmr::polymorphic_allocator pai1(pai); // implicit. Only copy constructor to test
  static_assert(is_same_v<decltype(pai1), decltype(pai)>);
}

void test_scoped_allocator_adaptor() // Explicit
{
  scoped_allocator_adaptor sai1{allocator<vector<string>>(), allocator<char>()}; // explicit
  static_assert(is_same_v<decltype(sai1), scoped_allocator_adaptor<allocator<vector<string>>, allocator<char>>>);
  scoped_allocator_adaptor sai2 = sai1; // implicit
  static_assert(is_same_v<decltype(sai2), decltype(sai1)>);
  scoped_allocator_adaptor sai3(move(sai1)); // implicit
  static_assert(is_same_v<decltype(sai3), decltype(sai1)>);
}

void test_reference_wrapper() // Implicit
{
  int i;
  reference_wrapper rw1(i); // implicit
  static_assert(is_same_v<decltype(rw1), reference_wrapper<int>>);
  // reference_wrapper rw1m(move(i)); // Expect compile error due to deleted constructor
  reference_wrapper rw2 = rw1; // implicit
  static_assert(is_same_v<decltype(rw2), decltype(rw1)>);
}

void test_arithmetic_operations() // 20.14.6
{
  // class plus: Implicit
  plus p1{};  // implicit
  static_assert(is_same_v<decltype(p1), plus<>>);
  plus p2  = p1; // implicit
  static_assert(is_same_v<decltype(p2), decltype(p1)>);

  // class minus: Implicit
  minus m1{};  // implicit 
  static_assert(is_same_v<decltype(m1), minus<>>);
  minus m2 = m1; // implicit
  static_assert(is_same_v<decltype(m2), decltype(m1)>);

  // class multiplies: Implicit
  multiplies mu1{};  // implicit
  static_assert(is_same_v<decltype(mu1), multiplies<>>);
  multiplies mu2  = mu1; // implicit
  static_assert(is_same_v<decltype(mu2), decltype(mu1)>);

  // class divides: Implicit
  divides d1{};  // implicit
  static_assert(is_same_v<decltype(d1), divides<>>);
  divides d2  = d1; // implicit
  static_assert(is_same_v<decltype(d2), decltype(d1)>);

  // class modulus: Implicit
  modulus mo1{};  // implicit
  static_assert(is_same_v<decltype(mo1), modulus<>>);
  modulus mo2  = mo1; // implicit
  static_assert(is_same_v<decltype(mo2), decltype(mo1)>);

  // class negate: Implicit
  negate n1{};  // implicit
  static_assert(is_same_v<decltype(n1), negate<>>);
  negate n2 = n1; // implicit
  static_assert(is_same_v<decltype(n2), decltype(n1)>);  
}

void test_comparisons() // 20.14.7
{
  // class equal_to: Implicit
  equal_to et1{};  // implicit
  static_assert(is_same_v<decltype(et1), equal_to<>>);
  equal_to et2  = et1; // implicit
  static_assert(is_same_v<decltype(et2), decltype(et1)>);

  // class not_equal_to: Implicit
  not_equal_to net1{};  // implicit
  static_assert(is_same_v<decltype(net1), not_equal_to<>>);
  not_equal_to net2  = net1; // implicit
  static_assert(is_same_v<decltype(net2), decltype(net1)>);

  // class greater: Implicit
  greater g1{};  // implicit
  static_assert(is_same_v<decltype(g1), greater<>>);
  greater g2  = g1; // implicit
  static_assert(is_same_v<decltype(g2), decltype(g1)>);

  // class less : Implicit
  less l1{};  // implicit
  static_assert(is_same_v<decltype(l1), less<>>);
  less l2  = l1; // implicit
  static_assert(is_same_v<decltype(l2), decltype(l1)>);

  // class greater_equal: Implicit
  greater_equal ge1{};  // implicit
  static_assert(is_same_v<decltype(ge1), greater_equal<>>);
  greater_equal ge2  = ge1; // implicit
  static_assert(is_same_v<decltype(ge2), decltype(ge1)>);

  // class less_equal: Implicit
  less_equal le1{};  // implicit
  static_assert(is_same_v<decltype(le1), less_equal<>>);
  less_equal le2  = le1; // implicit
  static_assert(is_same_v<decltype(le2), decltype(le1)>);
}

void test_logical_operations() // 20.14.8
{
  // class logical_and: Implicit
  logical_and la1{};  // implicit
  static_assert(is_same_v<decltype(la1), logical_and<>>);
  logical_and la2  = la1; // implicit
  static_assert(is_same_v<decltype(la2), decltype(la1)>);

  // class logical_or: Implicit
  logical_or lo1{};  // implicit
  static_assert(is_same_v<decltype(lo1), logical_or<>>);
  logical_or lo2  = lo1; // implicit
  static_assert(is_same_v<decltype(lo2), decltype(lo1)>);

  // class logical_not: Implicit
  logical_not ln1{};  // implicit
  static_assert(is_same_v<decltype(ln1), logical_not<>>);
  logical_not ln2  = ln1; // implicit
  static_assert(is_same_v<decltype(ln2), decltype(ln1)>);
}

void test_bit_operations() // 20.14.9
{
  // class bit_and: Implicit
  bit_and ba1{};  // implicit
  static_assert(is_same_v<decltype(ba1), bit_and<>>);
  bit_and ba2  = ba1; // implicit
  static_assert(is_same_v<decltype(ba2), decltype(ba1)>);

  // class bit_or: Implicit
  bit_or bo1{};  // implicit
  static_assert(is_same_v<decltype(bo1), bit_or<>>);
  bit_or bo2  = bo1; // implicit
  static_assert(is_same_v<decltype(bo2), decltype(bo1)>);

  // class bit_xor: Implicit
  bit_xor bx1{};  // implicit
  static_assert(is_same_v<decltype(bx1), bit_xor<>>);
  bit_xor bx2  = bx1; // implicit
  static_assert(is_same_v<decltype(bx2), decltype(bx1)>);

  // class bit_not: Implicit
  bit_not bn1{};  // implicit
  static_assert(is_same_v<decltype(bn1), bit_not<>>);
  bit_not bn2  = bn1; // implicit
  static_assert(is_same_v<decltype(bn2), decltype(bn1)>);
}

int fidi(double d, int i) { return i; }
int fidin(double d, int i) noexcept { return i; }

struct L {
  void operator()(int) {}
};

void test_function() // Explicit - 20.14.13.2
{
  function f1(fidi); // explicit
  static_assert(is_same_v<decltype(f1), function<int(double, int)>>);
  function f2(&fidi);
  static_assert(is_same_v<decltype(f2), function<int(double, int)>>);
  function f3 = f1; // implicit
  static_assert(is_same_v<decltype(f3), decltype(f1)>);
  // std::function<A(B) noexcept> is not allowed in n4640 standard draft
  function f4(fidin); 
  static_assert(is_same_v<decltype(f4), function<int(double, int)>>);
  function f5(&fidin);
  static_assert(is_same_v<decltype(f5), function<int(double, int)>>);
  function f6{L()};
  static_assert(is_same_v<decltype(f6), function<void(int)>>);
  function f7{[](int) { return 2.6; }};
  static_assert(is_same_v<decltype(f7), function<double(int)>>);
  function f8{[&](int) { return 2.6; }};
  static_assert(is_same_v<decltype(f8), function<double(int)>>);
}


// Adapted from example at
// http://en.cppreference.com/w/cpp/experimental/boyer_moore_searcher
void test_searchers() 
{
    std::string in = "Lorem ipsum dolor sit amge, consectetur adipiscing elit,"
                     " sed do eiusmod tempor incididunt ut labore et dolore magna aliqua";
    std::string needle = "pisci";
    // class default_searcher: Implicit - 20.14.14.1
    default_searcher ds1(needle.begin(), needle.end());
    static_assert(is_same_v<decltype(ds1), default_searcher<string::iterator, equal_to<>>>);
    default_searcher ds2(needle.begin(), needle.end(), my_pred<char>());
    static_assert(is_same_v<decltype(ds2), default_searcher<string::iterator, my_pred<char>>>);
    default_searcher ds3 = ds2;
    static_assert(is_same_v<decltype(ds3), decltype(ds2)>);

    // class boyer_moore_searcher: Implicit - 20.14.14.2
    boyer_moore_searcher bm1(needle.begin(), needle.end());
    static_assert(is_same_v<decltype(bm1), boyer_moore_searcher<string::iterator>>);
    boyer_moore_searcher bm2(needle.begin(), needle.end(), my_hash<char>());
    static_assert(is_same_v<decltype(bm2), boyer_moore_searcher<string::iterator, my_hash<char>>>);
    boyer_moore_searcher bm3(needle.begin(), needle.end(), my_hash<char>(), my_pred<char>());
    static_assert(is_same_v<decltype(bm3), boyer_moore_searcher<string::iterator, my_hash<char>, my_pred<char>>>);
    boyer_moore_searcher bm4 = bm3;
    static_assert(is_same_v<decltype(bm4), decltype(bm3)>);

    // class boyer_moore_searcher: Implicit - 20.14.14.3
    boyer_moore_horspool_searcher bmh1(needle.begin(), needle.end());
    static_assert(is_same_v<decltype(bmh1), boyer_moore_horspool_searcher<string::iterator>>);
    boyer_moore_horspool_searcher bmh2(needle.begin(), needle.end(), my_hash<char>());
    static_assert(is_same_v<decltype(bmh2), boyer_moore_horspool_searcher<string::iterator, my_hash<char>>>);
    boyer_moore_horspool_searcher bmh3(needle.begin(), needle.end(), my_hash<char>(), my_pred<char>());
    static_assert(is_same_v<decltype(bmh3), boyer_moore_horspool_searcher<string::iterator, my_hash<char>, my_pred<char>>>);
    boyer_moore_horspool_searcher bmh4 = bmh3;
    static_assert(is_same_v<decltype(bmh4), decltype(bmh3)>);

}

void test_wstring_convert() // Explicit
{
  wstring wstr ( L"test_wstring_convert" ); 
  string str;
  auto ccvt = make_unique<codecvt_utf8<wchar_t>>();
  // wstring_convert wc(ccvt.get()); // Why does this segv. Seems unrelated to P0433
  //  wstring_convert wc(ccvt.get(), codecvt_utf8<wchar_t>::state_type()); // Why does this segv. Seems unrelated to P0433

  // str = wc.to_bytes(wstr);

  // cout << str << endl;
}

void test_deque() // Explicit 23.3.8
{
  deque v1(allocator<string>{}); // explicit
  static_assert(is_same_v<decltype(v1), deque<string, allocator<string>>>);
  auto v2 = deque(3ul, std::allocator<int>());  // explicit
  static_assert(is_same_v<decltype(v2), deque<int, allocator<int>>>);
  deque v3(3, 'c', allocator<char>()); // implicit
  static_assert(is_same_v<decltype(v3), deque<char, allocator<char>>>);
  deque v4(v1.begin(), v1.end()); // explicit
  static_assert(is_same_v<decltype(v4), decltype(v1)>);
  deque v5(v1.begin(), v1.end(), scoped_allocator_adaptor<allocator<string>>()); // same as previous once gcc bug 79316 resolved
  static_assert(is_same_v<decltype(v5), deque<string, scoped_allocator_adaptor<allocator<string>>>>);
  deque v6 = v2; // implicit
  static_assert(is_same_v<decltype(v6), decltype(v2)>);
  deque v7 = move(v1); // implicit
  static_assert(is_same_v<decltype(v7), decltype(v1)>);
  deque v8(v2, allocator<int>()); // implicit
  static_assert(is_same_v<decltype(v8), decltype(v2)>);
  deque v9(move(v7), allocator<string>()); // implicit
  static_assert(is_same_v<decltype(v9), decltype(v7)>);
  deque v10({ 1, 2, 3, 4, 5}); // implicit
  static_assert(is_same_v<decltype(v10), deque<int>>);
  deque v11({ 1, 2, 3, 4, 5}, scoped_allocator_adaptor<allocator<int>>()); // implicit
  static_assert(is_same_v<decltype(v11), deque<int, scoped_allocator_adaptor<allocator<int>>>>);
}
  
void test_forward_list()  // Explicit 23.3.9
{
  forward_list v1(allocator<string>{}); // explicit
  static_assert(is_same_v<decltype(v1), forward_list<string, allocator<string>>>);
  auto v2 = forward_list(3ul, std::allocator<int>());  // explicit
  static_assert(is_same_v<decltype(v2), forward_list<int, allocator<int>>>);
  forward_list v3(3, 'c', allocator<char>()); // implicit
  static_assert(is_same_v<decltype(v3), forward_list<char, allocator<char>>>);
  forward_list v4(v1.begin(), v1.end()); // explicit
  static_assert(is_same_v<decltype(v4), decltype(v1)>);
  forward_list v5(v1.begin(), v1.end(), scoped_allocator_adaptor<allocator<string>>()); // same as previous once gcc bug 79316 resolved
  static_assert(is_same_v<decltype(v5), forward_list<string, scoped_allocator_adaptor<allocator<string>>>>);
  forward_list v6 = v2; // implicit
  static_assert(is_same_v<decltype(v6), decltype(v2)>);
  forward_list v7 = move(v1); // implicit
  static_assert(is_same_v<decltype(v7), decltype(v1)>);
  forward_list v8(v2, allocator<int>()); // implicit
  static_assert(is_same_v<decltype(v8), decltype(v2)>);
  forward_list v9(move(v7), allocator<string>()); // implicit
  static_assert(is_same_v<decltype(v9), decltype(v7)>);
  forward_list v10({ 1, 2, 3, 4, 5}); // implicit
  static_assert(is_same_v<decltype(v10), forward_list<int>>);
  forward_list v11({ 1, 2, 3, 4, 5}, scoped_allocator_adaptor<allocator<int>>()); // implicit
  static_assert(is_same_v<decltype(v11), forward_list<int, scoped_allocator_adaptor<allocator<int>>>>);
}
  
void test_list() // Explicit 23.3.10
{
  list v1(allocator<string>{}); // explicit
  static_assert(is_same_v<decltype(v1), list<string, allocator<string>>>);
  auto v2 = list(3ul, std::allocator<int>());  // explicit
  static_assert(is_same_v<decltype(v2), list<int, allocator<int>>>);
  list v3(3, 'c', allocator<char>()); // implicit
  static_assert(is_same_v<decltype(v3), list<char, allocator<char>>>);
  list v4(v1.begin(), v1.end()); // explicit
  static_assert(is_same_v<decltype(v4), decltype(v1)>);
  list v5(v1.begin(), v1.end(), scoped_allocator_adaptor<allocator<string>>()); // same as previous once gcc bug 79316 resolved
  static_assert(is_same_v<decltype(v5), list<string, scoped_allocator_adaptor<allocator<string>>>>);
  list v6 = v2; // implicit
  static_assert(is_same_v<decltype(v6), decltype(v2)>);
  list v7 = move(v1); // implicit
  static_assert(is_same_v<decltype(v7), decltype(v1)>);
  list v8(v2, allocator<int>()); // implicit
  static_assert(is_same_v<decltype(v8), decltype(v2)>);
  list v9(move(v7), allocator<string>()); // implicit
  static_assert(is_same_v<decltype(v9), decltype(v7)>);
  list v10({ 1, 2, 3, 4, 5}); // implicit
  static_assert(is_same_v<decltype(v10), list<int>>);
  list v11({ 1, 2, 3, 4, 5}, scoped_allocator_adaptor<allocator<int>>()); // implicit
  static_assert(is_same_v<decltype(v11), list<int, scoped_allocator_adaptor<allocator<int>>>>);
}
  
void test_vector() // Explicit 23.3.11
{
  vector v1(allocator<string>{}); // explicit
  static_assert(is_same_v<decltype(v1), vector<string, allocator<string>>>);
  auto v2 = vector(3ul, std::allocator<int>());  // explicit
  static_assert(is_same_v<decltype(v2), vector<int, allocator<int>>>);
  vector v3(3, 'c', allocator<char>()); // implicit
  static_assert(is_same_v<decltype(v3), vector<char, allocator<char>>>);
  vector v4(v1.begin(), v1.end()); // explicit
  static_assert(is_same_v<decltype(v4), decltype(v1)>);
  vector v5(v1.begin(), v1.end(), scoped_allocator_adaptor<allocator<string>>()); // same as previous once gcc bug 79316 resolved
  static_assert(is_same_v<decltype(v5), vector<string, scoped_allocator_adaptor<allocator<string>>>>);
  vector v6 = v2; // implicit
  static_assert(is_same_v<decltype(v6), decltype(v2)>);
  vector v7 = move(v1); // implicit
  static_assert(is_same_v<decltype(v7), decltype(v1)>);
  vector v8(v2, allocator<int>()); // implicit
  static_assert(is_same_v<decltype(v8), decltype(v2)>);
  vector v9(move(v7), allocator<string>()); // implicit
  static_assert(is_same_v<decltype(v9), decltype(v7)>);
  vector v10({ 1, 2, 3, 4, 5}); // implicit
  static_assert(is_same_v<decltype(v10), vector<int>>);
  vector v11({ 1, 2, 3, 4, 5}, scoped_allocator_adaptor<allocator<int>>()); // implicit
  static_assert(is_same_v<decltype(v11), vector<int, scoped_allocator_adaptor<allocator<int>>>>);

  // vector<bool>
  vector vb1(allocator<bool>{}); 
  static_assert(is_same_v<decltype(vb1), vector<bool, allocator<bool>>>);
  auto vb2 = vector(3ul, std::allocator<bool>());
  static_assert(is_same_v<decltype(vb2), vector<bool, allocator<bool>>>);
  vector vb3(3, true, allocator<bool>());
  static_assert(is_same_v<decltype(vb3), vector<bool, allocator<bool>>>);;
  vector vb4(vb1.begin(), vb1.end()); 
  static_assert(is_same_v<decltype(vb4), decltype(vb1)>);
  vector vb5(vb1.begin(), vb1.end(), scoped_allocator_adaptor<allocator<bool>>()); 
  static_assert(is_same_v<decltype(vb5), vector<bool, scoped_allocator_adaptor<allocator<bool>>>>);
  vector vb6 = vb2;
  static_assert(is_same_v<decltype(vb6), decltype(vb2)>);
  vector vb7 = move(vb1);
  static_assert(is_same_v<decltype(vb7), decltype(vb1)>);
  vector vb8(vb2, allocator<bool>());
  static_assert(is_same_v<decltype(vb8), decltype(vb2)>);
  vector vb9(move(vb7), allocator<bool>());
  static_assert(is_same_v<decltype(vb9), decltype(vb7)>);
  vector vb10({ false, true, true});
  static_assert(is_same_v<decltype(vb10), vector<bool>>);
  vector vb11({ false, true, true}, scoped_allocator_adaptor<allocator<bool>>());
  static_assert(is_same_v<decltype(vb11), vector<bool, scoped_allocator_adaptor<allocator<bool>>>>);
}

void test_map() // Explicit 23.4.4
{
  map m1(less<int>{}, std::allocator<pair<const int, string>>{});  // explicit
  static_assert(is_same_v<decltype(m1), map<int, string>>);
  map m2(m1.begin(), m1.end()); 
  static_assert(is_same_v<decltype(m2), map<int, string>>);
  map m3(m1.begin(), m1.end(), greater<int>()); 
  static_assert(is_same_v<decltype(m3), map<int, string, greater<int>>>);
  map m4(m1.begin(), m1.end(), greater<int>(), scoped_allocator_adaptor<allocator<pair<const int, string>>>()); // explicit
  static_assert(is_same_v<decltype(m4),
		          map<int, string, greater<int>,
		scoped_allocator_adaptor<allocator<pair<const int, string>>>>>);
  map m5 = m1;  // implicit
  static_assert(is_same_v<decltype(m5), decltype(m1)>);
  map m6 = move(m1);  // implicit
  static_assert(is_same_v<decltype(m6), decltype(m1)>);
  map m7(scoped_allocator_adaptor<std::allocator<pair<const int, string>>>{});  // explicit
  static_assert(is_same_v<decltype(m7),
		          map<int, string, less<int>,
		             scoped_allocator_adaptor<std::allocator<pair<const int, string>>>>>);
  map m8(m6, std::allocator<pair<const int, string>>{});  // depends
  static_assert(is_same_v<decltype(m8), decltype(m8)>);
  map m9(move(m6), std::allocator<pair<const int, string>>{});  // depends
  static_assert(is_same_v<decltype(m9), decltype(m6)>);

  // auto il = { {"foo"s, 1}, {"bar"s, 2}, {"baz"s, 3}, {"quux"s, 4}}; // Ill-formed. Prevents deducing maps from braced initializers
  //  map m({ {"foo"s, 1}, {"bar"s, 2}, {"baz"s, 3}, {"quux"s, 4}}); // Like this
  map m10 = initializer_list<pair<const string, int>>{ {"foo"s, 1}, {"bar"s, 2}, {"baz"s, 3}, {"quux"s, 4}};
  static_assert(is_same_v<decltype(m10), map<string, int>>); // explicit
  map m11(initializer_list<pair<const string, int>>{ {"foo"s, 1}, {"bar"s, 2}, {"baz"s, 3}, {"quux"s, 4}},
	     greater<string>(), scoped_allocator_adaptor<allocator<pair<const string, int>>>()); // explicit
  static_assert(is_same_v<decltype(m11), map<string, int, greater<string>,
		          scoped_allocator_adaptor<allocator<pair<const string, int>>>>>);
  map m12(initializer_list<pair<const string, int>>{ {"foo"s, 1}, {"bar"s, 2}, {"baz"s, 3}, {"quux"s, 4}},
	  greater<string>());
  static_assert(is_same_v<decltype(m12), map<string, int, greater<string>>>);
  map m13(m1.begin(), m1.end(), scoped_allocator_adaptor<allocator<pair<const int, string>>>()); // explicit
  static_assert(is_same_v<decltype(m13),
	                  map<int, string, /* default_order_t */less<int>,
		              scoped_allocator_adaptor<allocator<pair<const int, string>>>>>);
  map m14(initializer_list<pair<const string, int>>{ {"foo"s, 1}, {"bar"s, 2}, {"baz"s, 3}, {"quux"s, 4}},
	  scoped_allocator_adaptor<allocator<pair<const string, int>>>()); // explicit
  static_assert(is_same_v<decltype(m14), map<string, int, /* default_order_t */less<string>,
		          scoped_allocator_adaptor<allocator<pair<const string, int>>>>>);
}

void test_multimap() // Explicit 23.4.5
{
  multimap m1(less<int>{}, std::allocator<pair<const int, string>>{});  // explicit
  static_assert(is_same_v<decltype(m1), multimap<int, string>>);
  multimap m2(m1.begin(), m1.end()); 
  static_assert(is_same_v<decltype(m2), multimap<int, string>>);
  multimap m3(m1.begin(), m1.end(), greater<int>()); 
  static_assert(is_same_v<decltype(m3), multimap<int, string, greater<int>>>);
  multimap m4(m1.begin(), m1.end(), greater<int>(), scoped_allocator_adaptor<allocator<pair<const int, string>>>()); // explicit
  static_assert(is_same_v<decltype(m4),
		          multimap<int, string, greater<int>,
		scoped_allocator_adaptor<allocator<pair<const int, string>>>>>);
  multimap m5 = m1;  // implicit
  static_assert(is_same_v<decltype(m5), decltype(m1)>);
  multimap m6 = move(m1);  // implicit
  static_assert(is_same_v<decltype(m6), decltype(m1)>);
  multimap m7(scoped_allocator_adaptor<std::allocator<pair<const int, string>>>{});  // explicit
  static_assert(is_same_v<decltype(m7),
		          multimap<int, string, less<int>,
		             scoped_allocator_adaptor<std::allocator<pair<const int, string>>>>>);
  multimap m8(m6, std::allocator<pair<const int, string>>{});  // depends
  static_assert(is_same_v<decltype(m8), decltype(m8)>);
  multimap m9(move(m6), std::allocator<pair<const int, string>>{});  // depends
  static_assert(is_same_v<decltype(m9), decltype(m6)>);
  multimap m10 = initializer_list<pair<const string, int>>{ {"foo"s, 1}, {"bar"s, 2}, {"baz"s, 3}, {"quux"s, 4}};
  static_assert(is_same_v<decltype(m10), multimap<string, int>>); // explicit
  multimap m11(initializer_list<pair<const string, int>>{ {"foo"s, 1}, {"bar"s, 2}, {"baz"s, 3}, {"quux"s, 4}},
	       greater<string>(), scoped_allocator_adaptor<allocator<pair<const string, int>>>()); // explicit
  static_assert(is_same_v<decltype(m11), multimap<string, int, greater<string>,
		          scoped_allocator_adaptor<allocator<pair<const string, int>>>>>);
  multimap m12(initializer_list<pair<const string, int>>{ {"foo"s, 1}, {"bar"s, 2}, {"baz"s, 3}, {"quux"s, 4}},
		  greater<string>());
  static_assert(is_same_v<decltype(m12), multimap<string, int, greater<string>>>);
  multimap m13(m1.begin(), m1.end(), scoped_allocator_adaptor<allocator<pair<const int, string>>>()); // explicit
  static_assert(is_same_v<decltype(m13),
	                  multimap<int, string, /* default_order_t */less<int>,
		              scoped_allocator_adaptor<allocator<pair<const int, string>>>>>);
  multimap m14(initializer_list<pair<const string, int>>{ {"foo"s, 1}, {"bar"s, 2}, {"baz"s, 3}, {"quux"s, 4}},
	       scoped_allocator_adaptor<allocator<pair<const string, int>>>()); // explicit
  static_assert(is_same_v<decltype(m14), multimap<string, int, /* default_order_t */less<string>,
		          scoped_allocator_adaptor<allocator<pair<const string, int>>>>>);
}

void test_set() // Explicit 23.4.6
{
  set s1(greater<int>{}, scoped_allocator_adaptor<allocator<int>>{});  // explicit
  static_assert(is_same_v<decltype(s1), set<int, greater<int>,  scoped_allocator_adaptor<allocator<int>>>>);
  set s2(s1.begin(), s1.end());  // explicit
  static_assert(is_same_v<decltype(s2), set<int>>);
  set s3(s1.begin(), s1.end(), greater<int>());
  static_assert(is_same_v<decltype(s3), set<int, greater<int>>>);
  set s4(s1.begin(), s1.end(), greater<int>(), scoped_allocator_adaptor<allocator<int>>());
  static_assert(is_same_v<decltype(s4), set<int, greater<int>,  scoped_allocator_adaptor<allocator<int>>>>);
  set s5(s1);  // implicit
  static_assert(is_same_v<decltype(s5), decltype(s1)>);
  set s6(move(s1)); // implicit
  static_assert(is_same_v<decltype(s6), decltype(s1)>);
  set s7(scoped_allocator_adaptor<allocator<int>>{}); // explicit
  static_assert(is_same_v<decltype(s7),
                          set< int, less<int>, scoped_allocator_adaptor<allocator<int>>>>);
  set s8(s6, scoped_allocator_adaptor<allocator<int>>{}); // depends
  static_assert(is_same_v<decltype(s8), decltype(s6)>);
  set s9(move(s6), scoped_allocator_adaptor<allocator<int>>{}); // depends
  static_assert(is_same_v<decltype(s9), decltype(s6)>);
  set s10({ 1, 2, 3});  // explicit
  static_assert(is_same_v<decltype(s10), set<int>>);
  set s11({1, 2, 3}, greater<int>());
  static_assert(is_same_v<decltype(s11), set<int, greater<int>>>);
  set s12({1, 2, 3}, greater<int>(), scoped_allocator_adaptor<allocator<int>>());
  static_assert(is_same_v<decltype(s12), set<int, greater<int>, scoped_allocator_adaptor<allocator<int>>>>);
  set s13(s9.begin(), s9.end(), scoped_allocator_adaptor<allocator<int>>()); // explicit
  static_assert(is_same_v<decltype(s13),
                          set<int, /* default_order_t */ less<int>, scoped_allocator_adaptor<allocator<int>>>>);
  set s14({1, 2, 3}, scoped_allocator_adaptor<allocator<int>>()); // explicit
  static_assert(is_same_v<decltype(s14),
                          set<int, /* default_order_t */ less<int>, scoped_allocator_adaptor<allocator<int>>>>);
}

void test_multiset() // Explicit 23.4.7
{
  multiset s1(greater<int>{}, scoped_allocator_adaptor<allocator<int>>{});  // explicit
  static_assert(is_same_v<decltype(s1), multiset<int, greater<int>,  scoped_allocator_adaptor<allocator<int>>>>);
  multiset s2(s1.begin(), s1.end());  // explicit
  static_assert(is_same_v<decltype(s2), multiset<int>>);
  multiset s3(s1.begin(), s1.end(), greater<int>());
  static_assert(is_same_v<decltype(s3), multiset<int, greater<int>>>);
  multiset s4(s1.begin(), s1.end(), greater<int>(), scoped_allocator_adaptor<allocator<int>>());
  static_assert(is_same_v<decltype(s4), multiset<int, greater<int>,  scoped_allocator_adaptor<allocator<int>>>>);
  multiset s5(s1);  // implicit
  static_assert(is_same_v<decltype(s5), decltype(s1)>);
  multiset s6(move(s1)); // implicit
  static_assert(is_same_v<decltype(s6), decltype(s1)>);
  multiset s7(scoped_allocator_adaptor<allocator<int>>{}); // explicit
  static_assert(is_same_v<decltype(s7),
                          multiset< int, less<int>, scoped_allocator_adaptor<allocator<int>>>>);
  multiset s8(s6, scoped_allocator_adaptor<allocator<int>>{}); // depends
  static_assert(is_same_v<decltype(s8), decltype(s6)>);
  multiset s9(move(s6), scoped_allocator_adaptor<allocator<int>>{}); // depends
  static_assert(is_same_v<decltype(s9), decltype(s6)>);
  multiset s10({ 1, 2, 3});  // explicit
  static_assert(is_same_v<decltype(s10), multiset<int>>);
  multiset s11({1, 2, 3}, greater<int>());
  static_assert(is_same_v<decltype(s11), multiset<int, greater<int>>>);
  multiset s12({1, 2, 3}, greater<int>(), scoped_allocator_adaptor<allocator<int>>());
  static_assert(is_same_v<decltype(s12), multiset<int, greater<int>, scoped_allocator_adaptor<allocator<int>>>>);
  multiset s13(s9.begin(), s9.end(), scoped_allocator_adaptor<allocator<int>>()); // explicit
  static_assert(is_same_v<decltype(s13),
                          multiset<int, /* default_order_t */ less<int>, scoped_allocator_adaptor<allocator<int>>>>);
  multiset s14({1, 2, 3}, scoped_allocator_adaptor<allocator<int>>()); // explicit
  static_assert(is_same_v<decltype(s14),
                          multiset<int, /* default_order_t */ less<int>, scoped_allocator_adaptor<allocator<int>>>>);
}

void test_unordered_map() // Explicit
{
  unordered_map u1(32, my_hash<int>(), my_pred<int>(), allocator<pair<const int, string>>()); // explicit
  static_assert(is_same_v<decltype(u1), unordered_map<int, string, my_hash<int>, my_pred<int>>>);
  unordered_map u2(u1.begin(), u1.end(), 32, my_hash<int>(), my_pred<int>(),  allocator<pair<const int, string>>()); // explicit
  static_assert(is_same_v<decltype(u2),
		          unordered_map<int, string, my_hash<int>, my_pred<int>, allocator<pair<const int, string>>>>);
  unordered_map u3(u1.begin(), u1.end(), 32, my_hash<int>(), my_pred<int>());
  static_assert(is_same_v<decltype(u3), unordered_map<int, string, my_hash<int>, my_pred<int>>>);
  unordered_map u4(u1.begin(), u1.end(), 32, my_hash<int>());
  static_assert(is_same_v<decltype(u4), unordered_map<int, string, my_hash<int>>>);
  unordered_map u5(u1.begin(), u1.end(), 32);
  static_assert(is_same_v<decltype(u5), unordered_map<int, string>>);
  unordered_map u6(u1.begin(), u1.end());
  static_assert(is_same_v<decltype(u6), unordered_map<int, string>>);
  unordered_map u7(u1); // implicit
  static_assert(is_same_v<decltype(u7), decltype(u1)>);
  unordered_map u8(move(u1)); // implicit
  static_assert(is_same_v<decltype(u8), decltype(u1)>);
  unordered_map u9(scoped_allocator_adaptor<allocator<pair<const long, double>>>{});
  static_assert(is_same_v<decltype(u9),
	                  unordered_map<long, double, hash<long>, equal_to<long>,
                                        scoped_allocator_adaptor<allocator<pair<const long, double>>>>>);
  unordered_map u10(u9, scoped_allocator_adaptor<allocator<pair<const long, double>>>()); // depends
  static_assert(is_same_v<decltype(u10), decltype(u9)>);
  unordered_map u11(move(u9), scoped_allocator_adaptor<allocator<pair<const long, double>>>()); // depends
  static_assert(is_same_v<decltype(u11), decltype(u9)>);
  unordered_map u12(initializer_list<pair<const string, int>>({{"foo"s, 1}, {"bar"s, 2}, {"baz"s, 3}, {"quux"s, 4}}), // explicit
		    32, my_hash<string>(), my_pred<string>(), scoped_allocator_adaptor<allocator<pair<const string, int>>>());
  static_assert(is_same_v<decltype(u12),
                          unordered_map<string, int, my_hash<string>, my_pred<string>,
		                        scoped_allocator_adaptor<allocator<pair<const string, int>>>>>);
  unordered_map u13(initializer_list<pair<const string, int>>({{"foo"s, 1}, {"bar"s, 2}, {"baz"s, 3}, {"quux"s, 4}}),
		    32, my_hash<string>(), my_pred<string>());
  static_assert(is_same_v<decltype(u13), unordered_map<string, int, my_hash<string>, my_pred<string>>>);
  unordered_map u14(initializer_list<pair<const string, int>>({{"foo"s, 1}, {"bar"s, 2}, {"baz"s, 3}, {"quux"s, 4}}),
		    32, my_hash<string>());
  static_assert(is_same_v<decltype(u14), unordered_map<string, int, my_hash<string>>>);
  unordered_map u15(initializer_list<pair<const string, int>>({{"foo"s, 1}, {"bar"s, 2}, {"baz"s, 3}, {"quux"s, 4}}),
		    32);
  static_assert(is_same_v<decltype(u15), unordered_map<string, int>>);
  unordered_map u16(initializer_list<pair<const string, int>>({{"foo"s, 1}, {"bar"s, 2}, {"baz"s, 3}, {"quux"s, 4}}));
  static_assert(is_same_v<decltype(u16), unordered_map<string, int>>);
  unordered_map u17(32, scoped_allocator_adaptor<allocator<pair<const long, double>>>()); // explicit
  static_assert(is_same_v<decltype(u17),
	                  unordered_map<long, double, hash<long>, equal_to<long>,
                                        scoped_allocator_adaptor<allocator<pair<const long, double>>>>>);
  unordered_map u18(32, my_hash<long>(), scoped_allocator_adaptor<allocator<pair<const long, double>>>()); // explicit
  static_assert(is_same_v<decltype(u18),
	                  unordered_map<long, double, my_hash<long>, equal_to<long>,
                                        scoped_allocator_adaptor<allocator<pair<const long, double>>>>>);
  unordered_map u19(32, scoped_allocator_adaptor<allocator<pair<const int, string>>>()); // explicit
  static_assert(is_same_v<decltype(u19),
                          unordered_map<int, string, hash<int>, equal_to<int>,
	                                scoped_allocator_adaptor<allocator<pair<const int, string>>>>>);
  unordered_map u20(initializer_list<pair<const string, int>>({{"foo"s, 1}, {"bar"s, 2}, {"baz"s, 3}, {"quux"s, 4}}), // explicit
		    32, scoped_allocator_adaptor<allocator<pair<const string, int>>>());
  static_assert(is_same_v<decltype(u20),
                          unordered_map<string, int, hash<string>, equal_to<string>,
		                        scoped_allocator_adaptor<allocator<pair<const string, int>>>>>);
  unordered_map u21(initializer_list<pair<const string, int>>({{"foo"s, 1}, {"bar"s, 2}, {"baz"s, 3}, {"quux"s, 4}}), // explicit
		    32, my_hash<string>(), scoped_allocator_adaptor<allocator<pair<const string, int>>>());
  static_assert(is_same_v<decltype(u21),
                          unordered_map<string, int, my_hash<string>, equal_to<string>,
		                        scoped_allocator_adaptor<allocator<pair<const string, int>>>>>);
}
  
void test_unordered_multimap()  // Explicit
{
  unordered_multimap u1(32, my_hash<int>(), my_pred<int>(), allocator<pair<const int, string>>{}); // explicit
  static_assert(is_same_v<decltype(u1), unordered_multimap<int, string, my_hash<int>, my_pred<int>>>);
  unordered_multimap u2(u1.begin(), u1.end(), 32, my_hash<int>(), my_pred<int>(),  allocator<pair<const int, string>>()); // explicit
  static_assert(is_same_v<decltype(u2),
		          unordered_multimap<int, string, my_hash<int>, my_pred<int>, allocator<pair<const int, string>>>>);
  unordered_multimap u3(u1.begin(), u1.end(), 32, my_hash<int>(), my_pred<int>());
  static_assert(is_same_v<decltype(u3), unordered_multimap<int, string, my_hash<int>, my_pred<int>>>);
  unordered_multimap u4(u1.begin(), u1.end(), 32, my_hash<int>());
  static_assert(is_same_v<decltype(u4), unordered_multimap<int, string, my_hash<int>>>);
  unordered_multimap u5(u1.begin(), u1.end(), 32);
  static_assert(is_same_v<decltype(u5), unordered_multimap<int, string>>);
  unordered_multimap u6(u1.begin(), u1.end());
  static_assert(is_same_v<decltype(u6), unordered_multimap<int, string>>);
  unordered_multimap u7(u1); // implicit
  static_assert(is_same_v<decltype(u7), decltype(u1)>);
  unordered_multimap u8(move(u1)); // implicit
  static_assert(is_same_v<decltype(u8), decltype(u1)>);
  unordered_multimap u9(scoped_allocator_adaptor<allocator<pair<const long, double>>>{});
  static_assert(is_same_v<decltype(u9),
	                  unordered_multimap<long, double, hash<long>, equal_to<long>,
                                        scoped_allocator_adaptor<allocator<pair<const long, double>>>>>);
  unordered_multimap u10(u9, scoped_allocator_adaptor<allocator<pair<const long, double>>>{}); // depends
  static_assert(is_same_v<decltype(u10), decltype(u9)>);
  unordered_multimap u11(move(u9), scoped_allocator_adaptor<allocator<pair<const long, double>>>()); // depends
  static_assert(is_same_v<decltype(u11), decltype(u9)>);
  unordered_multimap u12(initializer_list<pair<const string, int>>({{"foo"s, 1}, {"bar"s, 2}, {"baz"s, 3}, {"quux"s, 4}}), // explicit
		    32, my_hash<string>(), my_pred<string>(), scoped_allocator_adaptor<allocator<pair<const string, int>>>());
  static_assert(is_same_v<decltype(u12),
                          unordered_multimap<string, int, my_hash<string>, my_pred<string>,
		                        scoped_allocator_adaptor<allocator<pair<const string, int>>>>>);
  unordered_multimap u13(initializer_list<pair<const string, int>>({{"foo"s, 1}, {"bar"s, 2}, {"baz"s, 3}, {"quux"s, 4}}),
		    32, my_hash<string>(), my_pred<string>());
  static_assert(is_same_v<decltype(u13), unordered_multimap<string, int, my_hash<string>, my_pred<string>>>);
  unordered_multimap u14(initializer_list<pair<const string, int>>({{"foo"s, 1}, {"bar"s, 2}, {"baz"s, 3}, {"quux"s, 4}}),
		    32, my_hash<string>());
  static_assert(is_same_v<decltype(u14), unordered_multimap<string, int, my_hash<string>>>);
  unordered_multimap u15(initializer_list<pair<const string, int>>({{"foo"s, 1}, {"bar"s, 2}, {"baz"s, 3}, {"quux"s, 4}}),
		    32);
  static_assert(is_same_v<decltype(u15), unordered_multimap<string, int>>);
  unordered_multimap u16(initializer_list<pair<const string, int>>({{"foo"s, 1}, {"bar"s, 2}, {"baz"s, 3}, {"quux"s, 4}}));
  static_assert(is_same_v<decltype(u16), unordered_multimap<string, int>>);
  unordered_multimap u17(32, scoped_allocator_adaptor<allocator<pair<const long, double>>>()); // explicit
  static_assert(is_same_v<decltype(u17),
	                  unordered_multimap<long, double, hash<long>, equal_to<long>,
                                        scoped_allocator_adaptor<allocator<pair<const long, double>>>>>);
  unordered_multimap u18(32, my_hash<long>(), scoped_allocator_adaptor<allocator<pair<const long, double>>>()); // explicit
  static_assert(is_same_v<decltype(u18),
	                  unordered_multimap<long, double, my_hash<long>, equal_to<long>,
                                        scoped_allocator_adaptor<allocator<pair<const long, double>>>>>);
  unordered_multimap u19(32, scoped_allocator_adaptor<allocator<pair<const int, string>>>()); // explicit
  static_assert(is_same_v<decltype(u19),
                          unordered_multimap<int, string, hash<int>, equal_to<int>,
	                                scoped_allocator_adaptor<allocator<pair<const int, string>>>>>);
  unordered_multimap u20(initializer_list<pair<const string, int>>({{"foo"s, 1}, {"bar"s, 2}, {"baz"s, 3}, {"quux"s, 4}}), // explicit
		    32, scoped_allocator_adaptor<allocator<pair<const string, int>>>());
  static_assert(is_same_v<decltype(u20),
                          unordered_multimap<string, int, hash<string>, equal_to<string>,
		                        scoped_allocator_adaptor<allocator<pair<const string, int>>>>>);
  unordered_multimap u21(initializer_list<pair<const string, int>>({{"foo"s, 1}, {"bar"s, 2}, {"baz"s, 3}, {"quux"s, 4}}), // explicit
		    32, my_hash<string>(), scoped_allocator_adaptor<allocator<pair<const string, int>>>());
  static_assert(is_same_v<decltype(u21),
                          unordered_multimap<string, int, my_hash<string>, equal_to<string>,
		                        scoped_allocator_adaptor<allocator<pair<const string, int>>>>>);
}

void test_unordered_set() // Explicit
{
  unordered_set s1({1, 2, 3, 4});
  unordered_set s2(s1.begin(), s1.end());
  static_assert(is_same_v<decltype(s2), unordered_set<int>>);
  unordered_set s3 = s2;
  static_assert(is_same_v<decltype(s3), unordered_set<int>>);
}

void test_unordered_multiset() // Explicit
{
  unordered_multiset s1({1, 2, 3, 4});
  unordered_multiset s2(s1.begin(), s1.end());
  static_assert(is_same_v<decltype(s2), unordered_multiset<int>>);
  unordered_multiset s3 = s2;
  static_assert(is_same_v<decltype(s3), unordered_multiset<int>>);
}

void test_queue() // Explicit
{
  deque d({3, 1, 4, 1, 5});
  queue q(d, allocator<int>());
  static_assert(is_same_v<decltype(q), queue<int>>);
}

void test_priority_queue() // Explicit
{
  deque d({3, 1, 4, 1, 5});
  priority_queue q(d.begin(), d.end());
  static_assert(is_same_v<decltype(q), priority_queue<int>>);
}

void test_stack() // Explicit
{
  deque d({3, 1, 4, 1, 5});
  stack s(d, allocator<int>());
  static_assert(is_same_v<decltype(s), stack<int>>);
  stack s2{s, allocator<int>()};
  int result;
  cout << __cxa_demangle(typeid(s2).name(), nullptr, nullptr, &result) << endl;
  // static_assert(is_same_v<decltype(s2), stack<int>>); // Compiler bug?
}

void test_iterators()
{
  // Iterator adapters (24.5)
  vector v({1, 2, 3});
  // class reverse_iterator : Implicit
  reverse_iterator ri(v.end());
  static_assert(is_same_v<decltype(ri), reverse_iterator<vector<int>::iterator>>);
  assert(*ri == 3);

  // class back_insert_iterator : Implicit
  back_insert_iterator biiv(v);
  static_assert(is_same_v<decltype(biiv), back_insert_iterator<vector<int>>>);
  back_insert_iterator biiv2(v);
  static_assert(is_same_v<decltype(biiv2), back_insert_iterator<vector<int>>>);

  // class front_insert_iterator : Implicit
  front_insert_iterator fiiv(v);
  static_assert(is_same_v<decltype(fiiv), front_insert_iterator<vector<int>>>);

  // class insert_iterator: Implicit
  list l({1, 2, 3}); // Why can't I say list l{1, 2, 3}? I can say list<int> l{1, 2, 3}
  insert_iterator ii(l, next(l.begin())); // Based on http://en.cppreference.com/w/cpp/iterator/insert_iterator
  static_assert(is_same_v<decltype(ii), insert_iterator<list<int>>>);

  // class move_iterator: Implicit
  move_iterator mi(l.begin());
  static_assert(is_same_v<decltype(mi), move_iterator<list<int>::iterator>>);

  // Stream iterators (24.6): Implicit
  istreambuf_iterator<char> isbi(cin);
  static_assert(is_same_v<decltype(isbi), istreambuf_iterator<char>>);
  
}


void test_complex() // Implicit
{
  complex c(2.5, 3.7);
  static_assert(is_same_v<decltype(c), complex<double>>);
}

void test_random()
{
  // Engines are not deducible because they all have non-type parameters

  // class uniform_int_distribution: Implicit
  uniform_int_distribution uid(5);
  static_assert(is_same_v<decltype(uid), uniform_int_distribution<int>>);

  // class uniform_real_distribution: Implicit
  uniform_real_distribution urd{};
  static_assert(is_same_v<decltype(urd), uniform_real_distribution<double>>);

  uniform_real_distribution urdf{2.6f};
  static_assert(is_same_v<decltype(urdf), uniform_real_distribution<float>>);

  // Bernoulli distribution is not a template
  
  // class binomial_distribution: Implicit
  binomial_distribution bd{7L};
  static_assert(is_same_v<decltype(bd), binomial_distribution<long>>);

  // geometric_distribution is not deducible
  
  // class negative_binomial_distribution: Implicit
  negative_binomial_distribution nbd{7L, 0.2};
  static_assert(is_same_v<decltype(nbd), negative_binomial_distribution<long>>);

  // poisson_distribution is not deducible
  
  // class exponential_distribution: Implicit
  exponential_distribution ed{1.1};
  static_assert(is_same_v<decltype(ed), exponential_distribution<double>>);

  // class gamma_distribution: Implicit
  gamma_distribution gd{};
  static_assert(is_same_v<decltype(gd), gamma_distribution<double>>);

  gamma_distribution gdf{2.5f, .7f};
  static_assert(is_same_v<decltype(gdf), gamma_distribution<float>>);

  // class weibull_distribution: Implicit
  weibull_distribution wd{};
  static_assert(is_same_v<decltype(wd), weibull_distribution<double>>);

  weibull_distribution wdf{2.5f, .7f};
  static_assert(is_same_v<decltype(wdf), weibull_distribution<float>>);

  // class extreme_value_distribution: Implicit
  extreme_value_distribution evd{};
  static_assert(is_same_v<decltype(evd), extreme_value_distribution<double>>);

  extreme_value_distribution evdf{2.5f, .7f};
  static_assert(is_same_v<decltype(evdf), extreme_value_distribution<float>>);

  // class normal_distribution: Implicit
  normal_distribution nd{};
  static_assert(is_same_v<decltype(nd), normal_distribution<double>>);

  normal_distribution ndf{2.5f, .7f};
  static_assert(is_same_v<decltype(ndf), normal_distribution<float>>);

  // class lognormal_distribution: Implicit
  lognormal_distribution lnd{};
  static_assert(is_same_v<decltype(lnd), lognormal_distribution<double>>);

  lognormal_distribution lndf{2.5f, .7f};
  static_assert(is_same_v<decltype(lndf), lognormal_distribution<float>>);

  // class chi_squared_distribution: Implicit
  chi_squared_distribution csd{};
  static_assert(is_same_v<decltype(csd), chi_squared_distribution<double>>);

  chi_squared_distribution csdf(2.5f);
  static_assert(is_same_v<decltype(csdf), chi_squared_distribution<float>>);  

  // class cauchy_distribution: Implicit
  cauchy_distribution cd{};
  static_assert(is_same_v<decltype(cd), cauchy_distribution<double>>);

  cauchy_distribution cdf{2.5f, .7f};
  static_assert(is_same_v<decltype(cdf), cauchy_distribution<float>>);

  // class fisher_f_distribution: Implicit
  fisher_f_distribution ffd{};
  static_assert(is_same_v<decltype(ffd), fisher_f_distribution<double>>);

  fisher_f_distribution ffdf{2.5f, .7f};
  static_assert(is_same_v<decltype(ffdf), fisher_f_distribution<float>>);

  // class student_t_distribution: Implicit
  student_t_distribution std{};
  static_assert(is_same_v<decltype(std), student_t_distribution<double>>);

  student_t_distribution stdf(2.5f);
  static_assert(is_same_v<decltype(stdf), student_t_distribution<float>>);

  // discrete_distribution is not deducible
  // piecewise_constant_distribution is not deducible
  // piecewise_linear_distribution is not deducible
}

void test_valarray()  // Explicit
{
  int ints[] = {1, 2, 3, 4, 5, 6, 7, 8};
  int *ip = ints;
  valarray v1(5, 6);
  valarray v2(ints, 8);
  valarray v3(ip, 8);
  valarray v4({1, 2, 3, 4});
  valarray v5(v2[slice(1, 3, 2)]);

  // Adapted from example at http://en.cppreference.com/w/cpp/numeric/valarray/gslice
  valarray v({ 111,112,113, 121,122,123, 131,132,133, 141,142,143,
	       211,212,213, 221,222,223, 231,232,233, 241,242,243});
  valarray v6 = v[gslice(0, {2, 4}, {4*3, 3})];
  valarray v7 = v[v > 200];  // construction from mask_array
  valarray v8 = v[valarray({(size_t)1, (size_t)5, (size_t)2})];       // construction from indirect_array
}

void test_io()
{
  // class basic_ios: Implicit
  basic_ios bios{cout.rdbuf()};
  // basic_streambuf constructors are not deducible

  // class basic_istream: Implicit
  basic_istream bis{cin.rdbuf()};

  // class basic_ostream: Implicit
  basic_ostream bos{cout.rdbuf()};

  string str = "foo";

  // class basic_stringbuf: Implicit
  auto bsb = basic_stringbuf(str);

  // class basic_istringstream: Implicit
  auto biss = basic_istringstream(str);

  // class basic_ostringstream: Implicit
  auto boss = basic_ostringstream(str);

  // basic_filebuf, basic_ifstream, basic_ofstream, and basic_fstream constructors are not deducible
  // filesystem classes are not templates
}

void test_regex()
{
  // class basic_regex: Explicit
  basic_regex br("f.*r");
  basic_regex br2("f.*r", 4);
  basic_regex br3("f.*r"s);
  string s = "f.*r";
  basic_regex br4(s.begin(), s.end());
  basic_regex br5(s.begin(), s.end(), regex_constants::icase);
  string s2 = "xfoobary";

  // class regex_iterator: Explicit
  regex_iterator ri(s2.begin(), s2.end(), br5);

  // class regex_token_iterator: Explicit
  regex_token_iterator rti(s2.begin(), s2.end(), br5);
}

void test_atomic() // Implicit
{
  atomic a = 3;
}

void test_thread()
{
  mutex m;
  {
    // class lock_guard: Implicit
    lock_guard lg(m);
  }

  {
    // class unique_lock: Implicit
    unique_lock ul(m);
  }
  shared_mutex sm;
  {
    // class shared_lock: Implicit
    shared_lock sl(sm);
  }
  {
  // class scoped_lock: Implicit
    // scoped_lock sl1(m, sm); // Not yet supported by g++ in either lock_guard or scoped_lock
  }

  // class promise: Implicit
  promise p{allocator_arg_t(), std::allocator<int>()};
  // future and shared_future  not deducible
  // Don't create function pointer deduction guide for packaged_task. Too narrow
}

unique_ptr up(new A<int>(3));
shared_ptr sp(new A<int>(3));
weak_ptr wp{sp};
int main()
{
  int i{5};
  basic_string_view bsv("foo");
  //  basic_string bs("hello world", 2);
  
  auto rw = reference_wrapper(i);
  allocator al = allocator<int>();

  cout << up->t << endl;
  cout << "Hello, world" << endl;
  test_pair();
  test_tuple();
  test_optional();
  test_unique_ptr();
  test_shared_ptr();
  test_weak_ptr();
  test_owner_less();
  test_polymorphic_allocator();
  test_scoped_allocator_adaptor();
  test_reference_wrapper();
  test_arithmetic_operations();
  test_comparisons();
  test_logical_operations();
  test_bit_operations();
  test_function();
  test_searchers();
  test_wstring_convert();
  test_deque();
  test_forward_list();
  test_list();
  test_vector();
  test_map();
  test_unordered_map();
  test_multimap();
  test_unordered_multimap();
  test_set();
  test_unordered_set();
  test_multiset();
  test_unordered_multiset();
  test_queue();
  test_priority_queue();
  test_stack();
  test_iterators();
  test_valarray();
  test_io();
  test_regex();
  test_atomic();
  test_thread();
  return 0;
}


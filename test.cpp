#include<cassert>
#include<iostream>
#include<memory>
#include<experimental/memory_resource> // Not in g++ yet
#include<tuple>
#include<optional>
#include<vector>
#include<scoped_allocator>
#include<functional>
#include<string_view>
#include<locale>
#include<codecvt>
#include<type_traits>
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

template<typename T>
struct A {
  int foo() { return 7; }
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

void test_pair() // Explicit
{
  // Some odd seeming tests are to make sure N4387 is not causing any trouble
  pair p(3, 7.5);  // explicit (Don't label most of the followign as they use same constructor)
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


void test_tuple()
{
  // Some odd seeming tests are to make sure N4387 is not causing any trouble
  tuple p1(3, 7.5, 1);
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
  tuple p9{p};
  static_assert(is_same_v<decltype(p9), tuple<int, double>>);
  tuple p10{pair(3, 5.2)};
  static_assert(is_same_v<decltype(p10), tuple<int, double>>);

  tuple pa1(allocator_arg, allocator<int>(), 3, 7.5, 1);
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
  tuple pa9{allocator_arg, allocator<int>(), p};
  static_assert(is_same_v<decltype(pa9), tuple<int, double>>);
  tuple pa10{allocator_arg, allocator<int>(), pair(3, 5.2)};
  static_assert(is_same_v<decltype(pa10), tuple<int, double>>);
}

void test_optional()
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

void test_unique_ptr()
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

void test_shared_ptr()
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

void test_weak_ptr()
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
  // owner_less ol{};  // implicit Resolve: Why doesn't this work?
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
  // plus p1{};  // implicit Resolve: Why doesn't this work?
  plus<int> pi;
  plus p1  = pi; // implicit
  static_assert(is_same_v<decltype(p1), decltype(pi)>);

  // class minus: Implicit
  // minus m1{};  // implicit Resolve: Why doesn't this work?
  minus<int> mi;
  minus m1  = mi; // implicit
  static_assert(is_same_v<decltype(m1), decltype(mi)>);

  // class multiplies: Implicit
  // multiplies mu1{};  // implicit Resolve: Why doesn't this work?
  multiplies<int> mui;
  multiplies mu1  = mui; // implicit
  static_assert(is_same_v<decltype(mu1), decltype(mui)>);

  // class divides: Implicit
  // divides d1{};  // implicit Resolve: Why doesn't this work?
  divides<int> di;
  divides d1  = di; // implicit
  static_assert(is_same_v<decltype(d1), decltype(di)>);

  // class modulus: Implicit
  // modulus mo1{};  // implicit Resolve: Why doesn't this work?
  modulus<int> moi;
  modulus mo1  = moi; // implicit
  static_assert(is_same_v<decltype(mo1), decltype(moi)>);

  // class negate: Implicit
  // negate n1{};  // implicit Resolve: Why doesn't this work?
  negate<int> ni;
  negate n1  = ni; // implicit
  static_assert(is_same_v<decltype(n1), decltype(ni)>);  
}

void test_comparisons() // 20.14.7
{
  // class equal_to: Implicit
  // equal_to et1{};  // implicit Resolve: Why doesn't this work?
  equal_to<int> eti;
  equal_to et1  = eti; // implicit
  static_assert(is_same_v<decltype(et1), decltype(eti)>);

  // class not_equal_to: Implicit
  // not_equal_to net1{};  // implicit Resolve: Why doesn't this work?
  not_equal_to<int> neti;
  not_equal_to net1  = neti; // implicit
  static_assert(is_same_v<decltype(net1), decltype(neti)>);

  // class greater: Implicit
  // greater g1{};  // implicit Resolve: Why doesn't this work?
  greater<int> gi;
  greater g1  = gi; // implicit
  static_assert(is_same_v<decltype(g1), decltype(gi)>);

  // class less : Implicit
  // less l1{};  // implicit Resolve: Why doesn't this work?
  less<int> li;
  less l1  = li; // implicit
  static_assert(is_same_v<decltype(l1), decltype(li)>);

  // class greater_equal: Implicit
  // greater_equal ge1{};  // implicit Resolve: Why doesn't this work?
  greater_equal<int> gei;
  greater_equal ge1  = gei; // implicit
  static_assert(is_same_v<decltype(ge1), decltype(gei)>);

  // class less_equal: Implicit
  // less_equal le1{};  // implicit Resolve: Why doesn't this work?
  less_equal<int> lei;
  less_equal le1  = lei; // implicit
  static_assert(is_same_v<decltype(le1), decltype(lei)>);
}

void test_logical_operations() // 20.14.8
{
  // class logical_and: Implicit
  // logical_and la1{};  // implicit Resolve: Why doesn't this work?
  logical_and<int> lai;
  logical_and la1  = lai; // implicit
  static_assert(is_same_v<decltype(la1), decltype(lai)>);

  // class logical_or: Implicit
  // logical_or lo1{};  // implicit Resolve: Why doesn't this work?
  logical_or<int> loi;
  logical_or lo1  = loi; // implicit
  static_assert(is_same_v<decltype(lo1), decltype(loi)>);

  // class logical_not: Implicit
  // logical_not ln1{};  // implicit Resolve: Why doesn't this work?
  logical_not<int> lni;
  logical_not ln1  = lni; // implicit
  static_assert(is_same_v<decltype(ln1), decltype(lni)>);
}

// Adapted from example at
// http://en.cppreference.com/w/cpp/experimental/boyer_moore_searcher
void test_searchers()
{
    std::string in = "Lorem ipsum dolor sit amge, consectetur adipiscing elit,"
                     " sed do eiusmod tempor incididunt ut labore et dolore magna aliqua";
    std::string needle = "pisci";
    auto bms = boyer_moore_searcher(needle.begin(), needle.end());
    /* // std::search doesn't yet support searchers in g++
    auto it = search(in.begin(), in.end(), 
                      boyer_moore_searcher(needle.begin(), needle.end()));
    if(it != in.end())
        std::cout << "The string " << needle << " found at offset "
                  << it - in.begin() << '\n';
    else
        std::cout << "The string " << needle << " not found\n";
    */
}

void test_wstring_convert()
{
  wstring wstr ( L"test_wstring_convert" ); 
  string str;
  auto ccvt = make_unique<codecvt_utf8<wchar_t>>();
  // wstring_convert wc(ccvt.get()); // Why does this segv. Seems unrelated to P0433
  //  wstring_convert wc(ccvt.get(), codecvt_utf8<wchar_t>::state_type()); // Why does this segv. Seems unrelated to P0433

  // str = wc.to_bytes(wstr);

  // cout << str << endl;
}

void test_deque()
{
  deque d1({ 1, 2, 3, 4, 5});
  static_assert(is_same_v<decltype(d1), deque<int>>);
  deque d2(d1.begin(), d1.end());
  static_assert(is_same_v<decltype(d2), deque<int>>);
  deque d3 = d2;
  static_assert(is_same_v<decltype(d3), deque<int>>);
}
  
void test_forward_list()
{
  forward_list d1({ 1, 2, 3, 4, 5});
  static_assert(is_same_v<decltype(d1), forward_list<int>>);
  forward_list d2(d1.begin(), d1.end());
  static_assert(is_same_v<decltype(d2), forward_list<int>>);
  forward_list d3 = d2;
  static_assert(is_same_v<decltype(d3), forward_list<int>>);
}
  
void test_list()
{
  list d1({ 1, 2, 3, 4, 5});
  static_assert(is_same_v<decltype(d1), list<int>>);
  list d2(d1.begin(), d1.end());
  static_assert(is_same_v<decltype(d2), list<int>>);
  list d3 = d2;
  static_assert(is_same_v<decltype(d3), list<int>>);
}
  
void test_vector()
{
  vector d1({ 1, 2, 3, 4, 5});
  static_assert(is_same_v<decltype(d1), vector<int>>);
  vector d2(d1.begin(), d1.end());
  static_assert(is_same_v<decltype(d2), vector<int>>);
  vector d3 = d2;
  static_assert(is_same_v<decltype(d3), vector<int>>);
  // Zhihao examples
  auto v1 = vector(3ul, std::allocator<int>());
  int result;
  cout << __cxa_demangle(typeid(v1).name(), nullptr, nullptr, &result) << endl;
  auto v2 = vector(3ul, 4, std::allocator<int>());
  cout << __cxa_demangle(typeid(v2).name(), nullptr, nullptr, &result) << endl;
}
  
void test_map()
{
  // auto il = { {"foo"s, 1}, {"bar"s, 2}, {"baz"s, 3}, {"quux"s, 4}}; // Error. Prevents deducing maps from braced initializers
  //  map m1({ {"foo"s, 1}, {"bar"s, 2}, {"baz"s, 3}, {"quux"s, 4}}); // Like this
  map<string, int> m1 = { {"foo"s, 1}, {"bar"s, 2}, {"baz"s, 3}, {"quux"s, 4}};
  map m2(m1.begin(), m1.end());
  static_assert(is_same_v<decltype(m2), map<string, int>>);
  map m3 = m2;
  static_assert(is_same_v<decltype(m3), map<string, int>>);
}
  
void test_unordered_map()
{
  unordered_map<string, int> m1 = { {"foo"s, 1}, {"bar"s, 2}, {"baz"s, 3}, {"quux"s, 4}};
  std::hash<string>()("foo"s);
  unordered_map m2(m1.begin(), m1.end());
  static_assert(is_same_v<decltype(m2), unordered_map<string, int>>);
  unordered_map m3 = m2;
  static_assert(is_same_v<decltype(m3), unordered_map<string, int>>);
}
  
void test_multimap()
{
  multimap<string, int> m1 = { {"foo"s, 1}, {"bar"s, 2}, {"baz"s, 3}, {"quux"s, 4}};
  multimap m2(m1.begin(), m1.end());
  static_assert(is_same_v<decltype(m2), multimap<string, int>>);
  multimap m3 = m2;
  static_assert(is_same_v<decltype(m3), multimap<string, int>>);
}

void test_unordered_multimap()
{
  unordered_multimap<string, int> m1 = { {"foo"s, 1}, {"bar"s, 2}, {"baz"s, 3}, {"quux"s, 4}};
  unordered_multimap m2(m1.begin(), m1.end());
  static_assert(is_same_v<decltype(m2), unordered_multimap<string, int>>);
  unordered_multimap m3 = m2;
  static_assert(is_same_v<decltype(m3), unordered_multimap<string, int>>);
}

void test_set()
{
  set s1({1, 2, 3, 4});
  set s2(s1.begin(), s1.end());
  static_assert(is_same_v<decltype(s2), set<int>>);
  set s3 = s2;
  static_assert(is_same_v<decltype(s3), set<int>>);
}

void test_unordered_set()
{
  unordered_set s1({1, 2, 3, 4});
  unordered_set s2(s1.begin(), s1.end());
  static_assert(is_same_v<decltype(s2), unordered_set<int>>);
  unordered_set s3 = s2;
  static_assert(is_same_v<decltype(s3), unordered_set<int>>);
}

void test_multiset()
{
  multiset s1({1, 2, 3, 4});
  multiset s2(s1.begin(), s1.end());
  static_assert(is_same_v<decltype(s2), multiset<int>>);
  multiset s3 = s2;
  static_assert(is_same_v<decltype(s3), multiset<int>>);
}

void test_unordered_multiset()
{
  unordered_multiset s1({1, 2, 3, 4});
  unordered_multiset s2(s1.begin(), s1.end());
  static_assert(is_same_v<decltype(s2), unordered_multiset<int>>);
  unordered_multiset s3 = s2;
  static_assert(is_same_v<decltype(s3), unordered_multiset<int>>);
}

void test_queue()
{
  deque d({3, 1, 4, 1, 5});
  queue q(d, allocator<int>());
  static_assert(is_same_v<decltype(q), queue<int>>);
}

void test_priority_queue()
{
  deque d({3, 1, 4, 1, 5});
  priority_queue q(d.begin(), d.end());
  static_assert(is_same_v<decltype(q), priority_queue<int>>);
}

void test_stack()
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
  reverse_iterator ri(v.end());
  static_assert(is_same_v<decltype(ri), reverse_iterator<vector<int>::iterator>>);
  assert(*ri == 3);
  back_insert_iterator biiv(v);
  static_assert(is_same_v<decltype(biiv), back_insert_iterator<vector<int>>>);
  back_insert_iterator biiv2(v);
  static_assert(is_same_v<decltype(biiv2), back_insert_iterator<vector<int>>>);
  front_insert_iterator fiiv(v);
  static_assert(is_same_v<decltype(fiiv), front_insert_iterator<vector<int>>>);
  list l({1, 2, 3}); // Why can't I say list l{1, 2, 3}? I can say list<int> l{1, 2, 3}
  insert_iterator ii(l, next(l.begin())); // Based on http://en.cppreference.com/w/cpp/iterator/insert_iterator
  static_assert(is_same_v<decltype(ii), insert_iterator<list<int>>>);
  move_iterator mi(l.begin());
  static_assert(is_same_v<decltype(mi), move_iterator<list<int>::iterator>>);

  // Stream iterators (24.6)
  istreambuf_iterator<char> isbi(cin);
  static_assert(is_same_v<decltype(isbi), istreambuf_iterator<char>>);
  
}


void test_complex()
{
  complex c(2.5, 3.7);
  static_assert(is_same_v<decltype(c), complex<double>>);
}

void test_random()
{
  // Engines are not deducible because they all have non-type parameters
  
  uniform_int_distribution uid(5);
  static_assert(is_same_v<decltype(uid), uniform_int_distribution<int>>);

  uniform_real_distribution urd{};
  static_assert(is_same_v<decltype(urd), uniform_real_distribution<double>>);

  uniform_real_distribution urdf{2.6f};
  static_assert(is_same_v<decltype(urdf), uniform_real_distribution<float>>);

  // Bernoulli distribution is not a template
  
  binomial_distribution bd{7L};
  static_assert(is_same_v<decltype(bd), binomial_distribution<long>>);

  // geometric_distribution is not deducible
  
  negative_binomial_distribution nbd{7L, 0.2};
  static_assert(is_same_v<decltype(nbd), negative_binomial_distribution<long>>);

  // poisson_distribution is not deducible
  
  exponential_distribution ed{1.1};
  static_assert(is_same_v<decltype(ed), exponential_distribution<double>>);

  gamma_distribution gd{};
  static_assert(is_same_v<decltype(gd), gamma_distribution<double>>);

  gamma_distribution gdf{2.5f, .7f};
  static_assert(is_same_v<decltype(gdf), gamma_distribution<float>>);

  weibull_distribution wd{};
  static_assert(is_same_v<decltype(wd), weibull_distribution<double>>);

  weibull_distribution wdf{2.5f, .7f};
  static_assert(is_same_v<decltype(wdf), weibull_distribution<float>>);

  extreme_value_distribution evd{};
  static_assert(is_same_v<decltype(evd), extreme_value_distribution<double>>);

  extreme_value_distribution evdf{2.5f, .7f};
  static_assert(is_same_v<decltype(evdf), extreme_value_distribution<float>>);

  normal_distribution nd{};
  static_assert(is_same_v<decltype(nd), normal_distribution<double>>);

  normal_distribution ndf{2.5f, .7f};
  static_assert(is_same_v<decltype(ndf), normal_distribution<float>>);

  lognormal_distribution lnd{};
  static_assert(is_same_v<decltype(lnd), lognormal_distribution<double>>);

  lognormal_distribution lndf{2.5f, .7f};
  static_assert(is_same_v<decltype(lndf), lognormal_distribution<float>>);

  chi_squared_distribution csd{};
  static_assert(is_same_v<decltype(csd), chi_squared_distribution<double>>);

  chi_squared_distribution csdf(2.5f);
  static_assert(is_same_v<decltype(csdf), chi_squared_distribution<float>>);  

  cauchy_distribution cd{};
  static_assert(is_same_v<decltype(cd), cauchy_distribution<double>>);

  cauchy_distribution cdf{2.5f, .7f};
  static_assert(is_same_v<decltype(cdf), cauchy_distribution<float>>);

  fisher_f_distribution ffd{};
  static_assert(is_same_v<decltype(ffd), fisher_f_distribution<double>>);

  fisher_f_distribution ffdf{2.5f, .7f};
  static_assert(is_same_v<decltype(ffdf), fisher_f_distribution<float>>);

  student_t_distribution std{};
  static_assert(is_same_v<decltype(std), student_t_distribution<double>>);

  student_t_distribution stdf(2.5f);
  static_assert(is_same_v<decltype(stdf), student_t_distribution<float>>);

  // discrete_distribution is not deducible
  // piecewise_constant_distribution is not deducible
  // piecewise_linear_distribution is not deducible
}

void test_valarray()
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
  basic_ios bios{cout.rdbuf()};
  // basic_streambuf constructors are not deducible
  basic_istream bis{cin.rdbuf()};
  basic_ostream bos{cout.rdbuf()};

  string str = "foo";
  auto bsb = basic_stringbuf(str);
  auto biss = basic_istringstream(str);
  auto boss = basic_ostringstream(str);

  // basic_filebuf, basic_ifstream, basic_ofstream, and basic_fstream constructors are not deducible
  // filesystem classes are not templates
}

void test_regex()
{
  basic_regex br("f.*r");
  basic_regex br2("f.*r", 4);
  basic_regex br3("f.*r"s);
  string s = "f.*r";
  basic_regex br4(s.begin(), s.end());
  basic_regex br5(s.begin(), s.end(), regex_constants::icase);
  string s2 = "xfoobary";
  regex_iterator ri(s2.begin(), s2.end(), br5);
  regex_token_iterator rti(s2.begin(), s2.end(), br5);
}

void test_atomic()
{
  atomic a = 3;
}

void test_thread()
{
  mutex m;
  {
    lock_guard lg(m);
  }
  {
    unique_lock ul(m);
  }
  shared_mutex sm;
  {
    shared_lock sl(sm);
  }
  {
    // lock_guard lg2(m, sm); // Not yet supported by g++ in either lock_guard or scoped_lock
  }
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
  A a(3);
  function af(&decltype(a)::foo);
  cout << af(a) << endl;

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


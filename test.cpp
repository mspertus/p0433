#include<iostream>
#include<memory>
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
#include<complex>

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
  T foo() { return t; }
  A(T t) : t{t} {}
  using pointer = T *;
  T t;
};

template<typename T> struct IL {  IL(initializer_list<T> il) {}};
void test_initializer_list()
{
  IL<int> il = { 1, 2, 3}; // Works
  // IL il2 = { 1, 2, 3};     // Error: substitution failed
  IL il3({1, 2, 3});       // Works
  static_assert(is_same_v<decltype(il3), IL<int>>);
}

// Adapted from http://stackoverflow.com/questions/13181248/construct-inner-allocator-from-a-scoped-allocator-adaptor
typedef allocator<int> Alloc1;
typedef vector<int, Alloc1 > Vec1;

scoped_allocator_adaptor alloc2{Alloc1(), Alloc1()};
typedef vector<Vec1, decltype(alloc2) > Vec2;

// Adapted from example at
// http://en.cppreference.com/w/cpp/experimental/boyer_moore_searcher
void test_searchers()
{
    std::string in = "Lorem ipsum dolor sit amet, consectetur adipiscing elit,"
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

void test_complex()
{
  complex c(2.5, 3.7);
  static_assert(is_same_v<decltype(c), complex<double>>);
}

unique_ptr up(new A<int>(3));
shared_ptr sp(new A<int>(3));
int main()
{
  int i{5};
  basic_string_view bsv("foo");
  auto rw = reference_wrapper(i);
  allocator al = allocator<int>();
  A a(3);
  function af(&decltype(a)::foo);
  cout << af(a) << endl;
  tuple t(3, 7.5);
  pair p(3, 7.5);
  optional o(7);
  static_assert(is_same_v<decltype(o), optional<int>>);
  optional o2 = o;
  static_assert(is_same_v<decltype(o2), optional<int>>);

  cout << up->t << endl;
  cout << "Hello, world" << endl;
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
  return 0;
}


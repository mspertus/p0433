#include<cassert>
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
#include<iterator>
#include<complex>
#include<random>
#include<valarray>
#include<sstream>
#include<fstream>
#include<regex>

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
  test_iterators();
  test_valarray();
  test_io();
  test_regex();
  return 0;
}


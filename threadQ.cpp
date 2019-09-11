#include <iostream>
#include <string>

#include <queue>
#include <functional>
#include <tuple>
#include <memory>

// TO DO : Make sure to implement locking of thread_queue in the place that it is being handled
//         This is to allow thread safety and exception safety of the pop and top functions

/*
 * Thread Queue class, this stores functions to processed by a list
 * of threads. These threads will pop off functions from this queue
 * so that they can be run concurrently.
 *
 * Notes:
 *   Only stores void functions which take no parameters in the queue
 *     this is so that any function grabbed wont need to be taken 
 *     in any order other than the way it is given in the queue
 *   This being said, we can push functions with return types, but the
 *     return will be absorbed.
 *   We can also pass functions with parameters, but these params must
 *     also be passed so that we can create a new void parameterless
 *     function that will be placed into the queue.
 *
 */

// MAJOR ISSUE ! NEED TO PROCESS THINGS IN ORDER GIVEN ! this is in case of state change before some other function uses a shared resource.

// TO DO : clean up and document how to use
/*class thread_queue {
  
    // Non-Copyable
    thread_queue(const thread_queue&);
    thread_queue& operator=(const thread_queue&);

    template <typename Ret, typename... Params>
    struct callable {

      Ret (&func)(Params...);
      std::tuple<Params...> params;

      template <typename... Params2>
      callable(Ret (&func1)(Params...), Params2&&... params):
        func(func1),
        params(std::foward<Params2>(params)...) { }

      void operator()() {
        std::apply(func, std::move(params));
      }

    };

    std::queue<std::function<void()>> thread_q_; // The actual queue

  public:

    // TO DO : Constructor, Destructor, move operations ?
    thread_queue() = default;

    // TO DO : Use move semantics ?
    void push(std::function<void()> func) { thread_q_.push(func); } // push member to add void() function to queue

    // Cast any other, ie non void() functions into void() functions by using
    //   lambda to call the function with passed params, and absorb the return value
    template <typename Ret, typename ... Params>
    void push(Ret (&func), Params&&... params) {
      std::function<void()> temp = std::bind(func, std::forward<Params>(params)...); 
      push(std::move(temp));
    }

    // TO DO : create an ability to assign the return value to some passed reference ?
    //         check how these function pushes act with things like member functions,
    //           other functions in different namespaces, ...

    // Thread Safety Note : Do not use pop and front without locking mutex and grouping them !
    void pop() { thread_q_.pop(); } 

    std::function<void()> front() { return thread_q_.front(); } // TO DO : Return a reference?

    bool empty() { return thread_q_.empty(); }

    // TO DO : Other member functions from queue ?


    // TO DO : Somehow allow return values to be returned ?
};
*/
// structs? template objects? other callables ?`

// Template initialization
template<typename T>
class move_only_function;

// Template with return type and arg types passed to move_only_function
template<typename R, typename... Args>
class move_only_function<R(Args...)> {

  // Base struct to allow polymorphism of callable template object
  struct callable_base {
    virtual R operator()(Args... args) = 0;
    virtual ~callable_base() = default;
  };

  // Callable template object containing callable
  template<typename F>
  struct callable : public callable_base {
    F func;
    callable(const F& f): func(f) { }
    callable(F&& f): func(std::move(f)) { }

    virtual R operator()(Args... args) override { // TO DO : Return value
      return static_cast<R>(func(args...));
    }
  };

  // move_only_function's function pointer
  std::unique_ptr<callable_base> func;

public:

  // Move constuctor
  move_only_function(move_only_function&& move_f): func(std::move(move_f.func)) { }

  // Template move constuctor 
  template<typename F>
  move_only_function(F&& f): func(std::make_unique<callable<F>>(std::forward<F>(f))) { }

  template<typename... Args2>
  R operator()(Args2&&... args) { // TO DO : And here
    return (*func)(std::forward<Args2>(args)...);
  }

};

// Queue of functions using move semantics for efficiency, these functions are given their params in advance so they can be invoked later with () operator
//   This instantiates move_only_function<void()> and each unique function signature added instanciates the member operators and template move constructors
//
//   TO DO : Future optimization would be to include a small buffer optimization
class thread_queue {

  // The thread queue
  std::queue<move_only_function<void()>> thread_q_;

public:

  thread_queue() = default;

  void push(move_only_function<void()> func) { thread_q_.push(std::move(func)); }

  template<typename R, typename... Params>
  void push(R (&func), Params&&... params) {
    thread_q_.push(move_only_function<void()>{
      [&, tup=std::make_tuple(std::forward<Params>(params)...)]() mutable {
          return std::apply(func, std::move(tup));
      }});
  }

  void pop() { thread_q_.pop(); }

  move_only_function<void()>& front() { return thread_q_.front(); }

};

class callable_obj {

  int j;

  public:

    callable_obj(): j(19) { }

    int operator()() { std::cout << "callable object" << j << std::endl; }

};

class testclass {

  int mem1;
  std::string mem2;

  public:

    testclass(): mem1(5), mem2("cats") { }

    // TO DO : Add ability to call non-static member functions
    static void mfunc1() { std::cout << "Inside of mfunc1" << std::endl; }
    void mfunc2(int g) { std::cout << "Inside of mfunc2" << mem1 + g << std::endl; }
    int mfunc3() { std::cout << "Inside of mfunc3" << mem2 << std::endl; }
    int mfunc4(int c) { std::cout << "Inside of mfunc3" << c << std::endl; }

};

void func1() {

  std::cout << "Inside of func1" << std::endl;

}

void func2(int a) {

  std::cout << "Inside of func2 " << a << std::endl;

}

int func3() {
 
  std::cout << "Inside of func3" << std::endl;
  return 5;

}

int func4(int a, int b) {

  std::cout << "Inside of func4 " << a + b << std::endl;
  return a + b;

}

static void sfunc1(int a) {
  int j = a;
  std::cout << "Inside of sfunc1 " << a << j << std::endl;
}

struct MoveOnly {
    MoveOnly() {}
    MoveOnly(MoveOnly&&) {}
};

void func5(MoveOnly m) {
    std::cout << "func5\n";
}

int main() {
  thread_queue test;
 // WORKS FOR OBJECTS OUT OF SCOPE
 {
  testclass tester1;
  int i = 10;
  int j = 5;
  int k = 2;
  test.push(func1);
  test.push(func2, i);
  test.push(func3);
  test.push(func4, j, k);
 // test.push(tester1.mfunc1);
 // test.push(std::mem_fun(tester1.mfunc2), 5);
 // test.push(tester1.mfunc3);
 // test.push(tester1.mfunc4, k);
  std::cout << i << " " << j << " " << k << std::endl;
 }
  test.front()();
  test.pop();
  test.front()();
  test.pop();
  test.front()();
  test.pop();
  test.front()();
  test.pop();
  //test.front()();
  //test.pop();
  //test.front()();
  //test.pop();
  //test.front()();
  //test.pop();
  //test.front()();
  //test.pop();

  int x = 7;
  test.push(sfunc1, 4);
  test.push(sfunc1, x);
  test.push(sfunc1, 3);
  test.front()();
  test.pop();
  test.front()();
  test.pop();
  test.front()();
  test.pop();

  test.push(func1);
  test.push(func2, 10);
  test.push(func3);
  test.push(func4, 5, 2);
  test.front()();
  test.pop();
  test.front()();
  test.pop();
  test.front()();
  test.pop();
  test.front()();
  test.pop();

  test.push(func5, MoveOnly{});
  test.front()();
  test.pop();

  std::cout << "IT WORKED BABY!" << std::endl;

  return 0;

}

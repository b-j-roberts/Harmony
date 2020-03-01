#include <iostream> // Testing includes
#include <string>

#include <queue> // Necessary includes
#include <functional>
#include <tuple>
#include <memory>

// g++ -c -g -std=c++17 threadQ.cpp

// TO DO : Make sure to implement locking of thread_queue in the place that it is being handled
//         This is to allow thread safety and exception safety of the pop and top functions
// TO DO : Create some kind of locking class so that there is not multiple of the same class functions. etc
//         taking up processes in which they cant be invoked at the same time

/*
 * Task Queue class, this stores functions to processed by a list
 * of threads. These threads will pop off functions from this queue
 * so that they can be run concurrently.
 *
 * Notes:
 *   Only stores void functions which take no parameters in the queue
 *   This being said, we can push functions with return types, but the
 *     return will be absorbed.
 *   We can also pass functions with parameters, but these params must
 *     also be passed so that we can create a new void parameterless
 *     function that will be placed into the queue.
 *
 */

// MAJOR ISSUE ! NEED TO PROCESS THINGS IN ORDER GIVEN ! this is in case of state change before some other function uses a shared resource.
// MAJOR ISSUE ! Passing classes as ref, for member function calls, seem to wipe memory of some types.
//   Possible fix ? Create a function member class that holds shared pointer to class type passed, holding class
//
// TO DO : Consider using a class to store references to parameters and actually invoking the parameters later ?
// TO DO : Consider having an option to save return value of function to some passed parameter ?

// Template initialization
template<typename T>
class move_only_function;

// Template with return type and arg types passed to move_only_function
template<typename R, typename... Args>
class move_only_function<R(Args...)> {

  // Base struct to allow polymorphism of callable template object
  struct callable_base {
    virtual void operator()(Args... args) = 0;
    virtual ~callable_base() = default;
  };

  // Callable template object containing callable
  template<typename F>
  struct callable : public callable_base {
    F func;
    callable(const F& f): func(f) { }
    callable(F&& f): func(std::move(f)) { }

    virtual void operator()(Args... args) override {
      func(args...);
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
  void operator()(Args2&&... args) {
    (*func)(std::forward<Args2>(args)...);
  }

};

// Queue of functions using move semantics for efficiency, these functions are given their params in advance so they can be invoked later with () operator
//   This instantiates move_only_function<void()> and each unique function signature added instanciates the member operators and template move constructors
//
//   TO DO : Future optimization would be to include a small buffer optimization
class task_queue {

  // The task queue
  std::queue<move_only_function<void()>> task_q_;

public:

  task_queue() = default;

  void push(move_only_function<void()> func) { task_q_.push(std::move(func)); }

  template<typename Fn, typename... Params>
  void push(Fn&& fn, Params&&... params) {
    task_q_.push(move_only_function<void()>{
      [fun=std::forward<Fn>(fn), tup=std::make_tuple(std::forward<Params>(params)...)]() mutable {
          return std::apply(std::move(fun), std::move(tup));
      }});
  }

  void pop() { task_q_.pop(); }

  move_only_function<void()>& front() { return task_q_.front(); }

};

/*
class testclass {

  int mem1;
  std::string mem2;
  int mem3;

  public:

    testclass(int a): mem1(a), mem2("cats"), mem3(6) { 
      std::cout << "cats constructed" << std::endl;
    }

    static void mfunc1() { std::cout << "Inside of mfunc1" << std::endl; }
    void mfunc2(int g) { std::cout << "Inside of mfunc2" << mem2 << " " << mem1 + g << " " << mem3 << std::endl; }
    int mfunc3() { 
      mem1 = 5; 
      std::cout << "Inside of mfunc3" << mem2 << " " << mem1 << std::endl; 
      //mem2 = "other";
    }
    int mfunc4(int c) { std::cout << "Inside of mfunc3" << c << std::endl; }
    void operator()(int k) { std::cout << "Op worked" << k << std::endl;}
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
  task_queue test;
 // WORKS FOR OBJECTS OUT OF SCOPE
  testclass tester1(3);
 {
  int i = 10;
  int j = 5;
  int k = 2;
  test.push(func1);
  test.push(func2, i);
  test.push(func3);
  test.push(func4, j, k);
  test.push(tester1.mfunc1);
  test.push(&testclass::mfunc2, tester1, 4);
  test.push(&testclass::mfunc3, &tester1);
  test.push(&testclass::mfunc2, std::move(tester1), 5);
  test.push(&testclass::mfunc2, std::ref(tester1), 5);
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
  test.front()();
  test.pop();
  test.front()();
  test.pop();
  test.front()();
  test.pop();
  test.front()();
  test.pop();
  test.front()();
  test.pop();

  tester1.mfunc2(2);
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

  int thisone = 5;
  test.push([thisonem=std::ref(thisone)](int i){ std::cout << thisonem + i << std::endl; }, 6);
  test.front()();
  thisone=4;
  test.front()();
  test.pop();

  testclass tester2(4);
  test.push(tester2, 3);
  test.front()();
  test.pop();

  std::cout << "IT WORKED BABY!" << std::endl;

  return 0;

}
*/

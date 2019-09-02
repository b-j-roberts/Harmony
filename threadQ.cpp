#include <iostream>
#include <string>

#include <queue>
#include <mutex>
#include <thread>
#include <functional>

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

// TO DO : clean up and document how to use
class thread_queue {

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

  private:

    // Non-Copyable
    thread_queue(const thread_queue&);
    thread_queue& operator=(const thread_queue&);

    std::queue<std::function<void()>> thread_q_; // The actual queue

    // TO DO : Somehow allow return values to be returned ?
};

class testclass {

  int mem1;
  std::string mem2;

  public:

    

}

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

int main() {
  thread_queue test;
 {
  int i = 10;
  int j = 5;
  int k = 2;
  test.push(func1);
  test.push(func2, i);
  test.push(func3);
  test.push(func4, j, k);
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

  std::cout << "IT WORKED BABY!" << std::endl;

  return 0;

}

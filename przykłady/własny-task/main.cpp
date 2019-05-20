/*
main.cpp: This file is part of the programistamag-coroutines Library.

    Copyright (C) Dawid Pilarski, PanicSoftware 2018-2019
    Distributed under the BSD 3-clause License.
    (See Milli/LICENSE)

    programistamag-coroutines IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
    IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
    FITNESS FOR A PARTICULAR PURPOSE, TITLE AND NON-INFRINGEMENT. IN NO EVENT
    SHALL THE COPYRIGHT HOLDERS OR ANYONE DISTRIBUTING THE SOFTWARE BE LIABLE
    FOR ANY DAMAGES OR OTHER LIABILITY, WHETHER IN CONTRACT, TORT OR OTHERWISE,
    ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER
    DEALINGS IN THE SOFTWARE.
*/

#include <cppcoro/single_consumer_event.hpp>
#include <experimental/coroutine>
#include <iostream>
#include <optional>
#include <vector>

template <typename T> class task {
public:
  struct promise_type;
  using coro_handle_t = std::experimental::coroutine_handle<promise_type>;
  struct final_task_awaiter;
  task(coro_handle_t handle) : coro_handle_(handle) {}

  task(task&& rhs) : coro_handle_(rhs.coro_handle_){
    rhs.coro_handle_=nullptr;
  }
  ~task(){
    if(coro_handle_)
      coro_handle_.destroy();
  }

  task& operator=(task&) = delete;
  auto operator co_await();
  T result() { return coro_handle_.promise().result(); }

private:
  coro_handle_t coro_handle_;
};

template <typename T> auto task<T>::operator co_await() {
  struct co_await_task_awaiter {
    task::coro_handle_t coroutine_handle;

    bool await_ready() { return false; }

    auto
    await_suspend(std::experimental::coroutine_handle<> calling_coro_handle) {
      coroutine_handle.promise().save_continuation(calling_coro_handle);
      return coroutine_handle;
    }

    T await_resume() { return coroutine_handle.promise().result(); }
  };
  assert(coro_handle_);
  return co_await_task_awaiter{coro_handle_};
}

template <typename T> struct task<T>::final_task_awaiter {
  task::coro_handle_t coroutine_handle;

  bool await_ready() { return false; }

  std::experimental::coroutine_handle<>
  await_suspend(task::coro_handle_t coro) {
    return coroutine_handle.promise().continuation();
  }

  void await_resume() {}
};

template <typename T> struct task<T>::promise_type {
  using coroutine_t = std::experimental::coroutine_handle<>;

public:
  task get_return_object() {
    return {task::coro_handle_t::from_promise(*this)};
  }

  std::experimental::suspend_always initial_suspend() { return {}; }

  auto final_suspend() {
    return task::final_task_awaiter{task::coro_handle_t::from_promise(*this)};
  }

  void return_value(T value) { value_ = std::move(value); }

  void unhandled_exception() { throw; }

  std::experimental::coroutine_handle<> continuation() {
    return another_coroutine_;
  }

  std::experimental::coroutine_handle<>
  save_continuation(std::experimental::coroutine_handle<> coro) {
    return another_coroutine_ = coro;
  }

  T result() { return value_; }

private:
  T value_;
  coroutine_t another_coroutine_;
};

struct scheduler {
  struct promise_type;
  using this_coro_t = std::experimental::coroutine_handle<promise_type>;
  scheduler(this_coro_t this_coro) : coro(this_coro) {}
  scheduler(const scheduler &rhs) = default;

  struct promise_type {
    scheduler get_return_object() { return this_coro_t::from_promise(*this); }
    std::experimental::suspend_never initial_suspend() { return {}; }
    std::experimental::suspend_always final_suspend() { return {}; }
    void unhandled_exception() { throw; }
    void return_void() {}
  };

  this_coro_t coro;
};

scheduler start_task(task<int>& task) {
  int value = co_await task;
  std::cout << "finished waiting for: " << value << std::endl;
}

template <typename... Task>
void sched(Task&&... tasks) {
  std::vector<scheduler> schedulers;

  (start_task(tasks),...);
}

int main() {
  cppcoro::single_consumer_event event;

  sched([&event]() -> task<int> {
           co_await event;
           std::cout << "pre resetting event" << std::endl;
           event.reset();
           std::cout << "post resetting event" << std::endl;
           co_await event;
           event.reset();
           co_await event;
           event.reset();
           co_return 1;
         }(),

         [&event]() -> task<int> {
           event.set();
           co_return 2;
         }(),

         [&event]() -> task<int> {
           co_return co_await[&event]()
               ->task<int> { // zagnieżdżona lambda wyłącznie po to, aby
                             // sprawdzić, czy zagnieżdżone taski również
                             // działają poprawnie
             event.set();
             co_return 3;
           }
           ();
         }(),

         [&event]() -> task<int> {
           co_return co_await[&event]()->task<int> {
             event.set();
             co_return 4;
           }
           ();
         }());
}
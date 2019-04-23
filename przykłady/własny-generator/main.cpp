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

#include <iostream>
#include <milli/repeat.hpp>

#include <experimental/coroutine> // potrzebne dla coroutine_handle

template <typename T> // T odpowiada za typ wartości, który będziemy otrzymywać
                      // z generatora
class generator {
public:
  T next() {
    coro_handle_.resume();
    return coro_handle_.promise()
        .recent_value; // funkcja promise() zwraca rejerencję do obiektu promise
                       // utworzonego wewnątrz korutyny, a recent_value to jej
                       // pole, które sobie zdefiniujemy
  }

  ~generator() {
    coro_handle_
        .destroy(); // niszczy obiekt korutyny - wywołuje destruktory wszystkich
                    // zmiennych korutyny oraz dealokuje jej pamięć
  }

  struct promise_type; // wczesna deklaracja typu promise_type
private:
  using coro_handle_t = std::experimental::coroutine_handle<promise_type>;
  coro_handle_t coro_handle_;
  generator(coro_handle_t handle) : coro_handle_(handle) {}
};

template <typename T> struct generator<T>::promise_type {
  T recent_value;

  generator<T> get_return_object() {
    return generator<T>::coro_handle_t::from_promise(*this);
  }

  std::experimental::suspend_always initial_suspend() { return {}; }
  std::experimental::suspend_always final_suspend() { return {}; }

  void return_void() {} // nasz generator nie może zwracać wartości

  // obsługa zwracania poprzez co_yield. Zwracanie suspend_always spowoduje
  // wstrzymanie korutyny.
  std::experimental::suspend_always yield_value(T value) {
    recent_value = value;
    return {};
  }

  void unhandled_exception() {
    throw; // propagowanie wyjątku do wywołującego/wznawiającego korutynę
  }
};

generator<unsigned int> positives_no_zero() {
  unsigned int current = 0;
  while (true) {
    co_yield++ current;
  }
}

int main() {
  auto generator = positives_no_zero();
  milli::repeat(10, [&]() { std::cout << generator.next() << std::endl; });
}
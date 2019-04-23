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


#include <cppcoro/generator.hpp>
#include <iostream>
#include <array>

class memory_counter{
public:
  static memory_counter& get_instance(){
    static memory_counter object;
    return object;
  }

  void reset() noexcept {
    allocated_bytes = 0;
  }

  void add_memory(std::size_t memory_in_bytes){
    allocated_bytes += memory_in_bytes;
  }

  std::size_t memory(){
    return allocated_bytes;
  }

  memory_counter(const memory_counter&) = delete;
private:
  memory_counter() = default;

  std::size_t allocated_bytes = 0;
};

void* operator new(std::size_t size){
  memory_counter::get_instance().add_memory(size);
  return std::malloc(size);
}

void operator delete(void* ptr) noexcept {
  free(ptr);
}

cppcoro::generator<unsigned long long> empty_coroutine(){
  std::array arr{0ull, 0ull};
  unsigned long long result=0;

  do {
    co_yield result;
    if(result == 0 and arr == std::array{0ull, 0ull})
      result = 1;
    else if (result == 1 and arr == std::array{0ull, 0ull})
      arr = {0, 1};
    else{
      arr[0] = arr[1];
      arr[1] = result;
      result = arr[0] + arr[1];
    }
  } while (result >= arr[1]);
}

int main(){
  memory_counter::get_instance().reset();// czyść alokacje zmiennych globalnych

  auto coro = empty_coroutine();

  std::cout << memory_counter::get_instance().memory() << std::endl; // 128 on my arch
  memory_counter::get_instance().reset();
}
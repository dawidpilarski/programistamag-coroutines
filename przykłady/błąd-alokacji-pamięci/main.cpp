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

#include <cstddef>
#include <experimental/coroutine>
#include <iostream>
#include <new>


#define PRINT_ON_MEM_ALLOC

void* operator new(std::size_t){
  throw std::bad_alloc();
}

void* operator new(std::size_t, const std::nothrow_t&) noexcept {
  return nullptr;
}

struct test{
  struct promise_type{

    test get_return_object(){
      return {};
    }

#ifdef PRINT_ON_MEM_ALLOC
    static test get_return_object_on_allocation_failure(){
      std::cout << "Nie można zaalokować pamięci" << std::endl;
      throw std::bad_alloc();
    }
#endif

    std::experimental::suspend_always initial_suspend(){return {};}
    std::experimental::suspend_always final_suspend(){return {};}
    void unhandled_exception(){throw;}
    void return_void(){}
  };
};

test coro(){
  std::cout << "Nigdy nie powinienem tu być" << std::endl;
  co_return; // żeby funkcja była korutyną
}

int main(){
  coro();
}
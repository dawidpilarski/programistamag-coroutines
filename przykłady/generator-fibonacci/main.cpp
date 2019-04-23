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
#include <array>
#include <cppcoro/generator.hpp>

void fibonacci() {
  std::array arr{0ull, 0ull};
  unsigned long long result=0;

  do {
    std::cout << result << ", " << std::endl;
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

cppcoro::generator<unsigned long long> fibonacci_gen() {
  std::array arr{0ull, 0ull};
  unsigned long long result=0;

  do {
    co_yield result; // TU JEST ZMIANA!
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



int main() {
  fibonacci(); // wyświetla liczby fibonacciego

  // to samo co powyżej, ale jako leniwy (ang.lazy) generator
  for (auto fibonacci_number : fibonacci_gen()) {
    std::cout << fibonacci_number << std::endl;
  }
}
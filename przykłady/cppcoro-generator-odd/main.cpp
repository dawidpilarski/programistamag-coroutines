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

cppcoro::generator<unsigned long long> odd() {
  auto current_odd = 1ull;
  while (true) {
    co_yield current_odd;
    auto new_odd = current_odd + 2;
    if (new_odd < current_odd)
      break;
    current_odd = new_odd;
  }
}

int main() {

  const auto uzyj_normalne = false;

  if constexpr (uzyj_normalne)// normalne użytkowanie
  {
    auto generator = odd();
    for (auto value : generator)
      std::cout << "odd value:" << value << std::endl;
  } else {
    // podstęp
    auto generator = odd();
    auto it = generator.begin();
    auto it2 = generator.begin();

    std::cout << *it << std::endl; // wypisze 3 zamiast 1
    std::cout << *it2 << std::endl; // wypisze 3 zamiast 1
  }


}
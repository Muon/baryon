# Baryon

This is a build order planner for Achron. To solve for a build order, you must
define it as in `problems/buildorder.hpp`.

There are 3 solvers available: A\*, DFBB and IDA\*. Use of the former two is
recommended, as IDA\* has proven quite slow on this search space.

For optimality, the A\* solver needs the heuristic to be both admissible and
monotonic, due to the use of a closed set. The DFBB solver requires only
admissibility for optimality and can provide approximate solutions, but is
somewhat slower and you must give an upper bound on the length of the plan.
IDA\* requires the heuristic to be admissible for optimality.

## License

The MIT License (MIT)

Copyright (c) 2014 Mak Nazecic-Andrlon

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in
all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
THE SOFTWARE.

A simple program to run post's tag system on (100)^n and check how many steps
it takes before the system halts.

## How to compile
Note: This program uses some Intel AVX2 intrinsics by default. If your computer
does not have a Intel CPU which supports AVX2, simply remove all the
`-mavx2` flags from the file `CMakeLists.txt` before proceeding.

```shell script
cmake -Bcmake-release-build -H. -DCMAKE_BUILD_TYPE=Release -DCMAKE_CXX_COMPILER=/path/to/c++_compiler
cmake -Bcmake-debug-build -H. -DCMAKE_BUILD_TYPE=Debug -DCMAKE_CXX_COMPILER=/path/to/c++_compiler
```

Usually, the path to your c++ compiler is `/usr/bin/clang++` or `/usr/bin/g++`.

To make a debug build, run
```shell script
cmake --build cmake-debug-build --target posts_cpp -- -j 2
```

To make a release build, run
```shell script
cmake --build cmake-release-build --target posts_cpp -- -j 2
```

## How to run
First, `cd` into either `cmake-debug-build` or `cmake-release-build` for 
the corresponding build.

Run
```shell script
./posts_cpp start end
```

Then the program will run experiments for `start <= n <= end` and print
the results to stdout.

## Example
`./posts_cpp 1 30` produces the following output
```
[n=1] finished, steps=4, cycleLength=2, maxWordLength=6, time=0.246847s
[n=2] finished, steps=15, cycleLength=6, maxWordLength=16, time=0.245945s
[n=3] finished, steps=10, cycleLength=6, maxWordLength=16, time=0.244261s
[n=4] finished, steps=25, cycleLength=6, maxWordLength=22, time=0.243558s
[n=5] finished, steps=411, cycleLength=0, maxWordLength=56, time=0.238600s
[n=6] finished, steps=47, cycleLength=10, maxWordLength=34, time=0.244720s
[n=7] finished, steps=2128, cycleLength=28, maxWordLength=176, time=0.245933s
[n=8] finished, steps=853, cycleLength=6, maxWordLength=76, time=0.244670s
[n=9] finished, steps=372, cycleLength=10, maxWordLength=62, time=0.245355s
[n=10] finished, steps=2805, cycleLength=6, maxWordLength=208, time=0.245332s
[n=11] finished, steps=366, cycleLength=6, maxWordLength=62, time=0.245688s
[n=12] finished, steps=2603, cycleLength=6, maxWordLength=208, time=0.244392s
[n=13] finished, steps=703, cycleLength=0, maxWordLength=68, time=0.240050s
[n=14] finished, steps=37912, cycleLength=0, maxWordLength=768, time=0.238164s
[n=15] finished, steps=612, cycleLength=6, maxWordLength=104, time=0.247845s
[n=16] finished, steps=127, cycleLength=28, maxWordLength=88, time=0.249598s
[n=17] finished, steps=998, cycleLength=10, maxWordLength=106, time=0.248945s
[n=18] finished, steps=2401, cycleLength=6, maxWordLength=224, time=0.248066s
[n=19] finished, steps=1200, cycleLength=10, maxWordLength=146, time=0.245961s
[n=20] finished, steps=623, cycleLength=6, maxWordLength=134, time=0.260735s
[n=21] finished, steps=5280, cycleLength=6, maxWordLength=226, time=0.259273s
[n=22] finished, steps=1778, cycleLength=0, maxWordLength=172, time=0.243525s
[n=23] finished, steps=1462, cycleLength=6, maxWordLength=132, time=0.256216s
[n=24] finished, steps=4346269, cycleLength=6, maxWordLength=4432, time=0.308284s
[n=25] finished, steps=4129, cycleLength=0, maxWordLength=206, time=0.238278s
[n=26] finished, steps=3241, cycleLength=6, maxWordLength=232, time=0.261662s
[n=27] finished, steps=7018, cycleLength=6, maxWordLength=378, time=0.258593s
[n=28] finished, steps=3885, cycleLength=6, maxWordLength=206, time=0.244869s
[n=29] finished, steps=14632, cycleLength=6, maxWordLength=432, time=0.243691s
[n=30] finished, steps=7019, cycleLength=6, maxWordLength=380, time=0.244486s
```

#include <iostream>
#include <cstdio>
#include <boost/dynamic_bitset.hpp>
#include <chrono>
#include "FixedBitSet.h"
#include <boost/lexical_cast.hpp>

using namespace std;

const auto CHECK_INTERVAL = 1000000UL;

inline void step(FixedBitSet &fbs) {
    if (fbs.get(0)) {
        fbs.append1101();
    } else {
        fbs.append00();
    }
    fbs.removeFirst(3);
}

void runExperiment(int n) {
    auto start = chrono::steady_clock::now();

    auto fbs = FixedBitSet::sigma(n); // current step
    auto a = FixedBitSet::sigma(n); // point a for cycle detection
    auto b = FixedBitSet(INT_MAX); // point b for cycle detection
    auto steps = 0UL;
    auto maxWordLength = fbs.getLength();
    auto nextCheckPos = CHECK_INTERVAL;
    auto cycle = false;

    while (fbs.getLength() > 0) {
        maxWordLength = max(maxWordLength, fbs.getLength());

        ++steps;
        step(fbs);

        if (steps < CHECK_INTERVAL) {
            continue;
        } else if (steps == nextCheckPos) {
            nextCheckPos += CHECK_INTERVAL;
            if (steps > CHECK_INTERVAL) a.replaceBy(b);
            b.replaceBy(fbs);
            continue;
        }

        if (fbs == b) {
            cycle = true;
            break;
        }
    }

    auto cycleLength = 0UL;
    if (cycle) {
        do {
            ++cycleLength;
            step(fbs);
        } while (fbs != b);

        // goes back to where a is
        steps = nextCheckPos - 2 * CHECK_INTERVAL;
        b.replaceBy(a);

        // advances b so a and b are "cycleLength" steps apart from each other
        for (auto i = 0UL; i < cycleLength; ++i) {
            step(b);
        }

        // advances a and b simultaneously to find the start of the cycle
        while (a != b) {
            ++steps;
            step(a);
            step(b);
        }
    }

    auto end = chrono::steady_clock::now();
    auto time = chrono::duration<double>(end - start).count();

    printf("[n=%d] finished, steps=%lu, cycleLength=%lu, maxWordLength=%d, time=%fs\n",
           n, steps, cycleLength, maxWordLength, time);
}

int main(int argc, char **argv) {
    if (argc != 3) {
        printf("You need to have 2 arguments specifying the range of n.");
        return 1;
    }

    int start = boost::lexical_cast<int>(argv[1]);
    int end = boost::lexical_cast<int>(argv[2]);
    for (int i = start; i <= end; ++i) {
        runExperiment(i);
    }
}

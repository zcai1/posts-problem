//
// Created by cai on 2020-09-24.
//

#include "FixedBitSet.h"

#include <stdexcept>
#include <algorithm>
#include <iostream>

#ifdef __AVX2__
#include <immintrin.h>
#endif

#ifdef SAFETY_CHECKS
namespace {
    const std::string OUT_OF_SPACE = "BitSet doesn't have enough capacity left!!";
    const std::string OUT_OF_BOUND = "BitSet operation out of bound!";
    const std::string SIZE_NOT_MATCHED = "FixedBitSets should have the same maxSize.";
}
#endif

typedef boost::dynamic_bitset<unsigned long, std::allocator<unsigned long>> db;

FixedBitSet::FixedBitSet(int maxSize)
    : maxSize(maxSize) {
    // allocate one more block for safety concerns
    auto actualSize = maxSize + db::bits_per_block;
    bs = boost::dynamic_bitset<>(actualSize);
}

void FixedBitSet::removeFirst(int nbits) {
    nbits = std::min(nbits, length);

    start = (maxSize - start > nbits) ? start + nbits : maxSize;
    length = std::max(0, length - nbits);

    if (start == maxSize) {
        trim();
    }
}

void FixedBitSet::append(int nbits, bool value) {
#ifdef SAFETY_CHECKS
    check(maxSize - length >= nbits, OUT_OF_SPACE);
#endif

    if (maxSize - length - start < nbits) {
        trim();
    }

    if (value) {
        bs.set(start + length, nbits, true);
    }
    length += nbits;
}

void FixedBitSet::append1101() {
    if (maxSize - length - start < 4) {
        trim();
    }

    bs.set(start + length);
    bs.set(start + length + 1);
    bs.set(start + length + 3);
    length += 4;
}

void FixedBitSet::append00() {
    if (maxSize - length - start < 2) {
        trim();
    }

    length += 2;
}

void FixedBitSet::clear(int i) {
#ifdef SAFETY_CHECKS
    check(i < length, OUT_OF_BOUND);
#endif

    bs.reset(start + i);
}

void FixedBitSet::set(int i) {
#ifdef SAFETY_CHECKS
    check(i < length, OUT_OF_BOUND);
#endif

    bs.set(start + i);
}

bool FixedBitSet::get(int i) {
#ifdef SAFETY_CHECKS
    check(i < length, OUT_OF_BOUND);
#endif

    return bs[start + i];
}

// this is a very expensive function
void FixedBitSet::trim() {
    if (start == 0) return;

    // removes everything before the actual start
    bs >>= start;
    start = 0;
}

// created by modifying the code of operator >>= in dynamic_bitset
void FixedBitSet::replaceBy(FixedBitSet &other) {
#ifdef SAFETY_CHECKS
    check(maxSize == other.maxSize, SIZE_NOT_MATCHED);
#endif

    auto clearStart = 0UL;
    auto *const b1 = &(bs.m_bits[0]);

    if (other.length > 0) {
        auto const last2 = (other.start + other.length - 1) / db::bits_per_block;
        auto const div2 = other.start / db::bits_per_block;
        auto const r2 = other.start % db::bits_per_block;
        auto *const b2 = &(other.bs.m_bits[0]);
        clearStart = last2 - div2 + 1;

        if (r2 != 0) {
            auto const ls2 = db::bits_per_block - r2;

            // auto vectorization by compiler
            for (auto j = div2; j < last2; ++j) {
                b1[j-div2] = (b2[j] >> r2) | (b2[j+1]  << ls2);
            }
            b1[last2-div2] = b2[last2] >> r2;
        } else {
            memcpy(b1, b2 + div2, sizeof(unsigned long) * clearStart);
        }
    }

    // reset the extra bits
    if (length > 0 || start > 0) {
        auto const last1 = (start + length - 1) / db::bits_per_block;
        if (clearStart <= last1) {
            memset(b1 + clearStart, 0, sizeof(unsigned long) * (last1 - clearStart + 1));
        }
    }

    start = 0;
    length = other.length;
}

// created by modifying the code of operator >>= in dynamic_bitset
bool operator==(const FixedBitSet &lhs, const FixedBitSet &rhs) {
    if (lhs.length != rhs.length) return false;
    if (lhs.length == 0) return true; // both are empty

    auto &bs1 = lhs.bs;
    auto &bs2 = rhs.bs;

    // length > 0 implies start <= start + length - 1
    auto last1 = (lhs.start + lhs.length - 1) / db::bits_per_block;
    auto last2 = (rhs.start + rhs.length - 1) / db::bits_per_block;
    auto const div1 = lhs.start / db::bits_per_block;
    auto const div2 = rhs.start / db::bits_per_block;
    auto const r1 = lhs.start % db::bits_per_block;
    auto const r2 = rhs.start % db::bits_per_block;
    auto *const b1 = &(bs1.m_bits[0]);
    auto *const b2 = &(bs2.m_bits[0]);

    // safe because maxSize cannot exceed INT_MAX
    long rangeDiff = (last1 - div1) - (last2 - div2);
    if (rangeDiff == -1L) {
        ++last1;
    } else if (rangeDiff == 1L) {
        ++last2;
    } else if (rangeDiff != 0L) {
        throw std::runtime_error("FixedBitSet comparison assertion failed.");
    }

    if (r1 != 0 && r2 == 0) {
        auto const ls1 = db::bits_per_block - r1;
        auto i = div1;
        auto j = div2;

#ifdef __AVX2__
        if (last1 >= 4) {
            // avoid unsigned long overflow
            for (; i < last1 - 3; i += 4, j += 4) {
                auto word1a = _mm256_lddqu_si256(reinterpret_cast<const __m256i *>(b1 + i));
                auto word1b = _mm256_lddqu_si256(reinterpret_cast<const __m256i *>(b1 + i + 1));
                auto word1 = _mm256_or_si256(
                        _mm256_srli_epi64(word1a, (int)r1),
                        _mm256_slli_epi64(word1b, (int)ls1)
                );
                auto word2 = _mm256_lddqu_si256(reinterpret_cast<const __m256i *>(b2 + j));
                auto cmpResult = _mm256_cmpeq_epi64(word1, word2);
                if (_mm256_movemask_epi8(cmpResult) != 0xffffffffU) return false;
            }
        }
#endif

        for (; i < last1; ++i, ++j) {
            auto word1 = (b1[i] >> r1) | (b1[i+1]  << ls1);
            if (word1 != b2[j]) return false;
        }

        if ((b1[last1] >> r1) != b2[last2]) return false;
    } else if (r1 != 0) {
        auto const ls1 = db::bits_per_block - r1;
        auto const ls2 = db::bits_per_block - r2;
        auto i = div1;
        auto j = div2;

#ifdef __AVX2__
        if (last1 >= 4) {
            // avoid unsigned long overflow
            for (; i < last1 - 3; i += 4, j += 4) {
                auto word1a = _mm256_lddqu_si256(reinterpret_cast<const __m256i *>(b1 + i));
                auto word1b = _mm256_lddqu_si256(reinterpret_cast<const __m256i *>(b1 + i + 1));
                auto word1 = _mm256_or_si256(
                        _mm256_srli_epi64(word1a, (int)r1),
                        _mm256_slli_epi64(word1b, (int)ls1)
                );

                auto word2a = _mm256_lddqu_si256(reinterpret_cast<const __m256i *>(b2 + j));
                auto word2b = _mm256_lddqu_si256(reinterpret_cast<const __m256i *>(b2 + j + 1));
                auto word2 = _mm256_or_si256(
                        _mm256_srli_epi64(word2a, (int)r2),
                        _mm256_slli_epi64(word2b, (int)ls2)
                );

                auto cmpResult = _mm256_cmpeq_epi64(word1, word2);
                if (_mm256_movemask_epi8(cmpResult) != 0xffffffffU) return false;
            }
        }
#endif

        for (; i < last1; ++i, ++j) {
            auto word1 = (b1[i] >> r1) | (b1[i+1]  << ls1);
            auto word2 = (b2[j] >> r2) | (b2[j+1]  << ls2);
            if (word1 != word2) return false;
        }

        if ((b1[last1] >> r1) != (b2[last2] >> r2)) return false;
    } else if (r2 == 0) {
        int result = memcmp(b1 + div1, b2 + div2, sizeof(unsigned long) * (last1 - div1 + 1));
        if (result != 0) return false;
    } else { // r2 != 0
        auto const ls2 = db::bits_per_block - r2;
        auto i = div1;
        auto j = div2;

#ifdef __AVX2__
        if (last1 >= 4) {
            // avoid unsigned long overflow
            for (; i < last1 - 3; i += 4, j += 4) {
                auto word2a = _mm256_lddqu_si256(reinterpret_cast<const __m256i *>(b2 + j));
                auto word2b = _mm256_lddqu_si256(reinterpret_cast<const __m256i *>(b2 + j + 1));
                auto word2 = _mm256_or_si256(
                        _mm256_srli_epi64(word2a, (int)r2),
                        _mm256_slli_epi64(word2b, (int)ls2)
                );
                auto word1 = _mm256_lddqu_si256(reinterpret_cast<const __m256i *>(b1 + i));
                auto cmpResult = _mm256_cmpeq_epi64(word1, word2);
                if (_mm256_movemask_epi8(cmpResult) != 0xffffffffU) return false;
            }
        }
#endif

        for (; i < last1; ++i, ++j) {
            auto word2 = (b2[j] >> r2) | (b2[j+1]  << ls2);
            if (b1[i] != word2) return false;
        }

        if (b1[last1] != (b2[last2] >> r2)) return false;
    }

    return true;
}

// for debug only
std::ostream &operator<<(std::ostream &out, const FixedBitSet &fbs) {
    std::stringstream ss;

    for (int i = fbs.start; i < fbs.start + fbs.length; ++i) {
        if (fbs.bs[i]) {
            ss << '1';
        } else {
            ss << '0';
        }
    }
    return out << ss.str();
}

#ifdef SAFETY_CHECKS
void FixedBitSet::check(bool value, const std::string &errorMsg) const {

    if (!value) {
        std::stringstream ss;
        ss << errorMsg << "\n Details: "
           << "maxSize=" << maxSize
           << ", start=" << start
           << ", length: " << length;
//           << ", cardinality: " << cardinality;
        throw std::invalid_argument(ss.str());
    }
}
#endif

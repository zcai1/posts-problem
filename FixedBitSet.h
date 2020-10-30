//
// Created by cai on 2020-09-24.
//

#ifndef POSTS_CPP_FIXEDBITSET_H
#define POSTS_CPP_FIXEDBITSET_H


#include <string>
#include <boost/dynamic_bitset.hpp>
#include <boost/archive/text_oarchive.hpp>
#include <boost/archive/text_iarchive.hpp>
#include <memory>

// A BitSet which stores at most "maxSize" bits
class FixedBitSet {
public:
    const int maxSize;

    explicit FixedBitSet(int maxSize);

    FixedBitSet(const FixedBitSet &fbs) = default;
    FixedBitSet(FixedBitSet &&fbs) = default;
    FixedBitSet &operator=(const FixedBitSet &fbs) = delete;
    FixedBitSet &operator=(FixedBitSet &&fbs) = delete;

    void replaceBy(FixedBitSet &other);

    void removeFirst(int nbits);

    void append(int nbits, bool value);

    void append1101();

    void append00();

    void clear(int i);

    void set(int i);

    bool get(int i);

    const int &getLength() const {
        return length;
    }

    static FixedBitSet sigma(int &n) {
        // no safety checks here
        FixedBitSet fbs(INT_MAX);

        for (int i = 0; i < n; ++i) {
            fbs.bs.set(i*3);
        }
        fbs.length = n*3;
        return fbs;
    }

    friend bool operator==(const FixedBitSet &lhs, const FixedBitSet &rhs);

    friend bool operator!=(const FixedBitSet &lhs, const FixedBitSet &rhs) {
        return !(lhs == rhs);
    }

    friend std::ostream &operator<<(std::ostream &out, const FixedBitSet &fbs);

private:
    int start = 0;
    int length = 0;
    boost::dynamic_bitset<> bs;

    void trim();

#ifdef SAFETY_CHECKS
    void check(bool value, const std::string &errorMsg = "") const;
#endif

    friend class boost::serialization::access;
    // When the class Archive corresponds to an output archive, the
    // & operator is defined similar to <<.  Likewise, when the class Archive
    // is a type of input archive the & operator is defined similar to >>.
    template<class Archive>
    void serialize(Archive & ar, const unsigned int version)
    {
        ar & start;
        ar & length;
        ar & bs;
    }
};

#endif //POSTS_CPP_FIXEDBITSET_H

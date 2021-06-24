#ifndef BITSET_H
#define BITSET_H

#include "Arduino.h"

template<int N>
class bitset {
public:
    bitset() {
        fill(0);
    }

    bitset(const bitset &a) {
        for (int i = 0; i < N; i++)
            _data[i] = a._data[i];
    }

    void set_bit(int index, bool val) {
        if (get_bit(index) != val)
            _data[index / 8] ^= (1 << (index % 8));
    }

    bool get_bit(int index) const {
        return (bool)(_data[index / 8] & (1 << (index % 8)));
    }

    bool empty() const {
        return !N;
    }

    int size() const {
        return N;
    }

    void fill(bool val) {
        uint8_t mask = ((val) ? 255 : 0);
        for (int i = 0; i < _cp; i++)
            _data[i] = mask;
    }

private:
    static const int _cp = (N + 7) / 8;
    uint8_t _data[_cp];
};
#endif
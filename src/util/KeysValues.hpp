#ifndef SPARSETENSOR_UTIL_KEYSVALUE
#define SPARSETENSOR_UTIL_KEYSVALUE

#include <iterator>

namespace sparsetensor::util::keys {
    template<class Associative>
    class Keys {
        Associative &associative;
    public:
        Keys(Associative &associative) : associative(associative) {}

        decltype(auto) begin() const { return iterator(associative.begin()); }

        decltype(auto) end() const { return iterator(associative.end()); }

        template<class Iterator>
        class iterator {
            using iter_key_t = std::remove_const_t<typename std::iterator_traits<Iterator>::value_type::first_type>;
            Iterator iter;
        public:
            iterator(Iterator iter) : iter(iter) {}

            iterator &operator++() {
                ++iter;
                return *this;
            }

            bool operator!=(const iterator &other) const {
                return iter != other.iter;
            }

            const iter_key_t &operator*() {
                return iter->first;
            }
        };
    };
};

namespace sparsetensor::util::values {
    template<class Associative>
    class Values {
        Associative &associative;
    public:
        Values(Associative &associative) : associative(associative) {}

        decltype(auto) begin() const { return iterator(associative.begin()); }

        decltype(auto) end() const { return iterator(associative.end()); }

        template<class Iterator>
        class iterator {
            using iter_val_t = typename std::iterator_traits<Iterator>::value_type::second_type;
            Iterator iter;
        public:
            iterator(Iterator iter) : iter(iter) {}

            iterator &operator++() {
                ++iter;
                return *this;
            }

            bool operator!=(const iterator &other) const {
                return iter != other.iter;
            }

            const iter_val_t &operator*() {
                return iter->second;
            }
        };
    };
};

template<class Associative>
decltype(auto) keys(Associative &associative) {
    return sparsetensor::util::keys::Keys(associative);
}

template<class Associative>
decltype(auto) values(Associative &associative) {
    return sparsetensor::util::values::Values(associative);
}


#endif //SPARSETENSOR_UTIL_KEYSVALUE

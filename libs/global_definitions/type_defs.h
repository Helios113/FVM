#ifndef TYPE_DEFS_H
#define TYPE_DEFS_H

#include <boost/multi_array.hpp>
#include <iostream>
#include <unordered_map>

#include "conduit.hpp"

typedef boost::multi_array<double, 3> scalar_field;
typedef boost::multi_array<double, 4> vector_field;
typedef boost::multi_array<double, 4> tensor_field;

class dynamic_double {
   private:
    std::vector<double> val = {};

   public:
    dynamic_double() {}
    dynamic_double(const double &b) { val.assign(&b, &b + 1); }
    dynamic_double(const int &b) { val.assign(&b, &b + 1); }

    dynamic_double(double *other, int size) { val.assign(other, other + size); }
    dynamic_double(std::vector<double> &b) { val = b; }

    operator double(void) { return val[0]; }
    operator std::vector<double>(void) { return val; }

    double operator[](int i) const { return val[i]; }
    double &operator[](int i) { return val[i]; }

    void operator=(const double &other) { val.assign(&other, &other + 1); }
    void operator=(std::vector<double> &other) { val = other; }
    void set_array(double *other, int size) { val.assign(other, other + size); }
    std::string to_string() {
        std::string res = "";
        for (int i = 0; i < val.size(); i++) {
            res += std::to_string(val[i]);
        }
        return res;
    }
};

typedef boost::multi_array<std::unordered_map<std::string, dynamic_double>, 1>
    complex_field;
#endif
#ifndef HARMONY_ARB_INT_H
#define HARMONY_ARB_INT_H

#include <vector>
#include <string>
#include <ostream>

#include <cstdint> // uint8_t
#include <cstddef> // std::size_t
using std::size_t;

// TO DO : Division operations (/ & /=)
// TO DO : modulus operations (% & %=)
// TO DO : Coversion operations ( addding integrals & strings )
// TO DO : different bases?
// TO DO : allow scientific notation?

// TO DO : Move length calls out of loops
// TO DO : Think about number_ container
// TO DO : Think about using std::div instead of % & /
// TO DO : Constexpr
// TO DO : Remove copy on (op)= & define (op) by (op)=
// TO DO : Think about using static reference technique for ONE & NEG_ONE


// Class for creating 'Arbitrarily large Integers' ( up to length of max size_t value )
// Each digit takes up 1 byte ( so to reach max value one would need several quintilion bytes )
class Integer {

  bool negative_;
  // Vector storing number ( in a backwards order, ie number_[0] is the least significant digit )
  std::vector<uint8_t> number_;

public:

  // Default value is 0
  Integer(): negative_(false) { number_.push_back(0); }
  explicit Integer(int);
  explicit Integer(const std::string&);

  size_t length() const { return number_.size(); }

  // Incrementing
  Integer& operator++();
  Integer operator++(int);
  Integer& operator--();
  Integer operator--(int);
  Integer& operator+=(const Integer&);
  Integer& operator-=(const Integer&);
  Integer& operator*=(const Integer&);

  // Operations
  friend const Integer operator+(const Integer&, const Integer&);
  friend const Integer operator-(const Integer&, const Integer&);
  friend const Integer operator*(const Integer&, const Integer&);
  
  // Comparison
  friend bool operator==(const Integer&, const Integer&);
  friend bool operator!=(const Integer&, const Integer&);
  friend bool operator<(const Integer&, const Integer&);
  friend bool operator>(const Integer&, const Integer&);
  friend bool operator<=(const Integer&, const Integer&);
  friend bool operator>=(const Integer&, const Integer&);

  // Stream
  friend std::ostream& operator<<(std::ostream&, const Integer&);

  // Helper functions
  friend bool unsigned_greater(const Integer&, const Integer&);
  friend Integer add_same(const Integer&, const Integer&);
  friend Integer add_different(const Integer&, const Integer&);
};

#endif

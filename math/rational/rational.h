#ifndef HARMONY_RATIONAL_H
#define HARMONY_RATIONAL_H

#include <ostream>

// TO DO : Reciprical function (with 0 checking) to replace divide
// TO DO : Implicit conversion Operators
// TO DO : Overflow?
// TO DO : Constexpr & noexcept

// Rational Number ( Represents space QQ ) that is always reduced in form num_ / denom_
class Rational {

  bool negative_;
  // numerator & denominator
  int num_, denom_;

  // Reduce rational number
  void reduce();
  // Helper functions for adding independant of rhs side sign
  void unsigned_add(const Rational&);
  void unsigned_subtract(const Rational&);

public:

  // Default value is 0
  Rational():  negative_(false), num_(0), denom_(1) { }
  explicit Rational(int num, int denom = 1);

  // Incrementing
  Rational& operator++();
  Rational operator++(int);
  Rational& operator--();
  Rational operator--(int);
  Rational& operator+=(const Rational&);
  Rational& operator-=(const Rational&);
  Rational& operator*=(const Rational&);
  Rational& operator/=(const Rational&);

  // Operations
  friend const Rational operator+(const Rational&, const Rational&);
  friend const Rational operator-(const Rational&, const Rational&);
  friend const Rational operator*(const Rational&, const Rational&);
  friend const Rational operator/(const Rational&, const Rational&);

  // Comparison
  friend bool operator==(const Rational&, const Rational&);
  friend bool operator!=(const Rational&, const Rational&);
  friend bool operator<(const Rational&, const Rational&);
  friend bool operator>(const Rational&, const Rational&);
  friend bool operator<=(const Rational&, const Rational&);
  friend bool operator>=(const Rational&, const Rational&);

  // Stream
  friend std::ostream& operator<<(std::ostream&, const Rational&);
};

#endif

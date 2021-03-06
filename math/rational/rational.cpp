#include "rational.h"

#include <stdexcept> // runtime_error

#include <numeric> // gcd

Rational::Rational(int num, int denom):
  negative_(false),
  num_(num),
  denom_(denom) {
  if(denom == 0) throw std::runtime_error("Denominator == 0 Error!");
  reduce();
}

void Rational::reduce() {
  if(num_ < 0) {
    num_ *= -1;
    negative_ = !negative_;
  }
  if(denom_ < 0) {
    denom_ *= -1;
    negative_ = !negative_;
  }

  int divisor = std::gcd(num_, denom_);
  num_ /= divisor;
  denom_ /= divisor;
}

// TO DO : Do like helper in arb_int?
void Rational::unsigned_add(const Rational& rhs) {
  if(denom_ == rhs.denom_) {
    negative_ ? num_ -= rhs.num_ : num_ += rhs.num_;
  } else {
    int divisor = std::gcd(denom_, rhs.denom_);
    num_ *= (rhs.denom_ / divisor);
    negative_ ? num_ -= rhs.num_ * (denom_ / divisor) : num_ += rhs.num_ * (denom_ / divisor);
    denom_ *= (rhs.denom_ / divisor); // lcm
  }
  reduce();
  // TO DO : Think of constraint ( overflow ) vs complexity
}

void Rational::unsigned_subtract(const Rational& rhs) {
  if(denom_ == rhs.denom_) {
    negative_ ? num_ += rhs.num_ : num_ -= rhs.num_;
  } else {
    int divisor = std::gcd(denom_, rhs.denom_);
    num_ *= (rhs.denom_ / divisor);
    negative_ ? num_ += rhs.num_ * (denom_ / divisor) : num_ -= rhs.num_ * (denom_ / divisor);
    denom_ *= (rhs.denom_ / divisor); // lcm
  }
  reduce();
  // TO DO : Think of constraint ( overflow ) vs complexity
}

Rational& Rational::operator+=(const Rational& rhs) {
  rhs.negative_ ? unsigned_subtract(rhs) : unsigned_add(rhs);
  return *this;
}

Rational& Rational::operator-=(const Rational& rhs) {
  rhs.negative_ ? unsigned_add(rhs) : unsigned_subtract(rhs);
  return *this;
}

Rational& Rational::operator*=(const Rational& rhs) {
  negative_ ^= rhs.negative_;
  num_ *= rhs.num_;
  denom_ *= rhs.denom_;
  reduce();
  return *this;
}

Rational& Rational::operator/=(const Rational& rhs) {
  negative_ ^= rhs.negative_;
  num_ *= rhs.denom_;
  denom_ *= rhs.num_;
  if(denom_ == 0) throw std::runtime_error("Dividing Rational by 0. ERROR!");
  reduce();
  return *this;
}

// Increments by 1
// Note : Does not call reduce because if already reduced then adding 1 will not make it reducable
Rational& Rational::operator++() {
  num_ += denom_;
  return *this;
}

Rational Rational::operator++(int) {
  Rational temp = *this;
  num_ += denom_;
  return temp;
}

Rational& Rational::operator--() {
  num_ -= denom_;
  return *this;
}

Rational Rational::operator--(int) {
  Rational temp = *this;
  num_ -= denom_;
  return temp;
}

const Rational operator+(const Rational& lhs, const Rational& rhs) {
  Rational ret(lhs);
  ret += rhs;
  return ret;
}

const Rational operator-(const Rational& lhs, const Rational& rhs) {
  Rational ret(lhs);
  ret -= rhs;
  return ret;
}

const Rational operator*(const Rational& lhs, const Rational& rhs) {
  Rational ret(lhs);
  ret *= rhs;
  return ret;
}

const Rational operator/(const Rational& lhs, const Rational& rhs) {
  Rational ret(lhs);
  ret /= rhs;
  return ret;
}

bool operator==(const Rational& lhs, const Rational& rhs) {
  return lhs.negative_ == rhs.negative_ && lhs.num_ == rhs.num_ && lhs.denom_ == rhs.denom_;
}

bool operator!=(const Rational& lhs, const Rational& rhs) {
  return !(lhs == rhs);
}

bool operator<(const Rational& lhs, const Rational& rhs) {
  return rhs > lhs;
}

bool operator>(const Rational& lhs, const Rational& rhs) {
  if(lhs.negative_ xor rhs.negative_) return !lhs.negative_;
  if(lhs.denom_ == rhs.denom_) {
    return (lhs.negative_ ? lhs.num_ < rhs.num_ : lhs.num_ > rhs.num_);
  } else {
    int divisor = std::gcd(lhs.denom_, rhs.denom_);
    return (lhs.negative_ ? lhs.num_ * (rhs.denom_ / divisor) < rhs.num_ * (lhs.denom_ / divisor) :
                            lhs.num_ * (rhs.denom_ / divisor) > rhs.num_ * (lhs.denom_ / divisor));
    // TO DO : Think complexity vs restraint ( overflow )
  }
}

bool operator<=(const Rational& lhs, const Rational& rhs) {
  return rhs > lhs || rhs == lhs;
}

bool operator>=(const Rational& lhs, const Rational& rhs) {
  return lhs > rhs || lhs == rhs;
}

std::ostream& operator<<(std::ostream& os, const Rational& r) {
  os << (r.negative_ ? "-" : "") << r.num_;
  if(r.denom_ != 1) os << " / " << r.denom_;
  return os;
}

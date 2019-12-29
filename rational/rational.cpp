#include <iostream>

// TO DO : Major sizing issue comes into play when finding denominators for adding 
//           and comparing greater because we are multiplying 2 numbers together so this 
//           can get large pretty quickly..
//           think of a way to avoid this?

class Rational {

  bool negative_;
  unsigned int num_, denom_; // TO DO : Think about type

  Rational& operator++();
  Rational operator++(int);
  Rational& operator--();
  Rational operator--(int);
  Rational& operator+=(const Rational&);
  Rational& operator-=(const Rational&);
  Rational& operator*=(const Rational&);
  Rational& operator/=(const Rational&);

public:

  // TO DO : Restruct 0 in denom

  // TO DO : Negative assignments and allow negative numbers on construction
  Rational(): num_(0), denom_(1), negative_(false) { }

  Rational(unsigned int num, unsigned int denom, bool negative = false):
    negative_(negative),
    num_(num),
    denom_(denom) {
    reduce();
  }

  Rational(int num, int denom): // TO DO : Test when each used
    negative_(num < 0 xor (denom < 0 && num != 0)),
    num_((num < 0 ? -1 : 1) * num),
    denom_((denom < 0 ? -1 : 1) * denom) {
    reduce();
  }

  // Default copy constructor and copy assignment operator

  void reduce() { // TO DO : Do i want this to be private? 
    for(unsigned i = 2; i <= num_ && i <= denom_; ++i) { // TO DO : Think about best max value here
      while(num_ % i == 0 && denom_ % i == 0) {
        num_ /= i;
        denom_ /= i;
      }
    }
  }

  void unsigned_add(const Rational& rhs) { 
    if(denom_ == rhs.denom_) {
      negative_ ? num_ -= rhs.num_ : num_ += rhs.num_; // TO DO : Issue when crossing 0
    } else { // TO DO : Think about lcd
      num_ *= rhs.denom_;
      negative_ ? num_ -= rhs.num_ * denom_ : num_ += rhs.num_ * denom_;
      denom_ *= rhs.denom_;
    }
    reduce();
  }

  void unsigned_subtract(const Rational& rhs) {
    if(denom_ == rhs.denom_) {
      negative_ ? num_ += rhs.num_ : num_ -= rhs.num_;
    } else {
      num_ *= rhs.denom_;
      negative_ ? num_ += rhs.num_ * denom_ : num -= rhs.num_ * denom_;
      denom_ *= rhs.denom_;
    }
    reduce();
  }

  void multiply(const Rational& rhs) { // TO DO : negatives
    negative_ ^= rhs.negative_;
    num_ *= rhs.num_;
    denom_ *= rhs.denom_;
    reduce();
  }

  void divide(const Rational& rhs) { // TO DO : negatives
    negative_ ^= rhs.negative_;
    num_ *= rhs.denom_;
    denom_ *= rhs.num_;
    reduce();
  }

  bool equal(const Rational& rhs) const { // TO DO : Think about if not reduced
    return num_ == rhs.num_ && denom_ == rhs.num_ && negative_ == rhs.negative_;
  }

  bool greater(const Rational& rhs) const { // TO DO : negative values
    if(negative_ xor rhs.negative_) return !negative_;
    else if(negative_) {
      if(denom_ == rhs.denom_) return num_ < rhs.num_;
      else {
        return num_ * rhs.denom_ < rhs.num_ * denom_;
      }
    } else {
      if(denom_ == rhs.denom_) return num_ > rhs.num_;
      else {
        return num_ * rhs.denom_ > rhs.num_ * denom_;
      }
    }
  }

  friend const Rational operator+(const Rational&, const Rational&);
  friend const Rational operator-(const Rational&, const Rational&);
  friend const Rational operator*(const Rational&, const Rational&);
  friend const Rational operator/(const Rational&, const Rational&);

  friend bool operator==(const Rational&, const Rational&);
  friend bool operator!=(const Rational&, const Rational&);
  friend bool operator<(const Rational&, const Rational&);
  friend bool operator>(const Rational&, const Rational&);
  friend bool operator<=(const Rational&, const Rational&);
  friend bool operator>=(const Rational&, const Rational&);

  friend std::ostream& operator<<(std::ostream&, const Rational&);

};

Rational& Rational::operator+=(const Rational& rhs) { // TO DO : Check this
  rhs.negative_ ? unsigned_subtract(rhs) : unsigned_add(rhs);
  return *this;
}

Rational& Rational::operator-=(const Rational& rhs) { // TO DO : Check this
  rhs.negative_ ? unsigned_add(rhs) : unsigned_subtract(rhs);
  return *this;
}

Rational& Rational::operator*=(const Rational& rhs) { // TO DO : Check this
  multiply(rhs);
  return *this;
}

Rational& Rational::operator/=(const Rational& rhs) { // TO DO : Check this
  divide(rhs);
  return *this;
}

// Increments by 1 / denom_
// TO DO : Think about this, notice that if the number gets reduced then the next call to 
//         ++ with increment by a different amount
Rational& Rational::operator++() { 
  ++num_;
  return *this;
}

Rational Rational::operator++(int) { 
  Rational temp = *this;
  ++num_;
  return temp;
}
Rational& Rational::operator--() { 
  --num_;
  return *this;
}

Rational Rational::operator--(int) { 
  Rational temp = *this;
  --num_;
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

// TO DO : *=...
const Rational operator*(const Rational& lhs, const Rational& rhs) {
  Rational ret(lhs);
  ret.multiply(rhs);
  return ret;
}

const Rational operator/(const Rational& lhs, const Rational& rhs) {
  Rational ret(lhs);
  ret.divide(rhs); // TO DO : Think about making reciprical function and deleting divide function
  return ret;
}

// TO DO : Think about making static and using same format as Integer for the comparison ops
bool operator==(const Rational& lhs, const Rational& rhs) {
  return lhs.equal(rhs);
}

bool operator!=(const Rational& lhs, const Rational& rhs) {
  return !lhs.equal(rhs);
}

bool operator<(const Rational& lhs, const Rational& rhs) {
  return rhs.greater(lhs);
}

bool operator>(const Rational& lhs, const Rational& rhs) {
  return lhs.greater(rhs);
}

bool operator<=(const Rational& lhs, const Rational& rhs) {
  return rhs.greater(lhs) || rhs.equal(lhs);
}

bool operator>=(const Rational& lhs, const Rational& rhs) {
  return lhs.greater(rhs) || lhs.equal(rhs);
}

std::ostream& operator<<(std::ostream& os, const Rational& r) {
  if(r.denom_ == 1) {
    os << (r.negative_ ? "-" : "") << r.num_;
  } else {
    os << (r.negative_ ? "-" : "") << r.num_ << " / " << r.denom_;
  }
  return os;
}

int main() {

  Rational r1(10 , 100);
  Rational r2;
//  Rational r3(2 , 5);
  Rational r3(3 , 7);

  r1.print();
  r2.print();
  r1.add(r3);
  r1.print();

  return 0;

}

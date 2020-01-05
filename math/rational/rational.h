#include <stdexcept>

#include <ostream>

// TO DO : Major sizing issue comes into play when finding denominators for adding 
//           and comparing greater because we are multiplying 2 numbers together so this 
//           can get large pretty quickly..
//           think of a way to avoid this?
// TO DO : Improve reduce function
// TO DO : Improve addition to fix sizes
// TO DO : Improve greater check to fix sizes
// TO DO : Reciprical function (with 0 checking) to replace divide
// TO DO : Think about implicit conversion

class Rational {

  bool negative_;
  int num_, denom_;

  void reduce() { 
    if(num_ < 0) {
      num_ *= -1;
      negative_ = !negative_;
    }
    if(denom_ < 0) {
      denom_ *= -1;
      negative_ = !negative_;
    }
    for(unsigned i = 2; i <= num_ && i <= denom_; ++i) { // TO DO : Think about best max value here
      while(num_ % i == 0 && denom_ % i == 0) {
        num_ /= i;
        denom_ /= i;
      }
    }
  }

  void unsigned_add(const Rational& rhs) { 
    if(denom_ == rhs.denom_) {
      negative_ ? num_ -= rhs.num_ : num_ += rhs.num_;
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
      negative_ ? num_ += rhs.num_ * denom_ : num_ -= rhs.num_ * denom_;
      denom_ *= rhs.denom_;
    }
    reduce();
  }

  void multiply(const Rational& rhs) {
    negative_ ^= rhs.negative_;
    num_ *= rhs.num_;
    denom_ *= rhs.denom_;
    reduce();
  }

  void divide(const Rational& rhs) {
    negative_ ^= rhs.negative_;
    num_ *= rhs.denom_;
    denom_ *= rhs.num_;
    reduce();
  }

  bool equal(const Rational& rhs) const {
    return num_ == rhs.num_ && denom_ == rhs.num_ && negative_ == rhs.negative_;
  }

  bool greater(const Rational& rhs) const {
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

public:

  Rational& operator++();
  Rational operator++(int);
  Rational& operator--();
  Rational operator--(int);
  Rational& operator+=(const Rational&);
  Rational& operator-=(const Rational&);
  Rational& operator*=(const Rational&);
  Rational& operator/=(const Rational&);


  Rational(): num_(0), denom_(1), negative_(false) { }

  explicit Rational(int num, int denom = 1):
    negative_(false),
    num_(num),
    denom_(denom) {
    if(denom == 0) throw std::runtime_error("Denominator == 0 Error!");
    reduce();
  }

  // Default copy constructor and copy assignment operator

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

Rational& Rational::operator+=(const Rational& rhs) { 
  rhs.negative_ ? unsigned_subtract(rhs) : unsigned_add(rhs);
  return *this;
}

Rational& Rational::operator-=(const Rational& rhs) {
  rhs.negative_ ? unsigned_add(rhs) : unsigned_subtract(rhs);
  return *this;
}

Rational& Rational::operator*=(const Rational& rhs) {
  multiply(rhs);
  return *this;
}

Rational& Rational::operator/=(const Rational& rhs) {
  divide(rhs);
  return *this;
}

// Increments by 1
// Note : This does not need to call reduce because if already reduced then adding 1 will not make
//        reducable
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

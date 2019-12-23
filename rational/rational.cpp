#include <iostream>

// TO DO : Major sizing issue comes into play when finding denominators for adding 
//           and comparing greater because we are multiplying 2 numbers together so this 
//           can get large pretty quickly..
//           think of a way to avoid this?

class Rational {

  unsigned int num_, denom_; // TO DO : Think about type
  bool negative_;

public:

  // TO DO : Negative assignments and allow negative numbers on construction
  Rational(): num_(0), denom_(1), negative_(false) { }

  Rational(unsigned int num, unsigned int denom):
    num_(num),
    denom_(denom),
    negative_(false) {
    reduce();
  }

  // Default copy constructor and copy assignment operator

  void reduce() { // TO DO : Do i want this to be private? 
    for(unsigned i = 2; i <= num_; ++i) { // TO DO : Think about best max value here
      while(num_ % i == 0 && denom_ % i == 0) {
        num_ /= i;
        denom_ /= i;
      }
    }
  }

  void print() { // TO DO : Temp
    if(denom_ == 1) {
      std::cout << (negative_ ? "-" : "") << num_ << std::endl;
    } else {
      std::cout << (negative_ ? "-" : "") << num_ << " / " << denom_ << std::endl;
    }
  }

  void add(const Rational& rhs) { // TO DO : Account for negatives 
                                  //         ( incorporate subtract into this ? )
    if(denom_ == rhs.denom_) {
      num_ += rhs.num_;
    } else { // TO DO : Think about lcd
      num_ *= rhs.denom_;
      num_ += rhs.num_ * denom_;
      denom_ *= rhs.denom_;
    }
    reduce();
  }

  void multiply(const Rational& rhs) { // TO DO : negatives
    num_ *= rhs.num_;
    denom_ *= rhs.denom_;
    reduce();
  }

  void divide(const Rational& rhs) { // TO DO : negatives
    num_ *= rhs.denom_;
    denom_ *= rhs.num_;
    reduce();
  }

  bool equal(const Rational& rhs) { // TO DO : Think about if not reduced
    return num_ == rhs.num_ && denom_ == rhs.num_ && negative_ == rhs.negative_;
  }

  bool greater(const Rational& rhs) { // TO DO : negative values
    if(denom_ == rhs.denom_) return num_ < rhs.num_;
    else {
      return num_ * rhs.denom_ < rhs.num_ * denom_;
    }
  }

};

// TO DO : Operators : + , - , * , / , << , < , == , <= , > , >= , != , 

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

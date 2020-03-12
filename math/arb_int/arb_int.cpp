#include "arb_int.h"

#include <map> // for new_top in add_different(Integer, Integer)

#include <algorithm> // transform, for_each, copy
#include <iterator> // back_inserter, ostream_iterator

#include <stdexcept> // runtime_error

Integer::Integer(int num):
  negative_(num < 0) {
  if(negative_) num *= -1;
  do {
    number_.push_back(num % 10);
    num /= 10;
  } while(num != 0);
}

Integer::Integer(const std::string& s) {
  if(s.length() > 0) {
    negative_ = s[0] == '-';
    if(negative_ && s.length() == 1) throw std::runtime_error("\"-\" is not a valid Integer");

    // Transform passed string into number_ by iterating backwards & removing negative sign
    std::transform(s.rbegin(), s.rend() - negative_, std::back_inserter(number_), [](auto c) {
      if(c > 47 && c < 58) return c - '0'; /* Character to digit */
      else throw std::runtime_error("Invalid digit character given to Integer constructor");
    });
  } else { // "" Becomes 0
    negative_ = false;
    number_.push_back(0);
  }
  // Stop -0
  if(length() == 1 && number_[0] == 0) negative_ = false;
}

// Return if lhs is larger than rhs (ignoring sign)
bool unsigned_greater(const Integer& lhs, const Integer& rhs) {
  if(lhs.length() == rhs.length()) { // if same length
    for(size_t i = lhs.length() - 1; i > 0; --i) {
      if(lhs.number_[i] == rhs.number_[i]) continue;
      else return lhs.number_[i] > rhs.number_[i];
    }
    return lhs.number_[0] > rhs.number_[0]; // Last digit
  } else { // compare by length
    return lhs.length() > rhs.length();
  }
}

// Returns sum of 2 Integers of the same sign ( ignores actual sign of values, takes sign of lhs )
Integer add_same(const Integer& lhs, const Integer& rhs) {
  // Follows normal addition tech learned in school
  Integer ret;
  ret.number_.clear();
  ret.negative_ = lhs.negative_; // Adding 2 negatives

  bool lhs_longer = lhs.length() >= rhs.length();
  const Integer& longer = lhs_longer ? lhs : rhs;

  size_t pos = 0;
  size_t shared_length = lhs_longer ? rhs.length() : lhs.length();
  uint8_t carry = 0;
  while(pos < shared_length) {
    uint8_t val = lhs.number_[pos] + rhs.number_[pos] + carry;
    ret.number_.push_back(val % 10);
    carry = val / 10;
    ++pos;
  }
  while(pos < longer.length()) {
    uint8_t val = longer.number_[pos] + carry;
    ret.number_.push_back(val % 10);
    carry = val / 10;
    ++pos;
  }
  if(carry != 0) ret.number_.push_back(carry);
  return ret;
}

// Return sum of 2 numbers with different sign (lhs positive & rhs negative) ( ignores actual signs )
Integer add_different(const Integer& lhs, const Integer& rhs) {
  // Follows normal subtraction tech learned in school
  Integer ret;
  ret.number_.clear();
  std::map<size_t, uint8_t> new_top;

  bool lhs_ugreater = unsigned_greater(lhs, rhs);
  const Integer& top = lhs_ugreater ? lhs : rhs;
  const Integer& bot = lhs_ugreater ? rhs : lhs;
  ret.negative_ = !lhs_ugreater;

  size_t pos = 0;
  // Lambda for borrowing when subtracting a larger from smaller single digit
  auto borrow = [&](size_t p){
    size_t inc = 1;
    while(top.number_[p + inc] == 0) new_top[p + inc++] = 9;
    new_top[p + inc] = top.number_[p + inc] - 1;
  };
  for(; pos < bot.length(); ++pos) {
    uint8_t top_num = new_top.count(pos) ? new_top[pos] : top.number_[pos];
    if(top_num >= bot.number_[pos])
     ret.number_.push_back(top_num - bot.number_[pos]);
    else {
      ret.number_.push_back(10 + top_num - bot.number_[pos]);
      borrow(pos);
    }
  }
  for(; pos < top.length(); ++pos) {
    ret.number_.push_back(new_top.count(pos) ? new_top[pos] : top.number_[pos]);
  }

  // remove leading 0s
  while(ret.length() > 1 && ret.number_[ret.length() - 1] == 0) ret.number_.pop_back();
  // TO DO : Think on this loop
  return ret;
}

// MEMBER OPERATORS

Integer& Integer::operator+=(const Integer& rhs) {
  auto value = *this + rhs;
  *this = value;
  return *this;
}

Integer& Integer::operator-=(const Integer& rhs) {
  auto value = *this - rhs;
  *this = value;
  return *this;
}

Integer& Integer::operator*=(const Integer& rhs) {
  auto value = *this * rhs;
  *this = value;
  return *this;
}

Integer& Integer::operator++() {
  static const Integer ONE(1);
  *this += ONE;
  return *this;
}

Integer Integer::operator++(int) {
  Integer temp = *this;
  ++*this;
  return temp;
}

Integer& Integer::operator--() {
  // TO DO : -= ONE
  static const Integer NEG_ONE(-1);
  *this += NEG_ONE;
  return *this;
}

Integer Integer::operator--(int) {
  Integer temp = *this;
  --*this;
  return temp;
}

// FRIEND OPERATORS

const Integer operator+(const Integer& lhs, const Integer& rhs) {
  if(lhs.negative_ == rhs.negative_) return add_same(lhs, rhs);
  else return lhs.negative_ ? add_different(rhs, lhs) : add_different(lhs, rhs);
}

const Integer operator-(const Integer& lhs, const Integer& rhs) {
  if(lhs.negative_ == !rhs.negative_) return add_same(lhs, rhs);
  else return lhs.negative_ ? add_different(rhs, lhs) : add_different(lhs, rhs);
}

const Integer operator*(const Integer& lhs, const Integer& rhs) {
  // Follows normal multiplication tech learned in school
  Integer ret;
  ret.number_.clear();
  if(lhs.negative_ xor rhs.negative_) ret.negative_ = true;

  size_t offset = 0;
  for(auto top_digit : lhs.number_) {
    uint8_t carry = 0;
    size_t pos = 0;
    for(; pos < rhs.length(); ++pos) { // move rhs length out
      uint8_t val = top_digit * rhs.number_[pos] + carry;
      if(pos + offset >= ret.length()) {
        ret.number_.push_back(val % 10);
        carry = val / 10;
      } else {
        val += ret.number_[pos + offset];
        ret.number_[pos + offset] = val % 10;
        carry = val / 10;
      }
    }
    size_t carry_pos = 0;
    while(carry != 0) { // TO DO : Think if this will always be an number_.push_back
      if(pos + offset + carry_pos >= ret.length()) {
        ret.number_.push_back(carry % 10);
        carry /= 10;
      } else {
        carry += ret.number_[pos + offset + carry_pos];
        ret.number_[pos + offset + carry_pos] = carry % 10;
        carry /= 10;
      }
      ++carry_pos;
    }
    ++offset;
  }

  // remove leading 0s
  while(ret.length() > 1 && ret.number_[ret.length() - 1] == 0) ret.number_.pop_back();
  // remove negative on 0 multiplication
  if(ret.length() == 1 && ret.number_[0] == 0) ret.negative_ = false;
  return ret;
}

bool operator==(const Integer& lhs, const Integer& rhs) {
  if(lhs.negative_ xor rhs.negative_) return false;
  if(lhs.length() != rhs.length()) return false;
  for(size_t i = lhs.length() - 1; i > 0; --i) {
    if(lhs.number_[i] != rhs.number_[i]) return false;
  }
  return lhs.number_[0] == rhs.number_[0]; // compare last digit
}

bool operator!=(const Integer& lhs, const Integer& rhs) {
  return !(lhs == rhs);
}

bool operator<(const Integer& lhs, const Integer& rhs) {
  return rhs > lhs;
}

bool operator>(const Integer& lhs, const Integer& rhs) {
  if(lhs.negative_ xor rhs.negative_) return rhs.negative_;
  return lhs.negative_ ? !unsigned_greater(lhs, rhs) : unsigned_greater(lhs, rhs);
}

bool operator<=(const Integer& lhs, const Integer& rhs) {
  return rhs > lhs || lhs == rhs;
}

bool operator>=(const Integer& lhs, const Integer& rhs) {
  return lhs > rhs || lhs == rhs;
}

std::ostream& operator<< (std::ostream& os, const Integer& s) {
  if(s.negative_) os << "-";
  std::copy(s.number_.rbegin(), s.number_.rend(), std::ostream_iterator<unsigned>(os, ""));
  return os;
}

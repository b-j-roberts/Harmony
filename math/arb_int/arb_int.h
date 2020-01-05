#include <vector>
#include <map> // for carry bytes
#include <string>

#include <cstdint> // TO DO : Is this needed?
#include <stdexcept>

#include <ostream>

// TO DO : think about division?
// TO DO : modulus
// TO DO : different bases?
// TO DO : allow scientific notation?
// TO DO : Think about implicit construction
// TO DO : Move length calls out of loops

// TO DO : Bug will occur when length of vector is max int size

// TO DO : More below
// TO DO : Move static functions out?

class Integer {

  std::vector<uint8_t> number_;
  bool negative_;

public:

  Integer(): negative_(false) { number_.push_back(0); }

  Integer(int num): negative_(num < 0) {
    if(num == 0) number_.push_back(0);
    if(negative_) num *= -1;
    while(num != 0) {
      number_.push_back(num % 10);
      num /= 10;
    }
  }

  Integer(std::string s) {
    if(s.length() > 0) {
      negative_ = s[0] == '-';
      for(int i = s.length() - 1; i >= negative_ ? 1 : 0; --i) {
        if(s[i] > 47 && s[i] < 58) {
          number_.push_back(s[i] - '0');
        } else {
          throw std::runtime_error("Invalid character : " + s.substr(i, 1) + 
                                   " given to Integer constructor");
        }
      }
      if(number_.size() == 0)
        throw std::runtime_error(s + " is not a valid string for Integer construction");

    } else {
      negative_ = false;
      number_.push_back(0);
    }
  }

  size_t length() const { return number_.size(); }

  Integer& operator++();
  Integer operator++(int);
  Integer& operator--();
  Integer operator--(int);
  Integer& operator+=(const Integer&);
  Integer& operator-=(const Integer&);
  Integer& operator*=(const Integer&);

private:

  void append(uint8_t i) { number_.push_back(i); }

  // tell if lhs is larger than rhs (ignoring sign)
  static bool unsigned_greater(const Integer& lhs, const Integer& rhs) {
    if(lhs.length() == rhs.length()) {// if same length
      for(int i = lhs.length() - 1; i >= 0; --i) {
        if(lhs.number_[i] == rhs.number_[i]) continue;
        else return lhs.number_[i] > rhs.number_[i]; 
      }
    } else { // compare by length
      return lhs.length() > rhs.length();
    }
    return true; // They are equal
  }

  // tell if lhs greater than rhs
  static bool greater(const Integer& lhs, const Integer& rhs) {
    if(lhs.negative_ xor rhs.negative_) return rhs.negative_; 
    bool flip = lhs.negative_; // if comparing neg or pos, flip bc larger number_ flips return
                               //   depending if positive or negative comparison
    return flip ? !unsigned_greater(lhs, rhs) : unsigned_greater(lhs, rhs);
  }

  static bool equal(const Integer& lhs, const Integer& rhs) {
    if(lhs.negative_ xor rhs.negative_) return false; // Not same if opposite signs
    if(lhs.length() != rhs.length()) return false;    // Not same if different lengths
    for(size_t i = lhs.length() - 1; i >= 0; --i) { // must be same length and sign
      if(lhs.number_[i] != rhs.number_[i]) return false; // some digit doesn't match
    }
    return true; // all same
  }

  // lhs is positive and rhs negative
  // add 2 integers of different sign
  static Integer add_different(const Integer& lhs, const Integer& rhs) {
    Integer ret;
    ret.number_.clear();
    std::map<size_t, uint8_t> carry_map;
    if(unsigned_greater(lhs, rhs)) { // use lhs as 'top' number
      size_t pos = 0;
      for(; pos < rhs.length(); ++pos) { // move length out
        if(carry_map.count(pos)) { // is carried in this position
          if(carry_map[pos] >= rhs.number_[pos])
           ret.append(carry_map[pos] - rhs.number_[pos]);
          else {
            size_t carried_pos = 1;
            ret.append(10 + carry_map[pos] - rhs.number_[pos]);
            while(lhs.number_[pos + carried_pos] == 0) carry_map[pos + carried_pos++] = 9;
            carry_map[pos + carried_pos] = lhs.number_[pos + carried_pos] - 1;
          } 
        } else { // isnt carried in this position
          if(lhs.number_[pos] >= rhs.number_[pos]) 
            ret.append(lhs.number_[pos] - rhs.number_[pos]);
          else { // need to use carrying
            size_t carried_pos = 1;
            ret.append(10 + lhs.number_[pos] - rhs.number_[pos]);
            while(lhs.number_[pos + carried_pos] == 0) carry_map[pos + carried_pos++] = 9;
            carry_map[pos + carried_pos] = lhs.number_[pos + carried_pos] - 1;
          }
        }
      }
      for(; pos < lhs.length(); ++pos) { // move out
        if(carry_map.count(pos)) ret.append(carry_map[pos]);
        else ret.append(lhs.number_[pos]);
      }
    } else { // use rhs as 'top' number
      ret.negative_ = true;
      size_t pos = 0;
      for(; pos < lhs.length(); ++pos) { // move length out
        if(carry_map.count(pos)) { // is carried in this position
          if(carry_map[pos] >= lhs.number_[pos])
           ret.append(carry_map[pos] - lhs.number_[pos]);
          else {
            size_t carried_pos = 1;
            ret.append(10 + carry_map[pos] - lhs.number_[pos]);
            while(rhs.number_[pos + carried_pos] == 0) carry_map[pos + carried_pos++] = 9;
            carry_map[pos + carried_pos] = rhs.number_[pos + carried_pos] - 1;
          }
        } else { // isnt carried in this position
          if(rhs.number_[pos] >= lhs.number_[pos])
            ret.append(rhs.number_[pos] - lhs.number_[pos]);
          else { // need to use carrying
            size_t carried_pos = 1;
            ret.append(10 + rhs.number_[pos] - lhs.number_[pos]);
            while(rhs.number_[pos + carried_pos] == 0) carry_map[pos + carried_pos++] = 9;
            carry_map[pos + carried_pos] = rhs.number_[pos + carried_pos] - 1;
          }
        }
      }
      for(; pos < rhs.length(); ++pos) { // move out
        if(carry_map.count(pos)) ret.append(carry_map[pos]);
        else ret.append(rhs.number_[pos]);
      }
    }

    // remove leading 0s
    while(ret.length() > 1 && ret.number_[ret.length() - 1] == 0) ret.number_.pop_back(); 
    return ret;
  }

  // add 2 Integers of the same sign
  static Integer add_same(const Integer& lhs, const Integer& rhs, bool negative) {
    Integer ret; 
    ret.number_.clear();
    ret.negative_ = negative; // Adding 2 negatives
    uint8_t carry = 0;
    bool lhs_max = lhs.length() >= rhs.length();
    size_t pos = 0;
    size_t both_max = lhs_max ? rhs.length() : lhs.length();
    while(pos < both_max) {
      size_t val = lhs.number_[pos] + rhs.number_[pos] + carry;
      ret.append(val % 10);
      carry = val / 10;
      ++pos;
    }
    if(lhs_max) {
      while(pos < lhs.length()) {
        size_t val = lhs.number_[pos] + carry;
        ret.append(val % 10);
        carry = val / 10;
        ++pos;
      }
    } else {
      while(pos < rhs.length()) { 
        size_t val = rhs.number_[pos] + carry;
        ret.append(val % 10);
        carry = val / 10;
        ++pos;
      }
    }
    if(carry != 0) ret.append(carry);
    return ret;
  }

  // multiply 2 Integer objects
  static Integer multiply(const Integer& lhs, const Integer& rhs) {
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
          ret.append(val % 10);
          carry = val / 10;
        } else {
          val += ret.number_[pos + offset];
          ret.number_[pos + offset] = val % 10;
          carry = val / 10;
        }
      }
      size_t carry_pos = 0;
      while(carry != 0) { // TO DO : Think if this will always be an append
        if(pos + offset + carry_pos >= ret.length()) {
          ret.append(carry % 10);
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

  friend const Integer operator+(const Integer&, const Integer&);
  friend const Integer operator-(const Integer&, const Integer&);
  friend const Integer operator*(const Integer&, const Integer&);
  
  friend bool operator==(const Integer&, const Integer&);
  friend bool operator!=(const Integer&, const Integer&);
  friend bool operator<(const Integer&, const Integer&);
  friend bool operator>(const Integer&, const Integer&);
  friend bool operator<=(const Integer&, const Integer&);
  friend bool operator>=(const Integer&, const Integer&);

  friend std::ostream& operator<<(std::ostream&, const Integer&);

};

Integer& Integer::operator+=(const Integer& rhs) { // TO DO : Make this define +
  auto value = *this + rhs;
  *this = value;
  return *this;
}

Integer& Integer::operator-=(const Integer& rhs) { // TO DO : Make this define -
  auto value = *this - rhs;
  *this = value;
  return *this;
}

Integer& Integer::operator*=(const Integer& rhs) { // TO DO : Make this define *
  auto value = *this * rhs;
  *this = value;
  return *this;
}

Integer& Integer::operator++() {
  static const Integer one(1);
  *this += one;
  return *this;
}

Integer Integer::operator++(int) {
  Integer temp = *this;
  ++*this;
  return temp;
}

Integer& Integer::operator--() {
  static const Integer neg_one(-1);
  *this += neg_one;
  return *this;
}

Integer Integer::operator--(int) {
  Integer temp = *this;
  --*this;
  return temp;
}

const Integer operator+(const Integer& lhs, const Integer& rhs) {
  if(lhs.negative_ == rhs.negative_) return Integer::add_same(lhs, rhs, lhs.negative_);
  else return lhs.negative_ ? Integer::add_different(rhs, lhs) : Integer::add_different(lhs, rhs);
}

const Integer operator-(const Integer& lhs, const Integer& rhs) {
  if(lhs.negative_ == !rhs.negative_) return Integer::add_same(lhs, rhs, lhs.negative_);
  else return lhs.negative_ ? Integer::add_different(rhs, lhs) : Integer::add_different(lhs, rhs);
}

const Integer operator*(const Integer& lhs, const Integer& rhs) {
  return Integer::multiply(lhs, rhs);
}

bool operator==(const Integer& lhs, const Integer& rhs) {
  return Integer::equal(lhs, rhs);
}

bool operator!=(const Integer& lhs, const Integer& rhs) {
  return !Integer::equal(lhs, rhs);
}

bool operator<(const Integer& lhs, const Integer& rhs) {
  return Integer::greater(rhs, lhs);
}

bool operator>(const Integer& lhs, const Integer& rhs) {
  return Integer::greater(lhs, rhs);
}

bool operator<=(const Integer& lhs, const Integer& rhs) {
  return Integer::greater(rhs, lhs) || Integer::equal(lhs, rhs);
}

bool operator>=(const Integer& lhs, const Integer& rhs) {
  return Integer::greater(lhs, rhs) || Integer::equal(lhs, rhs);
}

std::ostream& operator<< (std::ostream& os, const Integer& s) {
  if(s.negative_) os << "-";
  for(int i = s.number_.size() - 1; i != -1; --i) os << unsigned(s.number_[i]);
  return os;
}

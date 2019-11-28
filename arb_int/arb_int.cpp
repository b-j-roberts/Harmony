#include <vector>
#include <map>

#include <iostream> // TO DO : Debug

// TO DO : think about division?
// TO DO : modulus
// TO DO : different bases?
// TO DO : allow scientific notation?

class Integer {

public:

  std::vector<uint8_t> number_;
  bool negative_;

  // TO DO : Make default 0?
  explicit Integer(): negative_(false) { }

  explicit Integer(int num): negative_(num < 0) {
    if(num == 0) number_.push_back(0);
    if(negative_) num *= -1;
    while(num != 0) {
      number_.push_back(num % 10);
      num /= 10;
    }
  }

  size_t length() const { return number_.size(); }

  // TO DO : Think about when equal
  // tell if lhs is larger than rhs (ignoring sign)
  bool unsigned_greater(const Integer& lhs, const Integer& rhs) const {
    if(lhs.length() == rhs.length()) {// if same length
      for(size_t i = lhs.length() - 1; i >= 0; --i) {
        if(lhs.number_[i] == rhs.number_[i]) continue;
        else return lhs.number_[i] > rhs.number_[i]; 
      }
    } else { // compare by length
      return lhs.length() > rhs.length();
    }
    return true; // They are equal
  }

  // tell if lhs greater than rhs
  bool greater(const Integer& lhs, const Integer& rhs) const {
    if(lhs.negative_ xor rhs.negative_) return rhs.negative_; 
    bool flip = lhs.negative_; // if comparing neg or pos, flip bc larger number_ flips return
                               //   depending if positive or negative comparison
    return flip ? !unsigned_greater(lhs, rhs) : unsigned_greater(lhs, rhs);
  }

  bool equal(const Integer& lhs, const Integer& rhs) const {
    if(lhs.negative_ xor rhs.negative_) return false; // Not same if opposite signs
    if(lhs.length() != rhs.length()) return false;    // Not same if different lengths
    for(size_t i = lhs.length() - 1; i >= 0; --i) { // must be same length and sign
      if(lhs.number_[i] != rhs.number_[i]) return false; // some digit doesn't match
    }
    return true; // all same
  }

  // lhs is positive and rhs negative
  // add 2 integers of different sign
  Integer add_different(const Integer& lhs, const Integer& rhs) const {
    Integer ret;
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
    while(ret.number_[ret.length() - 1] == 0) ret.number_.pop_back(); // remove leading 0s
    return ret;
  }

  // add 2 Integers of the same sign
  Integer add_same(const Integer& lhs, const Integer& rhs) const {
    Integer ret; 
    if(lhs.negative_ && rhs.negative_) ret.negative_ = true; // Adding 2 negatives
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
      while(pos < lhs.length()) { // TO DO : Move length out
        size_t val = lhs.number_[pos] + carry;
        ret.append(val % 10);
        carry = val / 10;
        ++pos;
      }
    } else {
      while(pos < rhs.length()) { // TO DO : Move length out
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
  Integer multiply(const Integer& lhs, const Integer& rhs) const {
    Integer ret;
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
    return ret;
  }

  void print() {
    if(negative_) std::cout << '-';
    for(size_t i = number_.size() - 1; i != -1; --i) std::cout << unsigned(number_[i]);
    std::cout << std::endl;
  } 

private:

  void append(uint8_t i) { number_.push_back(i); }

};

int main() {

  size_t x = 1239;

  Integer i_1(1000000); 
  Integer i_2(x); 
  Integer i_3(-12092); 
  Integer i_4(-109); 

  i_1.print();
  i_2.print();
  i_3.print();
  i_4.print();

  Integer added_1 = i_1.add_same(i_1, i_4);
  added_1.print();
  Integer added_2 = i_1.add_different(i_1, i_4);
  added_2.print();

  Integer multiplied = i_1.multiply(i_3, i_2);
  multiplied.print();

  return 0;

}
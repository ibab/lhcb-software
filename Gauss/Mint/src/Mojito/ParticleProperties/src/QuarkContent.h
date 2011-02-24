#ifndef DALITZ_MINT_QUARK_CONTENT_HH
#define DALITZ_MINT_QUARK_CONTENT_HH

#include <vector>
#include <iostream>
#include <map>

class QuarkContent : public std::vector<int>{
  static char _names[6];
  static char _NAMES[6];
  static std::map<char, int> _positions;
  static bool initPositions();
 public:
  QuarkContent();
  QuarkContent(const QuarkContent& other);
  void antiThis();
  bool initFromString(const std::string& str);


  int content(int quarkNumber) const;
  int content(long int quarkNumber) const;
  int content(unsigned int quarkNumber) const;
  // all this because otherwise C doesn't know 
  // wether to convert to int or char
  //
  int content(char quarkName) const;
  int content(const std::string& quarkName) const;

  char nameFromPosition(int i) const;
  int positionFromName(char c) const;

  void print(std::ostream& os = std::cout) const;
  
  QuarkContent& operator+=(const QuarkContent& rhs);
  QuarkContent& operator-=(const QuarkContent& rhs);

  QuarkContent operator+(const QuarkContent& rhs) const;
  QuarkContent operator-(const QuarkContent& rhs) const;

};

std::ostream& operator<<(std::ostream& st, const QuarkContent& qc);
#endif
//

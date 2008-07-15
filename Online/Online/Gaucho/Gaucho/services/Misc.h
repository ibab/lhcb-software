#ifndef Misc_H
#define Misc_H 1

//  Author: jotalo, 14/04/2008

class Misc {

public:
  Misc();
  ~Misc();

  static std::vector<std::string>  splitString(const std::string &str, const char *s);
  static bool matchString(const std::string &str, const std::vector<std::string> &strV);
  static bool matchStringCaseIns(const std::string &str, const std::vector<std::string> &strV);
  static bool isStringInVector(const std::string &str, const std::vector<std::string> &strV);
  static size_t findCaseIns(const std::string& str1, const std::string& str2);
  static bool compareCharsCaseIns(char ch1, char ch2);
};

#endif    // Misc_H

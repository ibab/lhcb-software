#include <cstring>
#include <stdlib.h>
#include <iostream>
#include <string>
#include <vector>
#include <algorithm>
#include "GauchoAppl/Misc.h"

Misc::Misc()
{
}
Misc::~Misc()
{
}

std::vector<std::string>  Misc::splitString(const std::string &str, const char *s){

  std::vector<std::string> found;

  std::size_t begin = 0;
  while (true){
    std::size_t end = str.find(s, begin);
    if (std::string::npos == end) break;

    found.push_back(str.substr(begin, end - begin));
    begin = end + 1;
  }

  if (begin < str.size()) found.push_back(str.substr(begin, str.size()));

  return found;
}

bool Misc::matchString(const std::string &str, const std::vector<std::string> &strV) {
  std::vector<std::string>::const_iterator strVIt;
  size_t begin = 0;
  for(strVIt=strV.begin(); strVIt!=strV.end(); ++strVIt){
    if (begin >= str.size()) return false;
    size_t pos = str.find((*strVIt), begin);
    if (pos == std::string::npos) return false;
    begin = pos + (*strVIt).size();
  }
  return true;
}

bool Misc::matchStringCaseIns (const std::string &str, const std::vector<std::string> &strV) {
  std::vector<std::string>::const_iterator strVIt;
  size_t begin = 0;
  for(strVIt=strV.begin(); strVIt!=strV.end(); ++strVIt) {
    if (begin >= str.size()) return false;
    std::string lastPart = str.substr(begin);
    size_t pos = Misc::findCaseIns(lastPart, (*strVIt));
    if (pos == std::string::npos) return false;
    begin = begin + pos + (*strVIt).size();
  }
  return true;
}


bool Misc::isStringInVector(const std::string &str, const std::vector<std::string> &strV) {
  std::vector<std::string>::const_iterator it;
  for(it=strV.begin(); it!=strV.end(); ++it)
    if (str.compare(*it) ==0) return true;
  return false;
}

size_t Misc::findCaseIns(const std::string& str1, const std::string& str2) {
  std::string::const_iterator pos = std::search(str1.begin(), str1.end(), str2.begin(), str2.end(), Misc::compareCharsCaseIns);
  if (pos == str1.end()) return std::string::npos;
  return pos - str1.begin();
}

bool Misc::compareCharsCaseIns(char ch1, char ch2) {
  return toupper((unsigned char)ch1) == toupper((unsigned char)ch2);
}

std::string Misc::stringToUpper(std::string strToConvert)
{//change each element of the string to upper case
   for(unsigned int i=0;i<strToConvert.length();i++)
   {
      strToConvert[i] = toupper(strToConvert[i]);
   }
   return strToConvert;//return the converted string
}


void Misc::printSerializedString(const char * c, int size)
{
  int i=0;
  std::cout << "content (" << size << " bytes):" << std::endl;
  while(i < size && c != 0)
  {
    if( c[i] >= 33 && c[i] <= 126) std::cout << c[i];
    else std::cout << ".";
    i++;
  }
  std::cout << std::endl << "/content" << std::endl;
}

std::string Misc::charToString(char * c, int size)
{
  int i=0;

 // cout << "content (" << size << " bytes):" << endl;
  std::string value = "";
  while(i < size && c != 0)
  {
    if( c[i] >= 33 && c[i] <= 126) value = value + c[i];
    else value = value + ".";
    i++;
  }
  return value;
}

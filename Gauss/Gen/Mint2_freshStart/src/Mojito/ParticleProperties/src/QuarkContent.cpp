#include "Mint/QuarkContent.h"
#include <vector>
#include <iostream>

using namespace std;

char QuarkContent::_names[6] = {'d', 'u', 's', 'c', 'b', 't'};
char QuarkContent::_NAMES[6] = {'D', 'U', 'S', 'C', 'B', 'T'};
map<char, int> QuarkContent::_positions;

QuarkContent::QuarkContent()
  : vector<int>(6, 0)
{
  initPositions();
}

QuarkContent::QuarkContent(const QuarkContent& other)
  : vector<int>(other)
{}

bool QuarkContent::initPositions(){
  if(_positions.empty()){
    for(int i=0; i < 6; i++) _positions[_names[i]]=i;
    for(int i=0; i < 6; i++) _positions[_NAMES[i]]=i;
  }
  return true;
}

void QuarkContent::antiThis(){
  for(unsigned int i=0; i < this->size(); i++){
    (*this)[i] *= -1;
  }
}

bool QuarkContent::initFromString(const string& str){
  for(unsigned int i=0; i < this->size(); i++){
    (*this)[i]=0;
  }
  unsigned int pos =str.find("non-qQ");
  if(pos < str.size())return true;
  
  std::string localQ = str;
  pos = localQ.find("sqrt");
  if(pos < localQ.size()){
    localQ.replace(pos, 4, "");
  }
  //cout << "str : " << str << " localQ " << localQ << endl;
  
  for(unsigned int i=0; i < localQ.size(); i++){
    if('d' == localQ[i]) (*this)[0]++;
    if('D' == localQ[i]) (*this)[0]--;
    if('u' == localQ[i]) (*this)[1]++;
    if('U' == localQ[i]) (*this)[1]--;
    if('s' == localQ[i]) (*this)[2]++;
    if('S' == localQ[i]) (*this)[2]--;
    if('c' == localQ[i]) (*this)[3]++;
    if('C' == localQ[i]) (*this)[3]--;
    if('b' == localQ[i]) (*this)[4]++;
    if('B' == localQ[i]) (*this)[4]--;
    if('t' == localQ[i]) (*this)[5]++;
    if('T' == localQ[i]) (*this)[5]--;
  }
  return true;
}

int QuarkContent::content(long int quarkNumber) const{
  return content( (int) quarkNumber);
}
int QuarkContent::content(int quarkNumber) const{
  if(quarkNumber < 0) return 0;
  return content( (unsigned int) quarkNumber);
}
int QuarkContent::content(unsigned int quarkNumber) const{
  if(quarkNumber >= this->size()) return 0;
  return (*this)[quarkNumber];
}

int QuarkContent::content(const std::string& quarkName) const{
  if(quarkName.empty()) return 0;
  return content(quarkName[0]);
}

int QuarkContent::content(char quarkName) const{
  return content(positionFromName(quarkName));
}
char QuarkContent::nameFromPosition(int i) const{
  if(i < 0 || i >=6) return 'X';
  else return _names[i];
}
int QuarkContent::positionFromName(char c) const{
  map<char, int>::const_iterator it = _positions.find(c);
  if(_positions.end() == it) return -9999;
  else return it->second;
}

void QuarkContent::print(std::ostream& os) const{
  for(unsigned int i=0; i < this->size(); i++){
    os << "(" << nameFromPosition(i) << ":" << (*this)[i] << ")";
    if(i + 1< this->size()) os << " ";
  }
}

QuarkContent& QuarkContent::operator+=(const QuarkContent& rhs){
  unsigned int max = this->size();
  if(rhs.size() < max) max = rhs.size();
  for(unsigned int i=0; i < max; i++){
    (*this)[i] += rhs[i];
  }
  return *this;
}
QuarkContent& QuarkContent::operator-=(const QuarkContent& rhs){
  unsigned int max = this->size();
  if(rhs.size() < max) max = rhs.size();
  for(unsigned int i=0; i < max; i++){
    (*this)[i] -= rhs[i];
  }
  return *this;
}

QuarkContent QuarkContent::operator+(const QuarkContent& rhs) const{
  QuarkContent returnVal(*this);  
  returnVal += rhs;
  return returnVal;
}
QuarkContent QuarkContent::operator-(const QuarkContent& rhs) const{
  QuarkContent returnVal(*this);  
  returnVal -= rhs;
  return returnVal;
}
std::ostream& operator<<(std::ostream& st, const QuarkContent& qc){
  qc.print(st);
  return st;
}

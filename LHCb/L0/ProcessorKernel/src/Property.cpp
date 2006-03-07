#include "ProcessorKernel/Property.h"



/** 
    Build vectors
*/
const std::vector<int> L0Muon::Property::getVectorOfInt() const{
  std::vector<int> vi;
  const char * str = m_value.c_str();
  int length = m_value.size();
  char * endptr;

  for (int k=0;k<length;k++){
    int i = strtol(str,&endptr,0);
    if (endptr==str) {
      break;
    }
    str = endptr+1;
    vi.push_back(i);
  }
  return vi;
}
const std::vector<long> L0Muon::Property::getVectorOfLong() const{
  std::vector<long> vl;
  const char * str = m_value.c_str();
  int length = m_value.size();
  char * endptr;

  for (int k=0;k<length;k++){
    long l = strtol(str,&endptr,0);
    if (endptr==str) {
      break;
    }
    str = endptr+1;
    vl.push_back(l);
  }
  return vl;
}
const std::vector<float> L0Muon::Property::getVectorOfFloat() const{
  std::vector<float> vf;
  const char * str = m_value.c_str();
  int length = m_value.size();
  char * endptr;

  for (int k=0;k<length;k++){
    float f = (float)strtod(str,&endptr);
    if (endptr==str) {
      break;
    }
    str = endptr+1;
    vf.push_back(f);
  }
  return vf;
}
const std::vector<double> L0Muon::Property::getVectorOfDouble() const{
  std::vector<double> vd;
  const char * str = m_value.c_str();
  int length = m_value.size();
  char * endptr;

  for (int k=0;k<length;k++){
    double d = strtod(str,&endptr);
    if (endptr==str) {
      break;
    }
    str = endptr+1;
    vd.push_back(d);
  }
  return vd;
}
/**
    Define ouput operator.
    Output the internal string.
*/
std::ostream& operator<< (std::ostream& os, const L0Muon::Property  &property)
{

  const std::string s = property;
  return os << s;

}

// author: Jonas Rademacker (Jonas.Rademacker@bristol.ac.uk)
// status:  Mon 9 Feb 2009 19:17:58 GMT
#include "Mint/DalitzCoordinate.h"
#include <sstream>

#include <algorithm>

using namespace std;

bool DalitzCoordinate::I_am_Consecutive() const{
  if(this->size() <=1) return true;
  for(unsigned int i=1; i< this->size(); i++){
    if( (*this)[i] != (*this)[i-1] + 1) return false;
  }
  return true;
}

DalitzCoordinate::DalitzCoordinate()
  : vector<int>(1)
  , _mi(-9999)
  , _ma(-9999)
  , _val(-9999)
{
  (*this)[0] = -9999;
  makeName();
}

DalitzCoordinate::DalitzCoordinate(int i, int j)
  : vector<int>(2)
  , _mi(-9999)
  , _ma(-9999)
  , _val(-9999)
{
  (*this)[0] = i;
  (*this)[1] = j;
  sort(this->begin(), this->end());
  makeName();
}
DalitzCoordinate::DalitzCoordinate(int i, int j, int k)
  : vector<int>(3)
  , _mi(-9999)
  , _ma(-9999)
  , _val(-9999)
{
  (*this)[0] = i;
  (*this)[1] = j;
  (*this)[2] = k;
  sort(this->begin(), this->end());
  makeName();
}

DalitzCoordinate::DalitzCoordinate(const std::vector<int>& other)
  : vector<int>( (vector<int>) other)
  , _mi(-9999)
  , _ma(-9999)
  , _val(-9999)
{
  sort(this->begin(), this->end());
  makeName();
}
DalitzCoordinate::DalitzCoordinate(const DalitzCoordinate& other)
  : vector<int>( (vector<int>) other)
  , _mi(other._mi)
  , _ma(other._ma)
  , _val(other._val)
  , _name(other._name)
{
}

DalitzCoordinate& DalitzCoordinate::operator=(const DalitzCoordinate& other)
{
  this->resize(other.size());
  for(unsigned int i=0; i< other.size(); i++) (*this)[i] = other[i];
  _mi  = other._mi;
  _ma  = other._ma;
  _val = other._val;
  _name = other._name;
  return *this;
}

DalitzCoordinate DalitzCoordinate::mapMe(const Permutation& perm) const{
  /*
  cout << "DalitzCoordinate::mapMe of " << (*this) 
       << " called with\n " << perm << endl;
  */
  DalitzCoordinate mapped(*this);

  for(unsigned int i=0; i< this->size(); i++){
    //    cout << "mapping : " << (*this)[i] << endl;
    mapped[i] = perm[ (*this)[i] ];  
    //    cout << "mapped it to " << mapped[i] << endl;
  }
  sort(mapped.begin(), mapped.end());
  return mapped;
}


std::vector<DalitzCoordinate> DalitzCoordinate::split(int n) const{

  bool dbThis=false;

  std::vector<DalitzCoordinate> cList;
  cList.clear();

  double newWidth = (this->max() - this->min()) /((double) n);
  if(dbThis){
    cout << " DalitzCoordinate::split " << *this << endl;
    cout << "\t Currently: mi = " << min()
	 << " ma = " << max() << "\n\t";
  }
  for(int l=0; l < n; l++){
    DalitzCoordinate c(*this);
    double mi = this->min() + l*newWidth;
    double ma = this->min() + (l+1.0)*newWidth;
    if(dbThis) cout << ", ( " << mi << ", " << ma << " )";
    c.setMinMax(mi, ma);
    cList.push_back(c);
  }
  if(dbThis) cout << endl;
  return cList;
}


const std::string& DalitzCoordinate::name() const{
  return _name;
}

std::string& DalitzCoordinate::makeName(){
  std::stringstream strm;
  strm << "sij(";
  for(unsigned int i=0; i < this->size(); i++){
    if(i > 0) strm << ",";
    strm << (*this)[i];
  }
  strm << ")";
  std::string ing;
  strm >> ing;
  _name = ing;
  return _name;
}
std::string DalitzCoordinate::nameFileSave() const{
  std::stringstream strm;
  strm << "s";
  for(unsigned int i=0; i < this->size(); i++){
    strm << (*this)[i];
  }
  std::string ing;
  strm >> ing;
  return ing;
}

void DalitzCoordinate::print(std::ostream& os) const{
  os << this->name();
  os << ": val = " << this->val()
     << ", mi = "  << this->min()
     << ", ma = "  << this->max();
  return;
}
bool DalitzCoordinate::sameIndices(const DalitzCoordinate& rhs) const{
  if(this->size() != rhs.size()) return false;
  for(unsigned int i=0; i< size(); i++){
    if((*this)[i] != rhs[i]) return false;
  }
  
  return true;
}

bool DalitzCoordinate::operator==(const DalitzCoordinate& rhs) const{

  if(this->size() != rhs.size()) return false;
  for(unsigned int i=0; i< size(); i++){
    if((*this)[i] != rhs[i]) return false;
  }
  if(this->min() != rhs.min()) return false;
  if(this->max() != rhs.max()) return false;
  if(this->val() != rhs.val()) return false;
  
  return true;
}

bool DalitzCoordinate::operator!=(const DalitzCoordinate& rhs) const{
  return ! ((*this) == rhs);
}

// non-members:
std::ostream& operator<<(std::ostream& os, const DalitzCoordinate& c){
  c.print(os);
  return os;
}
//

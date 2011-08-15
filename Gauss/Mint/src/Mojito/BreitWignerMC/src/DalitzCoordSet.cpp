
#include "Mint/DalitzCoordSet.h"
#include <algorithm>

using namespace std;

DalitzCoordSet::DalitzCoordSet()
  : std::map<DalitzCoordKey, DalitzCoordinate>()
{
}

DalitzCoordSet::DalitzCoordSet(const DalitzCoordinate& dc_1)
  : std::map<DalitzCoordKey, DalitzCoordinate>()
{
  this->add(dc_1);
}
DalitzCoordSet::DalitzCoordSet(const std::vector<int>& dc_1)
  : std::map<DalitzCoordKey, DalitzCoordinate>()
{
  this->add((DalitzCoordinate)dc_1);
}
DalitzCoordSet::DalitzCoordSet(const DalitzCoordinate& dc_1
			       , const DalitzCoordinate& dc_2)
  : std::map<DalitzCoordKey, DalitzCoordinate>()
{
  this->add(dc_1, dc_2);
}

DalitzCoordSet::DalitzCoordSet(const DalitzCoordinate& dc_1
			       , const DalitzCoordinate& dc_2
			       , const DalitzCoordinate& dc_3)
  : std::map<DalitzCoordKey, DalitzCoordinate>()
{
  this->add(dc_1, dc_2, dc_3);
}
DalitzCoordSet::DalitzCoordSet(const DalitzCoordinate& dc_1
			       , const DalitzCoordinate& dc_2
			       , const DalitzCoordinate& dc_3
			       , const DalitzCoordinate& dc_4)
  : std::map<DalitzCoordKey, DalitzCoordinate>()
{
  this->add(dc_1, dc_2, dc_3, dc_4);
}
DalitzCoordSet::DalitzCoordSet(const DalitzCoordinate& dc_1
			       , const DalitzCoordinate& dc_2
			       , const DalitzCoordinate& dc_3
			       , const DalitzCoordinate& dc_4
			       , const DalitzCoordinate& dc_5)
  : std::map<DalitzCoordKey, DalitzCoordinate>()
{
  this->add(dc_1, dc_2, dc_3, dc_4, dc_5);
}
DalitzCoordSet::DalitzCoordSet(const DalitzCoordSet& other)
  : std::map<DalitzCoordKey, DalitzCoordinate>(other)
{}

void DalitzCoordSet::add(const DalitzCoordinate& dc_1){
  (*this)[dc_1.myKey()] = dc_1;
}
void DalitzCoordSet::add(const DalitzCoordinate& dc_1
			 , const DalitzCoordinate& dc_2){
  add(dc_1); add(dc_2);
}
void DalitzCoordSet::add(const DalitzCoordinate& dc_1
			 , const DalitzCoordinate& dc_2
			 , const DalitzCoordinate& dc_3){
  add(dc_1, dc_2); add(dc_3);
}
void DalitzCoordSet::add(const DalitzCoordinate& dc_1
			 , const DalitzCoordinate& dc_2
			 , const DalitzCoordinate& dc_3
			 , const DalitzCoordinate& dc_4){
  add(dc_1, dc_2, dc_3); add(dc_4);
}
void DalitzCoordSet::add(const DalitzCoordinate& dc_1
			 , const DalitzCoordinate& dc_2
			 , const DalitzCoordinate& dc_3
			 , const DalitzCoordinate& dc_4
			 , const DalitzCoordinate& dc_5){
  add(dc_1, dc_2, dc_3, dc_4); add(dc_5);
}

void DalitzCoordSet::add(const DalitzCoordSet& dcl){
  this->insert(dcl.begin(), dcl.end());
}

std::string DalitzCoordSet::name() const{
  std::string n="";
  int counter=0;
  for(std::map<DalitzCoordKey, DalitzCoordinate>::const_iterator it = this->begin();
      it != this->end(); it++){
    if(0 != counter++) n+= "_x_";
    n += it->second.name();
  }
  return n;
}

std::string DalitzCoordSet::nameFileSave() const{
  std::string n="";
  int counter=0;
  for(std::map<DalitzCoordKey, DalitzCoordinate>::const_iterator it = this->begin();
      it != this->end(); it++){
    if(0 != counter++) n+= "_x_";
    n += it->second.nameFileSave();
  }
  return n;
}


//----
//

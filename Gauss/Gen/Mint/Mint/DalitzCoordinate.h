#ifndef DALITZCOORDINATE_HH
#define DALITZCOORDINATE_HH
// author: Jonas Rademacker (Jonas.Rademacker@bristol.ac.uk)
// status:  Mon 9 Feb 2009 19:17:58 GMT

#include <vector>

#include "Mint/Permutation.h"

#include <iostream>
#include <string>

typedef std::vector<int> DalitzCoordKey;

class DalitzCoordinate : public std::vector<int>{
  double _mi, _ma, _val;
  std::string _name;
  std::string& makeName();

 public:

  DalitzCoordinate();
  DalitzCoordinate(int i, int j);
  DalitzCoordinate(int i, int j, int k);
  DalitzCoordinate(const std::vector<int>& other);
  DalitzCoordinate(const DalitzCoordinate& other);

  DalitzCoordinate& operator=(const DalitzCoordinate& other);

  void setMin(double min){_mi = min;}
  void setMax(double max){_ma = max;}
  void setMinMax(double min, double max){
    setMin(min); setMax(max);}
  void setVal(double val){_val = val;}

  double min()const{return _mi;}
  double max()const{return _ma;}
  double val()const{return _val;}

  bool I_am_Consecutive() const;

  DalitzCoordinate mapMe(const Permutation& perm) const;

  std::vector<DalitzCoordinate> split(int n) const;
  
  void print(std::ostream& os = std::cout) const;
  const std::string& name() const;
  std::string nameFileSave() const;
  
  bool sameIndices(const DalitzCoordinate& rhs) const;
  bool operator==(const DalitzCoordinate& rhs) const;
  bool operator!=(const DalitzCoordinate& rhs) const;

  const DalitzCoordKey& myKey() const{return *this;}

};
std::ostream& operator<<(std::ostream& os, const DalitzCoordinate& c);


#endif
//

#ifndef MINT_DALITZ_COORD_SET_HH
#define MINT_DALITZ_COORD_SET_HH

#include "Mint/DalitzCoordinate.h"
#include <map>

class DalitzCoordSet : public std::map<DalitzCoordKey, DalitzCoordinate>{
  // this list is always sorted (being a map)
  // map instead of set, so I can modify values inside DalitzCoordinate
 public:
  DalitzCoordSet();
  DalitzCoordSet(const DalitzCoordinate& dc_1);
  DalitzCoordSet(const std::vector<int>& dc_1);

  DalitzCoordSet(const DalitzCoordinate& dc_1
		 , const DalitzCoordinate& dc_2);
  DalitzCoordSet(const DalitzCoordinate& dc_1
		 , const DalitzCoordinate& dc_2
		 , const DalitzCoordinate& dc_3);
  DalitzCoordSet(const DalitzCoordinate& dc_1
		 , const DalitzCoordinate& dc_2
		 , const DalitzCoordinate& dc_3
		 , const DalitzCoordinate& dc_4);
  DalitzCoordSet(const DalitzCoordinate& dc_1
		 , const DalitzCoordinate& dc_2
		 , const DalitzCoordinate& dc_3
		 , const DalitzCoordinate& dc_4
		 , const DalitzCoordinate& dc_5);
  DalitzCoordSet(const DalitzCoordSet& other);

  std::string name() const;
  std::string nameFileSave() const;

  void add(const DalitzCoordinate& dc_1);
  void add(const DalitzCoordinate& dc_1
	   , const DalitzCoordinate& dc_2);
  void add(const DalitzCoordinate& dc_1
	   , const DalitzCoordinate& dc_2
	   , const DalitzCoordinate& dc_3);
  void add(const DalitzCoordinate& dc_1
	   , const DalitzCoordinate& dc_2
	   , const DalitzCoordinate& dc_3
	   , const DalitzCoordinate& dc_4);
  void add(const DalitzCoordinate& dc_1
	   , const DalitzCoordinate& dc_2
	   , const DalitzCoordinate& dc_3
	   , const DalitzCoordinate& dc_4
	   , const DalitzCoordinate& dc_5);
  void add(const DalitzCoordSet& dcl);
  
  void print(std::ostream& os) const;
};

std::ostream& operator<<(std::ostream& os, const DalitzCoordSet& dcs);
#endif
//
//

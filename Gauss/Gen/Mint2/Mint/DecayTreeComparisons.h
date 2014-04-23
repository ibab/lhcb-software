#ifndef DECAYTREE_COMPARISONS_HH
#define DECAYTREE_COMPARISONS_HH
// author: Jonas Rademacker (Jonas.Rademacker@bristol.ac.uk)
// status:  Mon 9 Feb 2009 19:18:03 GMT

#include "Mint/counted_ptr.h"

class lessBySVPAT{
 public:
  bool operator()(MINT::const_counted_ptr<DecayTree> a
		  , MINT::const_counted_ptr<DecayTree> b) const;
};

class lessBySVPAT_L{
 public:
  bool operator()(MINT::const_counted_ptr<DecayTree> a
		  , MINT::const_counted_ptr<DecayTree> b) const;
};

class moreByNDgtr{
 public:
  bool operator()(MINT::const_counted_ptr<DecayTree> a
		  , MINT::const_counted_ptr<DecayTree> b) const;
};

class moreByMass{
 public:
  bool operator()(MINT::const_counted_ptr<DecayTree> a
		  , MINT::const_counted_ptr<DecayTree> b) const;
};

class moreByL{
 public:
  bool operator()(MINT::const_counted_ptr<DecayTree> a
		  , MINT::const_counted_ptr<DecayTree> b) const;
};

class moreByID{
 public:
  bool operator()(MINT::const_counted_ptr<DecayTree> a
		  , MINT::const_counted_ptr<DecayTree> b) const;
};

class moreByAbsID_stillUnique{
 public:
  bool operator()(MINT::const_counted_ptr<DecayTree> a
		  , MINT::const_counted_ptr<DecayTree> b) const;
};

bool sortBySVPAT(DecayTree& A);
bool equalBySVPATRec(const DecayTree& A, const DecayTree& B);
bool equalBySVPAT(DecayTree A, DecayTree B);

bool sortBySVPAT_L(DecayTree& A);
bool equalBySVPAT_LRec(const DecayTree& A, const DecayTree& B);
bool equalBySVPAT_L(DecayTree A, DecayTree B);


bool sortByMoreDgtr(DecayTree& A);
bool sortByMoreMass(DecayTree& A);
bool sortByMoreL(DecayTree& A);
bool sortByMoreID(DecayTree& A);
bool sortByMoreAbsID_stillUnique(DecayTree& A);
bool standardSort(DecayTree& A);

#endif
//

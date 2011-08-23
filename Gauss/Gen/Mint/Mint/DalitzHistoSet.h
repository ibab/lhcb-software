#ifndef DALITZ_HISTO_SET_HH
#define DALITZ_HISTO_SET_HH
// author: Jonas Rademacker (Jonas.Rademacker@bristol.ac.uk)
// status:  Mon 9 Feb 2009 19:17:58 GMT

#include "Rtypes.h"

#include <map>
#include <string>
#include "Mint/DalitzEventPattern.h"
#include "Mint/DalitzHistogram.h"
#include "Mint/DalitzCoordSet.h"

class TTree;
class IDalitzEvent;

class DalitzHistoSet : public std::map< DalitzCoordSet, DalitzHistogram>{
  mutable TTree* _tree;
  std::string _name;
  void makeHistograms(const DalitzEventPattern& pat);
  bool makeDirectory(const std::string& asSubdirOf)const;
  const std::string& makeName();
 public:
  DalitzHistoSet();
  DalitzHistoSet(const DalitzHistoSet& other);
  void add(const DalitzHistogram& histo, double weight=1);
  void add(const DalitzHistoSet& hL, double weight=1);

  void multiply(const DalitzHistogram& hL);
  void multiply(const DalitzHistoSet& hL);
  void divide(const DalitzHistogram& hL);
  void divide(const DalitzHistoSet& hL);

  void addEvent(const IDalitzEvent* evtPtr, double weight=1);
  void scale(double sf);
  void setNormFactor(double sf=1);
  void clearAllHistos();

  const std::string& name() const;
  const std::string& dirName() const;
  std::string fullDirListFname(const std::string& asSubdirOf = ".") const;
  bool saveAsDir(const std::string& asSubdirOf=".") const;
  bool retrieveFromDir(const std::string& asSubDirOf=".");
  bool save(const std::string& filename="DalitzHistos.root") const;
  bool draw(const std::string& baseName=""
	    , const std::string& drawOpt=""
	    , const std::string& format="eps") const;

  bool drawWithFit(const DalitzHistoSet& fit
		   , const std::string& baseName=""
		   , const std::string& format="eps"
		   , const std::string& fitDrawOpt="HIST C SAME"
		   ) const;
  bool drawWithFitNorm(const DalitzHistoSet& fit
		       , const std::string& baseName=""
		       , const std::string& format="eps"
		       , const std::string& fitDrawOpt="HIST C SAME"
		       ) const;
  

  void setTitle(const std::string& title);
  void setFillColour(Color_t fcolor);
  void setLineColour(Color_t fcolor);

  double integral() const;

  DalitzHistoSet& operator*=(double sf);
  DalitzHistoSet operator*(double sf) const;
  DalitzHistoSet& operator/=(double sf);
  DalitzHistoSet operator/(double sf) const;
  DalitzHistoSet& operator+=(const DalitzHistoSet& rhs);
  DalitzHistoSet operator+(const DalitzHistoSet& rhs) const;
  DalitzHistoSet& operator-=(const DalitzHistoSet& rhs);
  DalitzHistoSet operator-(const DalitzHistoSet& rhs) const;
  DalitzHistoSet& operator*=(const DalitzHistoSet& rhs);
  DalitzHistoSet operator*(const DalitzHistoSet& rhs) const;
  DalitzHistoSet& operator/=(const DalitzHistoSet& rhs);
  DalitzHistoSet operator/(const DalitzHistoSet& rhs) const;
};

DalitzHistoSet operator*(double sf, const DalitzHistoSet& dhs);

#endif
//

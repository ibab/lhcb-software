#ifndef DALITZ_HISTOGRAM_TAMMTAMM_HH
#define DALITZ_HISTOGRAM_TAMMTAMM_HH
// author: Jonas Rademacker (Jonas.Rademacker@bristol.ac.uk)
// status:  Mon 9 Feb 2009 19:17:58 GMT

#include "TH1D.h"

#include "Mint/DalitzCoordinate.h"
#include "Mint/DalitzCoordSet.h"
#include "Mint/DalitzEventPattern.h"
#include "Mint/counted_ptr.h"
#include "Mint/CLHEPPhysicalConstants.h"

#include "TTree.h"
#include "TBranch.h"
#include "TH1.h"
#include "TCanvas.h"


#include <string>

class IDalitzEvent;

class DalitzHistogram{
 private:
  double _units;
  int _nbins;
  MINT::counted_ptr<TH1> _h;
  DalitzCoordSet _c;
  DalitzEventPattern _pat;
  std::string _name;
  std::string _smartTitle;
  mutable TTree* _tree;
  std::vector<int> _patForTree;
  
  void makeName();
  bool makeDirectory(const std::string& asSubdirOf=".")const;
  std::string valueFileName(const std::string& asSubdirOf="."
			    , const char* theName=0) const;
  std::string histoFileName(const std::string& asSubdirOf="."
			    , const char* theName=0) const;
  bool saveValues(const std::string& inDir=".") const;
  bool retrieveValues(const std::string& fromDirectory
		      , const char* theName=0);
  bool saveHisto(const std::string& inDir=".") const;
  bool retrieveHisto(const std::string& fromDir
		     , const char* theName=0);

 public:
  DalitzHistogram();
  DalitzHistogram(const DalitzCoordSet& c
		  , const DalitzEventPattern& p
		  , int bins=200
		  , double units=GeV*GeV
		  );
  //  DalitzHistogram(TTree* t);
  DalitzHistogram(const DalitzHistogram& other);

  const std::string& name() const{return _name;}
  void smartTitle(const DalitzCoordSet& c_in
  				  , const DalitzEventPattern& pat_in);
  const std::string smartTitle() const;
  const std::string hname() const;
  const std::string& htitle() const;
  const std::string& dirName() const;
  const DalitzEventPattern& pattern() const{return _pat;}
  //TTree* makeTree() const;
  //TTree* ttree() const;

  void init(const DalitzCoordSet& c, const DalitzEventPattern& p
	    , int bins = 100
	    , double units = GeV*GeV
	    );
  bool saveAsDir(const std::string& asSubdirOf=".") const;
  bool retrieveFromDir(const std::string& asSubdirOf="."
		       , const char* theName=0);
  //bool writeTree() const;
  void add(const DalitzHistogram& addMeToYou, double weight=1);
  void multiply(const DalitzHistogram& multiplyWith);
  void divide(const DalitzHistogram& divideBy);

  void setNormFactor(double f=1){_h->SetNormFactor(f);}

  void addEvent(const IDalitzEvent* evtPtr, double weight=1);

  MINT::const_counted_ptr<TH1> histo() const{return _h;}
  MINT::counted_ptr<TH1> histo() {return _h;}
  const DalitzCoordSet& coord() const{return _c;}
  DalitzCoordSet& coord() {return _c;}

  void scale(double sf);
  void setTitle(const std::string& title);
  void clearHisto();

  bool draw(const std::string& baseName=""
	    , const std::string& drawOpt=""
	    , const std::string& format="eps"
	    ) const;

  bool drawWithFit(const DalitzHistogram& fit
		   , const std::string& baseName=""
		   , const std::string& format="eps"
		   , const std::string& fitDrawOpt="HIST C SAME"
		   ) const;

  bool drawWithFit(	TCanvas& can
		   , const DalitzHistogram& fit
		   , const std::string& baseName=""
		   , const std::string& format="eps"
		   , const std::string& fitDrawOpt="HIST C SAME"
		   ) const;

  bool drawEachAmp( TCanvas& can
		, const std::string& baseName=""
	    , const std::string& drawOpt="SAME"
	    , const std::string& format="eps"
	    ) const;

  MINT::counted_ptr<TH1> getHisto();



  double integral() const;

  DalitzHistogram& operator=(const DalitzHistogram& other);
};


#endif
//

#ifndef AMPINITIALISER_HH
#define AMPINITIALISER_HH
// author: Jonas Rademacker (Jonas.Rademacker@bristol.ac.uk)
// status:  Mon 9 Feb 2009 19:18:02 GMT

#include <string>
#include "Mint/DecayTree.h"
#include "Mint/DalitzEventPattern.h"
#include "Mint/LineshapeMaker.h"
class AmpInitialiser{
  DecayTree _dt;
  char _SPD;
  bool _valid;
  std::string _lopt; // note: the linshape maker
  // figures out which lineshape to use by
  // knowing the particle types involved.
  // This is for additional options
  // One could be: ALWAYS_BREIT_WIGNER
  // or USE_XYZMODEL_FOR_RHO, or CLEO2012, or None 

  std::vector<double> _numOpts; // numerical options
  // (introduced for min/max in piece-wise defined functions)

  bool _autoSwap;
  
 public:
  AmpInitialiser();
  AmpInitialiser(const std::string& StandardisedDecayTreeName
		 , const std::string& lopt =""
		 , const std::vector<double>& numOpts = std::vector<double>()
		 , bool autoSwap=true
		 );
  AmpInitialiser(const DecayTree& dt_in
		 , char SPD_in='?'
		 , const std::string& lopt =""
		 , const std::vector<double>& numOpts = std::vector<double>()
		 , bool autoSwap=true
		 );
  AmpInitialiser(const DecayTree& dt_in
		 , const std::string& lopt
		 , const std::vector<double>& numOpts = std::vector<double>()
		 , bool autoSwap = true
		 );
  AmpInitialiser(const AmpInitialiser& other);
  AmpInitialiser& operator=(const AmpInitialiser& rhs);


  const DecayTree& tree() const;
  DalitzEventPattern getEventPattern()const;

  void setTree(const DecayTree& dt_in);
  char SPD() const;
  void setSPD(char SPD_in);

  const std::string& lopt() const;
  void setLopt(const std::string& lopt_in);
  void addLopt(const std::string& lopt_in);
  
  void setNumOpts(const std::vector<double>& numOpts_in=std::vector<double>());
  void setNumOpts(double n1);
  void setNumOpts(double n1, double n2);
  void setNumOpts(double n1, double n2, double n3);

  const std::vector<double>& numOpts() const;

  bool autoSwap() const;
  bool setAutoSwap(bool autoSwap_in=true);

  std::string uniqueName() const;
  std::string name() const; // same as unique name.

  ~AmpInitialiser(){
  }
};

#endif
//

#ifndef RELINFOPFVARIABLES_H 
#define RELINFOPFVARIABLES_H 1

// Include files
// from Gaudi
#include "GaudiKernel/ToolFactory.h"
#include "GaudiAlg/GaudiTool.h"
#include "Kernel/IRelatedInfoTool.h"            // Interface

class IDVAlgorithm;
typedef std::map <std::string,double> MapStringDouble;

/** @class RelinfoPFVariables RelinfoPFVariables.h
 *  
 *
 *  @author Xabier Cid Vidal
 *  @date   2014-06-30
 */
class RelInfoPFVariables : public GaudiTool, virtual public IRelatedInfoTool
{

public: 

  /// Standard constructor
  RelInfoPFVariables( const std::string& type, 
               const std::string& name,
               const IInterface* parent);
  
  virtual StatusCode initialize();

  /// Loop over PF particles and fill the variables
  virtual StatusCode calculateRelatedInfo( const LHCb::Particle*
                                           , const LHCb::Particle*);
  
  
  virtual LHCb::RelatedInfoMap* getInfo();  
  
  
  virtual ~RelInfoPFVariables( ); ///< Destructor

private:
  
  std::string m_PFLocation; // location of the PFParticles
  std::vector<std::string> m_variables;
  std::vector<short int> m_keys; 

  MapStringDouble m_properties; // includes all the output properties values
  
  LHCb::RelatedInfoMap m_map;

  double invariantMass(std::vector<Gaudi::LorentzVector> mypars);
  void emptyProperties();
  void fillMap(); // fill the output map

};
  
#endif // RELINFOPFVARIABLES_H

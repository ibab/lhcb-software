// $Id: JetProduction.h,v 1.2 2007-10-10 14:37:55 gcorti Exp $
#ifndef LBPYTHIA_JETPRODUCTION_H 
#define LBPYTHIA_JETPRODUCTION_H 1

// Include files
// from Gaudi
#include "GaudiAlg/GaudiTool.h"
#include "GaudiAlg/GaudiAlgorithm.h"
#include "Generators/IProductionTool.h"
#include "GaudiKernel/RndmGenerators.h"
#include "GaudiKernel/IRndmGenSvc.h"

// Forward declaration
class IBeamTool ;

/** @class JetProduction JetProduction.h 
 *  
 *  Tool to produce Jet events with Pythia.
 *  Description :
 *    This code is used to generate jets of particles, 
 *      using the Pythia generator.
 *    The functionning is very similar to the PythiaProduction algorithm.
 *    See the option file JetGuns.opts in the Gen/LbPythia package
 *      for more informations.
 * 
 *  @author Gueissaz Neal
 *  @date   2005-08-16
 *  @date   2007-10-09 (last modified)
 */

namespace GenMode {
  enum Mode { GaussMode=1, FlatMode };
};  

class JetProduction : public GaudiTool, virtual public IProductionTool {

  //class JetProduction : public PythiaProduction {


public:
  typedef std::vector<std::string> CommandVector ;
  
  /// Standard constructor
  JetProduction( const std::string & type , const std::string & name ,
                 const IInterface * parent ) ;
  
  virtual ~JetProduction( ); ///< Destructor
  
  virtual StatusCode initialize( ) ;   ///< Initialize method
  
  virtual StatusCode finalize( ) ;   ///< Finalize method
  
  virtual StatusCode generateEvent( HepMC::GenEvent * theEvent , 
                                    LHCb::GenCollision * theCollision ) ;
  
  virtual StatusCode initializeGenerator( ) ;
  
  virtual void setStable( const LHCb::ParticleProperty * thePP ) ;
  
  virtual void updateParticleProperties( const LHCb::ParticleProperty * thePP ) ;
  
  virtual void turnOnFragmentation( ) ;
  
  virtual void turnOffFragmentation( ) ;
  
  virtual StatusCode hadronize( HepMC::GenEvent * theEvent , 
                                LHCb::GenCollision * theCollision ) ;
  
  virtual void savePartonEvent( HepMC::GenEvent * theEvent ) ;
  
  virtual void retrievePartonEvent( HepMC::GenEvent * theEvent ) ;
  
  virtual void printRunningConditions( ) ;
  
  virtual bool isSpecialParticle( const LHCb::ParticleProperty * thePP ) const ;
  
  virtual StatusCode setupForcedFragmentation( const int thePdgId ) ;
  
protected:
  /// Parse Pythia commands from a string vector
  StatusCode parsePythiaCommands( const CommandVector & theVector ) ;
  
  /// Print Pythia parameters
  void printPythiaParameter( ) ;
  
  /// Retrieve hard process information
  void hardProcessInfo( LHCb::GenCollision * theCollision ) ;
  
  int m_userProcess ; ///< type of User process
  
protected:
  
  void setPygive ( const CommandVector& vct ) { m_pygive = vct ; }
  void addPygive ( const std::string&   item ) { m_pygive.push_back ( item ) ; }
  const CommandVector& pygive() const { return m_pygive ; }
  
  /// PYTHIA -> HEPEVT -> HEPMC conversion 
  StatusCode toHepMC
  ( HepMC::GenEvent*     theEvent    , 
    LHCb::GenCollision * theCollision ) ;
  
protected:
  
  CommandVector m_defaultSettings ;
	CommandVector m_commandVector ; ///< Commands to setup pythia
  
  CommandVector m_pygive        ; ///< Commands in "Pygive" format


  // event listing level for "generateEvent"
  int m_eventListingLevel  ;
  // event listing level for "hadronize"
  int m_eventListingLevel2 ;
  int m_initializationListingLevel ;
  int m_finalizationListingLevel ;
  
  std::string m_pythiaListingFileName ;
  int m_pythiaListingUnit ;
  
  int         m_particleDataUnit   ;
  std::string m_particleDataOutput ;
  std::string m_particleDataInput  ;
  int         m_particleDataLevel  ;

private:

  // MSTU(1)/MSTU(2) for initialization PYLIST
  int m_ini_mstu_1 ;
  int m_ini_mstu_2 ;
  // MSTU(1)/MSTU(2) for "generateEvent" PYLIST
  int m_eve_mstu_1 ;
  int m_eve_mstu_2 ;
  // MSTU(1)/MSTU(2) for "hadronize" PYLIST
  int m_had_mstu_1 ;
  int m_had_mstu_2 ;
  // list of particles to be printed 
  std::vector<int> m_pdtlist ;
  int m_nEvents ; ///< Internal event counter

  int m_njets;
  int m_index;
  
  int m_particlesmin; int m_particlesmax;
  int m_particlesmean; double m_particlessigma;
  std::vector<int> m_particles;
  int m_particlesgenmode; int m_particlesdiff;

  double m_energymin; double m_energymax;
  double m_energymean; double m_energysigma;
  std::vector<double> m_energy;
  int m_energygenmode; int m_energydiff;
  
  double m_thetamin;double m_thetamax;  
  double m_thetamean;  double m_thetasigma;
  std::vector<double> m_theta;
  int m_thetagenmode; int m_thetadiff;
  
  double m_phimin;double m_phimax;
  double m_phimean;  double m_phisigma;
  std::vector<double> m_phi;
  int m_phigenmode; int m_phidiff;
 
  double generateValue( const int mode, const double mean,
                        const double sigma, const double min,
                        const double max );
 
  //Define the method and the Flat random number generator
  mutable IRndmGenSvc* m_RGS;
  IRndmGenSvc* randSvc() const;
  Rndm::Numbers m_flatGenerator;
  Rndm::Numbers m_gaussGenerator;
  
};
#endif // LBPYTHIA_JETPRODUCTION_H

// $Id: PythiaProduction.h,v 1.11 2008-10-23 15:34:15 robbep Exp $
#ifndef LBPYTHIA_PYTHIAPRODUCTION_H 
#define LBPYTHIA_PYTHIAPRODUCTION_H 1
// ============================================================================
// Include files
// ============================================================================
// From STL
#include <fstream>
//
// from Gaudi
// ============================================================================
#include "GaudiAlg/GaudiTool.h"
#include "Generators/IProductionTool.h"
#include "Generators/ICounterLogFile.h"
// ============================================================================
// Forward declaration
class IBeamTool ;
namespace LHCb {
  class ParticleProperty ;
} ;

/** @class PythiaProduction PythiaProduction.h 
 *  
 *  Interface tool to produce events with Pythia
 * 
 *  @author Patrick Robbe
 *  @date   2005-08-16
 */
class PythiaProduction : public GaudiTool, virtual public IProductionTool {
public:
  typedef std::vector<std::string> CommandVector ;
  
  /// Standard constructor
  PythiaProduction( const std::string & type , const std::string & name ,
                    const IInterface * parent ) ;
  
  virtual ~PythiaProduction( ); ///< Destructor
  
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

private:
  ICounterLogFile * m_xmlLogTool ; ///< XML Log file to store cross-sections

protected:
  /// Parse Pythia commands from a string vector
  StatusCode parsePythiaCommands( const CommandVector & theVector ) ;
  
  /// Print Pythia parameters
  void printPythiaParameter( ) ;
  
  /// Retrieve hard process information
  void hardProcessInfo( LHCb::GenCollision * theCollision ) ;

  int m_userProcess ; ///< type of User process
  std::string m_frame   ;  ///< FRAME string
  std::string m_beam    ;  ///< BEAM string  
  std::string m_target  ;  ///< TARGET string

protected:
  
  void setPygive ( const CommandVector& vct ) { m_pygive = vct ; }
  void addPygive ( const std::string&   item ) { m_pygive.push_back ( item ) ; }
  const CommandVector& pygive() const { return m_pygive ; }
  
  /// PYTHIA -> HEPEVT -> HEPMC conversion 
  StatusCode toHepMC
  ( HepMC::GenEvent*     theEvent    , 
    LHCb::GenCollision * theCollision ) ;
  
protected:
  
  double m_win          ;  ///< WIN
  

  CommandVector m_defaultSettings ;
	CommandVector m_commandVector ; ///< Commands to setup pythia
  
  CommandVector m_pygive        ; ///< Commands in "Pygive" format

  bool m_variableEnergy ;

  // event listing level for "generateEvent"
  int m_eventListingLevel  ;
  // event listing level for "hadronize"
  int m_eventListingLevel2 ;
  int m_initializationListingLevel ;
  int m_finalizationListingLevel ;
  
  std::string m_beamToolName ;
  
  std::string m_pythiaListingFileName ;
  int m_pythiaListingUnit ;
  
  int         m_particleDataUnit   ;
  std::string m_particleDataOutput ;
  std::string m_particleDataInput  ;
  int         m_particleDataLevel  ;

  IBeamTool * m_beamTool ;
  
private:

  void writePythiaEntryHeader(std::ofstream &outdec, 
                              const LHCb::ParticleProperty * thePP ) ;
  
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

  double m_widthLimit ; ///< Limit to consider a particle with no lifetime
  
  /** Name of optional SLHA decay file (to be placed in Gen/DecFiles/dkfiles 
   *  directory)
   */
  std::string m_slhaDecayFile ; 
  std::vector< int > m_pdecaylist ;
  std::string m_slhaSpectrumFile ;
  // ==========================================================================
  /// boolean flag to force the valiadation of IO_HEPEVT 
  bool m_validate_HEPEVT ; // force the valiadation of IO_HEPEVT 
  // ==========================================================================
  /// the file to dump the HEPEVT incinsistencies 
  std::string   m_inconsistencies ; // the file to dump the HEPEVT incinsistencies 
  // ==========================================================================
  std::ostream* m_HEPEVT_errors ;
  // ==========================================================================

  std::vector<int> m_updatedParticles;

  std::vector< int > m_particlesToAdd ;
};
#endif // LBPYTHIA_PYTHIAPRODUCTION_H

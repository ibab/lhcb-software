#ifndef LBHERWIGPP_HERWIGPPPRODUCTION_H 
#define LBHERWIGPP_HERWIGPPPRODUCTION_H 1

// Gaudi
#include "GaudiAlg/GaudiTool.h"
#include "Generators/IProductionTool.h"

// forward declaration
class IBeamTool ;

/** @class HerwigppProduction HerwigppProduction.h 
 *  
 *  Interface tool to produce events with Herwig++
 * 
 *  @author Philip Ilten
 *  @date   2010-03-30
 */

class HerwigppProduction : public GaudiTool, virtual public IProductionTool
{
 public:
  typedef std::vector<std::string> CommandVector ;
  
  /// default constructor
  HerwigppProduction( const std::string & type , const std::string & name ,
                     const IInterface * parent ) ;
  
  /// default destructor
  virtual ~HerwigppProduction( );
  
  /// initialize method
  virtual StatusCode initialize( ) ;
  
  /// finalize method
  virtual StatusCode finalize( ) ; 
  
  /// initialize Herwig++ generator
  virtual StatusCode initializeGenerator( ) ;
  
  /// generate an event
  virtual StatusCode generateEvent( HepMC::GenEvent * theEvent , 
                                    LHCb::GenCollision * theCollision ) ;

  /// convert Herwig++ event pointer to HepMC format
  virtual StatusCode toHepMC ( HepMC::GenEvent * theEvent ) ;

  /// set a particle stable
  virtual void setStable( const ParticleProperty * thePP ) ;

  /// modify a particle
  virtual void updateParticleProperties( const ParticleProperty * thePP ) ;

  /// dummy methods
  virtual void turnOnFragmentation( ) ;

  virtual void turnOffFragmentation( ) ;

  virtual StatusCode hadronize( HepMC::GenEvent * theEvent , 
                                LHCb::GenCollision * theCollision ) ;
  
  virtual void savePartonEvent( HepMC::GenEvent * theEvent ) ;

  virtual void retrievePartonEvent( HepMC::GenEvent * theEvent ) ;

  virtual void printRunningConditions( ) ;
  
  /// debugging methods for HepMC checking
  StatusCode writeEvent(HepMC::GenEvent * theEvent);
  StatusCode printEvent(HepMC::GenEvent * theEvent);
  StatusCode readEvent(HepMC::GenEvent * theEvent);

  virtual bool isSpecialParticle( const ParticleProperty * thePP ) const ;

  virtual StatusCode setupForcedFragmentation( const int thePdgId ) ;
  
protected:

  /// convert an integer to a string
  virtual std::string int2string(int num) ;

  /// convert a double to a string
  virtual std::string double2string(double num) ;

  /// replace special characters in the command string
  virtual std::string replaceSpecialCharacters(std::string str) ;
  
private:

  CommandVector m_defaultSettings ; ///< Herwig++ default settings
  CommandVector m_commandVector ;   ///< Herwig++ user settings
  
  std::string m_beamToolName ; ///< name of beam tool
  
  int m_id1 , m_id2 ; ///< PDG id's for the beams 
  double m_cme ; ///< center of mass energy
  
  IBeamTool *m_beamTool ; ///< beam tool to use
  
  ThePEG::EGPtr m_herwigpp; ///< Herwig++ engine
  ThePEG::EventPtr m_event; ///<  generated event

  int m_nEvents ;
  
  Rndm::Numbers m_random ; ///< random number generator
  double m_seed ; ///< random number generator seed

} ;
#endif // LBHERWIGPP_HERWIGPPPRODUCTION_H

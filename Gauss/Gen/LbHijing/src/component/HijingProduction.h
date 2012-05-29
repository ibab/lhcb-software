// $Id: HijingProduction.h,v 1.2 2007-03-09 12:47:33 gcorti Exp $
#ifndef LBHIJING_HIJINGPRODUCTION_H 
#define LBHIJING_HIJINGPRODUCTION_H 1

// Include files
// from Gaudi
#include "GaudiAlg/GaudiTool.h"
#include "Generators/IProductionTool.h"

/** @class HijingProduction HijingProduction.h 
 *  
 *  Interface tool to produce events with Hijing
 * 
 *  @author Daniele Gregori
 *  @date   2006-02-25
 */
class HijingProduction : public GaudiTool, virtual public IProductionTool {
public:
  typedef std::vector< std::string > CommandVector ;
  
  /// Standard constructor
  HijingProduction( const std::string& type, 
                    const std::string& name,
                    const IInterface* parent);
  
  virtual ~HijingProduction( ); ///< Destructor
  
  virtual StatusCode initialize( ) ;   ///< Initialize method
  
  virtual StatusCode finalize( ) ;   ///< Finalize method
  virtual StatusCode generateEvent( HepMC::GenEvent * theEvent , 
                                    LHCb::GenCollision * theCollision ) ;
  
  virtual StatusCode initializeGenerator( ) ;

  virtual void setStable( const LHCb::ParticleProperty * thePP ) ;
  
  virtual void updateParticleProperties( const LHCb::ParticleProperty * thePP ) ;
  
  virtual bool isSpecialParticle( const LHCb::ParticleProperty * thePP ) const ;
  
  virtual void turnOnFragmentation( ) ;
  
  virtual void turnOffFragmentation( ) ;
  
  virtual StatusCode setupForcedFragmentation( const int thePdgId ) ;
  
  virtual StatusCode hadronize( HepMC::GenEvent * theEvent , 
                                LHCb::GenCollision * theCollision ) ;
  
  virtual void savePartonEvent( HepMC::GenEvent * theEvent ) ;
  
  virtual void retrievePartonEvent( HepMC::GenEvent * theEvent ) ;
  
  virtual void printRunningConditions( ) ;
  
protected:
  StatusCode parseHijingCommands( const CommandVector & theVector ) ;
  
private:
  int m_beam_to_use    ; // 1 = beam1, 2 = beam2
  double m_efrm        ; // colliding energy
  std::string m_frame  ; // frame type
  std::string m_proj   ; // projectile type
  std::string m_targ   ; // target type 
  int m_iap            ; // projectile mass number
  int m_izp            ; // projectile charge number
  int m_iat            ; // target mass number
  int m_izt            ; // target charge number
  double m_bmin        ; // minimum impact parameter
  double m_bmax        ; // maximum impact parameter
  
  CommandVector m_defaultSettings ;
  CommandVector m_commandVector ; ///< Commands to setup hijing
  
};
#endif // LBHIJING_HIJINGPRODUCTION_H

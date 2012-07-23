// $Id: GiGaHiggsParticles.cpp,v 1.1 2008-06-24 10:29:45 ibelyaev Exp $
// ============================================================================
// Include files 
// ============================================================================
// include files 
// ============================================================================
// STD & STL 
// ============================================================================
#include <algorithm>
// ============================================================================
// GaudiKernel
// ============================================================================
#include "GaudiKernel/PhysicalConstants.h"
#include "GaudiKernel/ToolFactory.h"
// LHCb 
#include "Kernel/IParticlePropertySvc.h"
#include "Kernel/ParticleProperty.h"
// ============================================================================
// GiGa 
// ============================================================================
#include "GiGa/GiGaPhysConstructorBase.h"
// ============================================================================
// Geant4 
// ============================================================================
#include "G4ParticleTable.hh"
// ============================================================================
// Local
// ============================================================================
#include "G4Higgses.h"
// ============================================================================
/** @class GiGaHiggsParticles 
 *  simple class to construct Higgs particles 
 *  (needed for Hidden Valley studies)
 *
 *  The major properties:
 *  
 *    - "Higgses" : list of higgs particles to be created.
 *                  the default valeu contains [ "H_10" , "H_20" , "H_30" ] 
 *
 *  It also prints (@ MSG::INFO) the properties of created higgses 
 *
 *  @author Vanya BELYAEV Ivan.Belyaev@nikhef.nl
 *  @date 2008-06-22
 */
class GiGaHiggsParticles : public GiGaPhysConstructorBase
{
  // ==========================================================================
  /// friend factory for instantiation 
  friend class ToolFactory<GiGaHiggsParticles>;
  // ==========================================================================
protected:
  // ==========================================================================
  /// constructor 
  GiGaHiggsParticles  
  ( const std::string& type   ,
    const std::string& name   ,
    const IInterface*  parent ) 
    : GiGaPhysConstructorBase ( type , name , parent )
    , m_higgses ()
  {
    m_higgses.push_back ( "H_10" ) ;
    m_higgses.push_back ( "H_20" ) ;
    m_higgses.push_back ( "H_30" ) ;
    declareProperty 
      ("Higgses" , m_higgses , 
       "The List of Higgsed to be instantiated") ;  
  }
  // virtual and protected destcructor 
  virtual ~GiGaHiggsParticles () {} ;
  // ==========================================================================
public:
  // ==========================================================================
  /// construct the particles 
  virtual void ConstructParticle ()    ; // construct the particles 
  /// construct the processed  (empty) 
  virtual void ConstructProcess  () {} ; // construct the processed 
  // ==========================================================================
protected:
  // ==========================================================================  
  /// get the particle property for the given particle name 
  const LHCb::ParticleProperty* pp 
  ( const std::string&    n , 
    LHCb::IParticlePropertySvc* s ) const 
  {
    Assert ( 0 != s , "Invalid ParticleProperty Service") ;
    const LHCb::ParticleProperty* p = s->find ( n ) ;
    Assert ( 0 != p , "No information is availale for '" + n + "'") ;
    return p ;
  }
  // ==========================================================================
private:
  // ==========================================================================
  /// The default constructor is disabled 
  GiGaHiggsParticles () ; // The default constructor is disabled
  /// The copy constructor is disabled 
  GiGaHiggsParticles ( const GiGaHiggsParticles& ) ;     // No copy constructor
  /// The assigenement operator is disabled
  GiGaHiggsParticles& operator=( const GiGaHiggsParticles& ) ; // No assignenemt
  // ==========================================================================
private:
  // ==========================================================================
  typedef std::vector<std::string> Strings ;
  /// list of Higgses to be instantiated
  Strings    m_higgses ; // list of Higgses to be instantiated
  // ==========================================================================
};
// ============================================================================
// construct the particles 
// ============================================================================
void GiGaHiggsParticles::ConstructParticle () // construct the particles 
{
  
  LHCb::IParticlePropertySvc* ppSvc = 
    svc<LHCb::IParticlePropertySvc> ("LHCb::ParticlePropertySvc") ;
  
  Strings tmp = m_higgses ;
  {
    // ========================================================================
    Strings::iterator it = std::find  ( tmp.begin() , tmp.end() , "H_10" ) ;
    if ( tmp.end() != it )
    {
      const LHCb::ParticleProperty* h10 = pp ( "H_10" , ppSvc ) ;
      G4H_10* h_10 = G4H_10::Create 
        ( h10 -> mass     () ,       
          h10 -> lifetime () * Gaudi::Units::c_light ) ;
      Assert ( 0 != h_10 , "Unable to create H_10" ) ;
      if ( msgLevel ( MSG::INFO )  ) { h_10->DumpTable () ; }  
      tmp.erase ( it ) ;  
    }
    // ========================================================================
    it = std::find  ( tmp.begin() , tmp.end() , "H_20" ) ;
    if ( tmp.end() != it )
    {
      const LHCb::ParticleProperty* h20 = pp ( "H_20" , ppSvc ) ;
      G4H_20* h_20 = G4H_20::Create 
        ( h20 -> mass     () ,       
          h20 -> lifetime () * Gaudi::Units::c_light ) ;
      Assert ( 0 != h_20 , "Unable to create H_20" ) ;
      if ( msgLevel ( MSG::INFO ) ) { h_20->DumpTable () ; }  
      tmp.erase ( it ) ;  
    }
    // ========================================================================
    it = std::find  ( tmp.begin() , tmp.end() , "H_30" ) ;
    if ( tmp.end() != it )
    {
      const LHCb::ParticleProperty* h30 = pp ( "H_30" , ppSvc ) ;
      G4H_30* h_30 = G4H_30::Create 
        ( h30 -> mass     () ,       
          h30 -> lifetime () * Gaudi::Units::c_light ) ;
      Assert ( 0 != h_30 , "Unable to create H_30" ) ;
      if ( msgLevel ( MSG::INFO ) ) { h_30->DumpTable () ; }  
      tmp.erase ( it ) ;  
    }
    // ========================================================================
    Assert ( tmp.empty() , "Unknown Higgses in the list!" ) ;
    // ========================================================================
  }
}
// ============================================================================
/// Declaration of the Tool Factory
DECLARE_TOOL_FACTORY ( GiGaHiggsParticles ) ;
// ============================================================================
// The END 
// ============================================================================


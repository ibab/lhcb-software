// $Id: GaussTrackActionByEnergyType.h,v 1.3 2007-01-12 15:36:43 ranjard Exp $
#ifndef GAUSSTOOLS_GAUSSTRACKACTIONBYTYPE_H 
#define GAUSSTOOLS_GAUSSTRACKACTIONBYTYPE_H 1

// local
#include "GaussTools/GaussTrackActionZ.h"

class G4ParticleDefinition ;

/** @class GaussTrackActionByEnergyType GaussTrackActionByEnergyType.h 
 *  
 *  Track action, valid for certain region in Z based on 
 *  particle/children types 
 *
 *  @author Vanya BELYAEV Ivan.Belyaev@itep.ru
 *  @date   2004-02-19
 */
class GaussTrackActionByEnergyType : public GaussTrackActionZ 
{
  /// friend factory for instantiation 
  //  friend class GiGaFactory<GaussTrackActionByEnergyType> ;
public:

  /** initialize the track action  
   *  @see GiGaTrackActionBase 
   *  @see GiGaBase 
   *  @see  AlgTool 
   *  @see IAlgTool 
   *  @return status code 
   */
  virtual StatusCode initialize () ;
  
  /** perform action 
   *  @see G4VUserTrackingAction
   *  @param pointer to new track opbject 
   */
  virtual void PreUserTrackingAction  ( const G4Track* ) ;
  
  /** perform action 
   *  @see G4VUserTrackingAction
   *  @param pointer to new track opbject 
   */
  virtual void PostUserTrackingAction ( const G4Track* ) ;
  
  //protected:
  
  /** standard constructor 
   *  @see GiGaTrackActionBase 
   *  @see GiGaBase 
   *  @see AlgTool 
   *  @param type type of the object (?)
   *  @param name name of the object
   *  @param parent  pointer to parent object
   */
  GaussTrackActionByEnergyType
  ( const std::string& type   ,
    const std::string& name   ,
    const IInterface*  parent ) ;
  
  /// destructor (virtual and protected)
  virtual ~GaussTrackActionByEnergyType();
  
private:
  
  // default constructor   is disabled 
  GaussTrackActionByEnergyType() ; 
  // copy    constructor   is disabled 
  GaussTrackActionByEnergyType           
  ( const GaussTrackActionByEnergyType& ) ; 
  // assignement operator  is disabled 
  GaussTrackActionByEnergyType& operator=
  ( const GaussTrackActionByEnergyType& ) ;
  
protected:
  
  typedef std::vector<std::string>                  Names  ;
  typedef std::vector<const G4ParticleDefinition*>  PartDefs; 
  
  double threshold  () const { return m_threshold ; }
  
  bool  storeOwn ( const G4Track* track ) const 
  {
    if ( 0 == track         ) { return false ; }
    if ( m_ownTypes.empty() ) { return false ; }
    return std::binary_search ( m_ownTypes.begin     () , 
                                m_ownTypes.end       () , 
                                track->GetDefinition () ) ;
  } ;
  
private:
  
  Names    m_ownNames    ;
  PartDefs m_ownTypes    ; // sorted 
  double   m_threshold   ;
  bool     m_rejectRICHphe  ;
  
};

// ============================================================================
// The END 
// ============================================================================
#endif // GAUSSTOOLS_GAUSSTRACKACTIONBYTYPE_H
// ============================================================================

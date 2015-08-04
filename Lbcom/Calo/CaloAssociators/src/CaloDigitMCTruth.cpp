// ============================================================================
// Include files 
// ============================================================================
// GaudiAlg
// ============================================================================
#include "GaudiAlg/GaudiAlgorithm.h"
// ============================================================================
// Event 
// ============================================================================
#include "Event/CaloDigit.h"
#include "Event/CaloCluster.h"
#include "Event/MCParticle.h"
// ============================================================================
//  CaloEvent 
// ============================================================================
#include "Event/CaloDataFunctor.h"
//CaloUtils
#include "CaloUtils/CaloMCTools.h"
#include "CaloUtils/Calo2MC.h"
// ============================================================================
// LHCbKernel
// ============================================================================
#include "Kernel/CaloCellIDKeyTraits.h"
// ============================================================================
// CaloDet
// ============================================================================
#include "CaloDet/DeCalorimeter.h"
// ============================================================================
// Linker 
// ============================================================================
#include "Linker/LinkerWithKey.h"
// ============================================================================
#include "Kernel/CaloCellIDKeyTraits.h"
//
// ============================================================================

/** @class CaloDigitMCTruth CaloDigitMCTruth.cpp
 *
 *  Simple algorithm to build Linkers for CaloDigit -> MCParticle relations 
 *
 *  Algorithm has following important properties 
 * 
 *   - "Input"     
 *      name of CaloDigit container, 
 *      the default value is CaloDigitLocation::Ecal 
 *
 *   - "Detector"  
 *      name of detector element, 
 *      the default value of DeCalorimeterLocation::Ecal 
 *
 *   - "MinDigitEnergy" and "MinDigitEt"
 *      Monte Carlo links will be set only for digits 
 *      with energy *OR* transverse energy over the 
 *      correspoding thresholds.
 *
 *   - "MinParticleEdeposit" and "MinParticleETdeposit"
 *      Monte Carlo link will be set only for 
 *      cumulative deposition of energy *OR* 
 *      transverse energy depositions over the 
 *      corresponding thresholds  
 *
 *   - "MinParticleFraction"
 *      Monte Carlo link will be set only if 
 *      the cumulative deposition of energy 
 *      is abothe a fraction over teh total 
 *      energy of digit
 *
 *   - "MinParticleEnergy" 
 *      Monte Carlo link wil lbe set only for 
 *      Monte Carlo particles above the threshold 
 *      on the energy   
 *      
 *  @see CaloDigit
 *  @see MCParticle
 *
 *  @author Vanya BELYAEV Ivan.Belyaev@lapp.in2p3.fr
 *  @date 2005-05-03 
 */

class CaloDigitMCTruth : public GaudiAlgorithm 
{
  // friend factory for instantiation 
  friend class AlgFactory<CaloDigitMCTruth> ;
public:
  /// algorithm execution 
  virtual StatusCode execute    () ;
protected:
  /** standard constructor 
   *  @param name algorithm instance name 
   *  @param pSvc pointer to service locator
   */
  CaloDigitMCTruth
  ( const std::string& name , 
    ISvcLocator*       pSvc ) 
    : GaudiAlgorithm ( name , pSvc ) 
    // Digit energy thresholds 
    , m_cellET (   1 * Gaudi::Units::MeV     ) 
    , m_cellE  (  50 * Gaudi::Units::MeV     ) 
    // particle energy contribution thresholds 
    , m_minET  (   1 * Gaudi::Units::MeV     ) 
    , m_minE   (  50 * Gaudi::Units::MeV     ) 
    , m_minFr  (   1 * Gaudi::Units::perCent ) 
    // particle minimum energy
    , m_minPE  (  50 * Gaudi::Units::MeV     ) 
    //
    , m_input     ( LHCb::CaloDigitLocation:: Ecal ) 
    , m_detector  ( DeCalorimeterLocation::   Ecal ) 
  {
    //
    declareProperty ( "MinDigitET"            , m_cellET ) ;
    declareProperty ( "MinDigitEnergy"        , m_cellE  ) ;
    //
    declareProperty ( "MinParticleETdeposit"  , m_minET  ) ;
    declareProperty ( "MinParticleEdeposit"   , m_minE   ) ;
    declareProperty ( "MinParticleFraction"   , m_minFr  ) ;
    //
    declareProperty ( "MinParticleEnergy"     , m_minPE  ) ;
    // 
    declareProperty ( "Input"     , m_input    ) ;
    declareProperty ( "Detector"  , m_detector ) ;
    

    StatusCode sc = setProperty     ( "StatPrint" , "true"     ) ;
    sc.isSuccess() ?
      debug() << "StatPrint set to 'true' " << endmsg :
      warning()<<"Setting StatPrint FAILED" << endmsg;

  };
  /// virtual destructor (protected)
  virtual ~CaloDigitMCTruth() {}
private:
  // default constructor  is disabled 
  CaloDigitMCTruth ();
  // copy    constructor  is disabled  
  CaloDigitMCTruth ( const CaloDigitMCTruth& ) ;
  // assignement operator is disabled  
  CaloDigitMCTruth& operator=( const CaloDigitMCTruth& ) ;
private:
  // only cells with ET over this threshold will be considered 
  double m_cellET  ;
  // only cells with  E over this threshold will be considered 
  double m_cellE   ;
  // minimal transverse energy deposition by particle  
  double m_minET   ;
  // minimal            energy deposition by particle  
  double m_minE    ;  
  // minimal relative   energy deposition by particle 
  double m_minFr   ;  
  // minimal particle energy 
  double m_minPE   ;  
  // input data
  std::string  m_input    ; ///< input data
  // detector data 
  std::string  m_detector ; ///< detector data
};
// ============================================================================

// ============================================================================
// declare algorithm factory
// ============================================================================
DECLARE_ALGORITHM_FACTORY(CaloDigitMCTruth)
// ============================================================================

// ============================================================================
/// algorithm execution 
// ============================================================================
StatusCode CaloDigitMCTruth::execute    () 
{
  using namespace LHCb::CaloDataFunctor ;
  using namespace CaloMCTools     ;
  
  /// calorimeter digit 
  typedef const LHCb::CaloDigit                      Digit     ;
  /// container of calorimeter digit 
  typedef const LHCb::CaloDigits                     Digits    ;
  /// detector element 
  typedef const DeCalorimeter                        Detector  ;
  /// simple predicate for find digits  with large energy
  typedef Over_E_Threshold<Digit*>                   OverE     ;
  /// simple predicate for find digits  with large transverse energy
  typedef Over_Et_Threshold<Digit*,Detector*>        OverET    ;
  /// helper class to build MC history tree 
  typedef MCCaloHistory<LHCb::CaloDigit>             MCHistory ;
  /// auxillary structure for conversion  "CaloCellID" -> "int"
  //  typedef Containers::key_traits<LHCb::CaloCellID>   Key       ;

  // get digits from TES  
  Digits*   digits   = getIfExists<Digits>     ( m_input ) ;
  if ( !digits ) return StatusCode::SUCCESS;
  
  // get the detector from TDS 
  Detector* detector = getDet<Detector>( m_detector ) ;
  
  // scale factor for recalculation of eActive into eTotal 
  const double activeToTotal = detector->activeToTotal() ;
  
  { // check the availability of the global MC truth 
    LHCb::MCCaloDigits* _mc = mcTruth<LHCb::MCCaloDigits>( digits ) ;
    if ( 0 == _mc ) { Error ( "MCCaloDigits* point to NULL" ).ignore() ; }
  }
  
  // create the Linker object 
  LHCb::Calo2MC::DigitLink linker ( eventSvc () , msgSvc () , m_input ) ;
  
  // evaluates to "true" for digits over E  threshold 
  OverE  overE  (            m_cellE  ) ;
  // evaluates to "true" for digits over Et threshold 
  OverET overET ( detector , m_cellET ) ;
  
  // counter for number of links
  unsigned long nLinks = 0 ;

  // loop over all digits 
  for ( Digits::const_iterator idigit = digits->begin() ; 
        digits->end() != idigit ; ++idigit ) 
  {
    Digit* digit = *idigit ;
    if ( 0 == digit ) { continue ; } // skip invalid

    
    // skip extra small energy depositions 
    if ( !overE  ( digit ) && !overET ( digit ) ) { continue ; }
    
    // coefficient to transform energy to transverse energy
    const double sinTheta = detector->cellSine( digit->cellID() ) ;
    
    CaloMCMap map1 ;
    
    MCHistory history( &map1 ) ;
    
    // build the history
    history( digit ).ignore();
    //StatusCode _sc = history( digit ) ;
    //debug() << "code is " << _sc.getCode() << endmsg;
    //if(_sc.isFailure())warning() << "history FAILED" << endmsg;

    // copy history map into the separate container 
    CaloMCMap map2( map1 ) ;
    
    { // update the copy of history map 
      for ( CaloMCMap::iterator entry = map1.begin() ; 
            map1.end() != entry ; ++entry ) 
      { 
        const LHCb::MCParticle* particle = entry -> first  ;
        const double            energy   = entry -> second ;
        StatusCode up = updateCaloMCMap ( particle , energy , map2 ) ; 
        if(up.isFailure())warning() <<"updateCaloMCMap FAILED " << endmsg;
      }
    }
    
    // threshold value 
    const double threshold = m_minFr * digit->e () ;
    
    // fill the relation table 
    for ( CaloMCMap::iterator entry = map2.begin() ; 
          map2.end() != entry ; ++entry ) 
    {
      const LHCb::MCParticle* particle = entry -> first ;
      if ( 0 == particle                       ) { continue ; }
      
      // use only more or less  "energetic" particles 
      if ( particle->momentum().e() < m_minPE  ) { continue ; }
      
      // cumulative energy deposition from this particle 
      const double      energy   = activeToTotal * entry -> second ;
      // skip very small energy depositions 
      if ( energy            < m_minE    &&
           energy * sinTheta < m_minET   &&  
           energy            < threshold       ) { continue  ;}
      
      // fill actual relations 
      linker.link ( digit , particle , energy ) ;
      

      // increment the counter 
      ++nLinks ;
      
    }; // end of loop over all MC-entries for given digit 
    
  }; // end of loop over all digits
  
  // count number of links 
  counter ("#Digit->MC links") += nLinks ;
  
  if ( 0 == nLinks ) { counter("Evts w/o links") += 1; }
  
  if ( msgLevel ( MSG::DEBUG ) ) 
  { debug() << "Number of MC-links #" << nLinks << endmsg ; }
  
  return StatusCode::SUCCESS ;
  
}
// ============================================================================

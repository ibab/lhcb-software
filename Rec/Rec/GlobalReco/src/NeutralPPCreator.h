// $Id: NeutralPPCreator.h,v 1.2 2002-07-27 19:24:05 gcorti Exp $
// ============================================================================
// CVS tag $Name: not supported by cvs2svn $
// ============================================================================
// $Log: not supported by cvs2svn $
// Revision 1.1  2002/07/15 19:27:09  ibelyaev
//  add new algorithm for creation on Neutral ProtoParticles
// 
// ============================================================================
#ifndef NEUTRALPPCREATOR_H 
#define NEUTRALPPCREATOR_H 1
// Include files
// from STL
#include <string>
#include <algorithm>
// LHCbKernel
#include "Relations/IAssociatorWeighted.h"
// from CaloInterfaces
#include "CaloInterfaces/ICaloHypoLikelyhood.h"
// from CaloEvent
#include "Event/CaloHypo.h"
// from CalUtils
#include "CaloUtils/ClusterFunctors.h"
// from CaloKernel
#include "CaloKernel/CaloAlgorithm.h"

/** @class NeutralPPCreator NeutralPPCreator.h
 *  
 *  The algorithm creates the container 
 *  of neutral protoparticles 
 *
 *  @author Vanya Belyaev Ivan.Belyaev@itep.ru
 *  @date   2002-07-15
 */
class NeutralPPCreator : public CaloAlgorithm 
{
  /// friend factory form instantiation
  friend class AlgFactory<NeutralPPCreator>;
  
private:

  /// additional inputs 
  typedef std::vector<std::string> Inputs   ;
  /// photon mathching associator type 
  typedef IAssociatorWeighted<CaloCluster,TrStoredTrack,float> Match    ;
  typedef ICaloHypoLikelyhood                                  SpdPrsID ;
  typedef const CaloHypo           Hypo     ;
  typedef const CaloHypos          Hypos    ;
  typedef const CaloHypo::Clusters Clusters ;
  typedef const Match::DirectType  Table    ;
  typedef const Table::Range       Range    ;
  typedef SmartRefVector<CaloHypo> Calos    ;
  
public:

  /** standard initialization method
   *  @see CaloAlgorithm 
   *  @see     Algorithm 
   *  @see    IAlgorithm
   *  @return ctatus code 
   */
  virtual StatusCode initialize () ; 
  
  /** standard execution  method
   *  @see CaloAlgorithm 
   *  @see     Algorithm 
   *  @see    IAlgorithm
   *  @return ctatus code 
   */
  virtual StatusCode execute    () ; 
  
protected:
  
  /** Standard constructor  
   *  @see CaloAlgorithm
   *  @see     Algorithm
   *  @param name the name of the algorithm instance
   *  @param svcLoc pointer to Service Locator 
   */
  NeutralPPCreator 
  ( const std::string& name   , 
    ISvcLocator*       svcLoc );
  
  /// destructor (protected and virtual)
  virtual ~NeutralPPCreator() ; 
  
  /** helpful function to determine the 'overlap' of 
   *  two CaloHypo object 
   *  @param hypo1 pointer to the first hypo
   *  @param hypo2 pointer to the second hypo
   *  @return true if objects are overlapped 
   */
  inline bool overlap
  ( const CaloHypo* hypo1 , 
    const CaloHypo* hypo2 ) const 
  {
    // trivial cases 
    if( hypo1 == hypo2           ) { return true  ; }
    if( 0 == hypo1 || 0 == hypo2 ) { return false ; }
    
    CaloHypo::Clusters::const_iterator cluster = 
      std::find_first_of ( hypo1 -> clusters().begin () , 
                           hypo1 -> clusters().end   () , 
                           hypo2 -> clusters().begin () , 
                           hypo2 -> clusters().end   () );
    // 
    return 
      ( hypo1->clusters().end() != cluster ) ? true : false ;
  };
  
  /** helpful function to determine the 'overlap' of 
   *  CaloHypo and PtotoParticle objects 
   *  @param hypo     pointer to the hypo
   *  @param particle pointer to the particle
   *  @return true if objects are overlapped 
   */
  inline bool overlap
  ( const CaloHypo*      hypo     , 
    const ProtoParticle* particle ) const 
  {
    // trivial cases 
    if( 0 == hypo && 0 == particle ) { return false ; }
    // loop over all hypos in Particle
    const Calos& calos = particle->calo();
    for( Calos::const_iterator calo  = calos.begin() ; 
         calos.end() != calo ; ++calo ) 
      {
        if( !(*calo) ) { continue ; }
        if( overlap( hypo , *calo ) ) { return true ; }
      }
    return false;
  };

  /** process of one calo hypo into the protoparticle
   *  @param hypo CloHypo object 
   *  @param particle Protopareticle to be updated 
   *  @param associator table 
   *  @return statsu code 
   */
  StatusCode process
  ( const CaloHypo* hypo     , 
    ProtoParticle*  particle , 
    const Table*    table    ) const ;
  
private:
  
  /// the default constructor is disabled  
  NeutralPPCreator();
  /// the copy    constructor is disabled  
  NeutralPPCreator( const NeutralPPCreator& );
  /// the assignement         is disabled  
  NeutralPPCreator& operator=( const NeutralPPCreator& );  

private:

  /// additional inputs 
  Inputs                           m_inputs ;  
  /// Associator type/name 
  std::string     m_matchName   ;
  // associator itself 
  Match*         m_match        ;
  

  // Spd/Prs photon ID tool type/name
  std::string     m_spdPrsName  ;
  SpdPrsID*       m_spdPrs      ;
  
  ClusterFunctors::ClusterFromCalo m_ecal ;
  
};

// ============================================================================
// The END 
// ============================================================================
#endif // NEUTRALPPCREATOR_H
// ============================================================================

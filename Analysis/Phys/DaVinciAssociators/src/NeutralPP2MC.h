// $Id: NeutralPP2MC.h,v 1.7 2004-07-27 16:50:11 pkoppenb Exp $
// ============================================================================
// CVS tag $Name: not supported by cvs2svn $
// ============================================================================
// $Log: not supported by cvs2svn $
// Revision 1.6  2004/06/11 15:26:17  phicharp
// v5r0 - Using Linker tables, see release notes
//
// Revision 1.5  2004/03/11 10:30:26  pkoppenb
// upstream -> downstream
//
// Revision 1.4  2003/07/29 17:48:12  gcorti
// little changes for gcc32
//
// Revision 1.3  2003/04/17 09:58:26  phicharp
// Allow Links associator to use Particles not in the TES
//
// Revision 1.2  2002/10/02 07:06:28  phicharp
// New associators, include multiple associations etc..
//
// Revision 1.1  2002/09/12 12:16:12  gcorti
// Neutral PP2MC algorithm
//
//  rename when transferign to DaVinciAssociators
// 
// Revision 1.2  2002/09/07 11:01:49  ibelyaev
//  fix the bugs, kindly found by Gloria and Galina
//
// Revision 1.1  2002/09/04 15:16:14  ibelyaev
//  add new associator and it's algorithm
//
// Revision 1.1  2002/06/26 19:03:56  ibelyaev
//  add one more new algorithm for MCTruth
//
// Revision 1.2  2002/06/13 12:28:48  ibelyaev
//  new options and new algorithm
//
// ============================================================================
#ifndef CALOASSOCIATORS_NeutralPP2MC_H 
#define CALOASSOCIATORS_NeutralPP2MC_H 1
// Include files
// from STL
#include <string>
// from Gaudi

// from DaVinci Associators
#include "AsctAlgorithm.h"
#include "DaVinciAssociators/ProtoParticle2MCAsct.h"
#include "DaVinciAssociators/Particle2MCLink.h"

/** @class NeutralPP2MC NeutralPP2MC.h
 *  
 *  The algorithm for building the relations
 *  between CaloClusters and MCParticles
 *
 *  This implementation is "primitive" - it relate
 *  CaloCluster object with MCParticle, which HAS a deposition 
 *  of active energy with exceed of some threshold.
 *  the deposited energy is the "weight" of the relation
 * 
 *  @see CaloCluster
 *  @see MCParticle
 *  @see Relation2D 
 *
 *  @author Vanya Belyaev Ivan.Belyaev@itep.ru
 *  @date   08/04/2002
 */
class NeutralPP2MC
  : public AsctAlgorithm
{
  // friend factory for instantiation 
  friend class AlgFactory<NeutralPP2MC>;

public:

  /** standard initialization of the algorithm
   *  @see CaloAlgorithm 
   *  @see     Algorithm 
   *  @see    IAlgorithm 
   *  @return StatusCode
   */
  virtual StatusCode initialize () ;

  /** standard execution of the algorithm
   *  @see CaloAlgorithm 
   *  @see     Algorithm 
   *  @see    IAlgorithm 
   *  @return StatusCode
   */
  virtual StatusCode execute    () ;

  /** standard finalization of the algorithm
   *  @see CaloAlgorithm 
   *  @see     Algorithm 
   *  @see    IAlgorithm 
   *  @return StatusCode
   */
  virtual StatusCode finalize   () ;
  
protected:
  
  /** Standard constructor
   *  @param name name of the algorithm
   *  @param svc  service locator
   */
  NeutralPP2MC
  ( const std::string& name , 
    ISvcLocator*       svc  );
  
  /// destructor (virtual and protected)
  virtual ~NeutralPP2MC();
  /// get output table name
  
private:
  
  /// default  constructor is private
  NeutralPP2MC();
  /// copy     constructor is private
  NeutralPP2MC
  ( const NeutralPP2MC& );
  /// assignement operator is private
  NeutralPP2MC& operator=
  ( const NeutralPP2MC& );
  
private:
  
  typedef IAssociatorWeighted<CaloCluster,MCParticle,float> MCAsct;
  
  /// type of the underlying  MC associator 
  std::string m_asctType ;
  /// name of the underlying  MC associator 
  std::string m_asctName ;
  /// MC associator itself  
  MCAsct*     m_asct     ;
};

// ============================================================================
// The END 
// ============================================================================
#endif // NeutralPP2MC_H
// ============================================================================

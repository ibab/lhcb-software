// $Id: ClusChi22ID.cpp,v 1.5 2007-08-24 21:25:18 odescham Exp $
// ============================================================================
// CVS tag $Name: not supported by cvs2svn $, version $Revision: 1.5 $
// ============================================================================
// $Log: not supported by cvs2svn $
// Revision 1.4  2006/06/22 15:40:40  ibelyaev
//  fix incinsistencied for 'Brem'-algorithms
//
// Revision 1.2  2006/06/20 18:17:48  odescham
// minor update to please ChargedProtoPAlg
//
// Revision 1.1  2006/06/18 18:35:28  ibelyaev
//  the firstcommmit for DC06 branch
// 
// ============================================================================
// Include files 
// ============================================================================
// Boost
// ============================================================================
#include "boost/static_assert.hpp"
// ============================================================================
// Local 
// ============================================================================
#include "CaloChi22ID.h"
#include "ToVector.h"
// ============================================================================
class ClusChi22ID: public CaloChi22ID 
{
  /// friend factory for instantiation 
  friend class AlgFactory<ClusChi22ID>;
public:
  /// algorithm execution 
  virtual StatusCode execute() ;
protected:
  /// Standard protected constructor
  ClusChi22ID 
  ( const std::string& name , 
    ISvcLocator*       pSvc ) 
    :  CaloChi22ID ( name , pSvc ) 
  {
    _setProperty ( "Input"  , LHCb::CaloIdLocation::ClusterMatch ) ;
    _setProperty ( "Output" , LHCb::CaloIdLocation::ClusChi2     ) ;
    // @todo it must be in agrement with "Threshold" for PhotonMatchAlg 
    _setProperty ( "CutOff" , "1000"  ) ; //  
    // track types:
    _setProperty ( "AcceptedType" , Gaudi::Utils::toString<int>
                  ( LHCb::Track::Long       ,
                    LHCb::Track::Ttrack     ,
                    LHCb::Track::Downstream ) ) ;
  } ;
  /// protected vertual destructor
  virtual ~ClusChi22ID(){}; 
private:
  ClusChi22ID () ;
  ClusChi22ID           ( const ClusChi22ID& ) ;
  ClusChi22ID& operator=( const ClusChi22ID& ) ;
} ;
// ============================================================================
#define INHERITS(T1,T2) \
     (Relations::IsConvertible<const T1*,const T2*>::value && \
     !Relations::IsConvertible<const T1*,const void*>::same)
// ============================================================================
//// declare algorithm factory
// ============================================================================
DECLARE_ALGORITHM_FACTORY ( ClusChi22ID ) ;
// ============================================================================
StatusCode ClusChi22ID::execute() 
{
  typedef IRelationWeighted2D<LHCb::CaloCluster,LHCb::Track,float> TABLEI ;
  typedef LHCb::Relation1D<LHCb::Track,float> Table  ;
  // check
  BOOST_STATIC_ASSERT(INHERITS(TABLEI,LHCb::Calo2Track::IClusTrTable2D));
  
  // get the input table 
  const TABLEI* input = get<TABLEI>( m_input ) ;
  // create and register the output table 
  Table* output = new Table ( input->relations().size() + 10 ) ;
  put ( output , m_output ) ;
  // perform the actual jobs 
  return doTheJob( input->inverse() , output ) ;
} ;
// =============================================================================


// ============================================================================
// The END 
// ============================================================================








// $Id: TupleToolCaloHypo.cpp,v 1.1 2010-05-21 17:03:11 odescham Exp $
// Include files
// from Gaudi
#include "GaudiKernel/ToolFactory.h"
// local
#include "TupleToolCaloHypo.h"
#include "GaudiAlg/Tuple.h"
#include "GaudiAlg/TupleObj.h"
#include "Event/Particle.h"
#include "GaudiKernel/IRegistry.h" //

//-----------------------------------------------------------------------------
// Implementation file for class : TupleToolCaloHypo
//
// 2008-10-31 :(Happy Halloween)
// Yasmine Amhis
//-----------------------------------------------------------------------------

using namespace Gaudi;
using namespace LHCb;

// Declaration of the Tool Factory
DECLARE_TOOL_FACTORY( TupleToolCaloHypo )

//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
  TupleToolCaloHypo::TupleToolCaloHypo( const std::string& type,
                                        const std::string& name,
                                        const IInterface* parent )
    : TupleToolBase ( type, name , parent ){
    declareInterface<IParticleTupleTool>(this);
    declareProperty("DataList",m_dataList);
    declareProperty("AllowBremHypo",m_brem = false);
    declareProperty("AllowChargedHypo",m_charged=false);
    m_dataList.push_back( "All" );
  }



StatusCode TupleToolCaloHypo::initialize() {
  if( ! TupleToolBase::initialize() ) return StatusCode::FAILURE;
  m_estimator = tool<ICaloHypoEstimator>("CaloHypoEstimator","CaloHypoEstimator",this);

  info() << "Caution : this TupleTool may trigger part of Calo reconstruction - ONLY WORK ON DST" << endmsg;

  return StatusCode::SUCCESS;
}

//=============================================================================
StatusCode TupleToolCaloHypo::fill(const Particle* , const Particle* P
                                   ,const std::string& head
                                   ,Tuples::Tuple& tuple ){

  const std::string prefix=fullName(head);

  if( NULL == P )return StatusCode::SUCCESS;
  if( NULL == P->proto() )return StatusCode::SUCCESS;
  if( 0 != P->charge() && !m_charged)return StatusCode::SUCCESS;

  bool filltuple = true;
  const SmartRefVector<LHCb::CaloHypo>& hypos = P->proto()->calo();
  bool hasCalo = true;
  bool hasBremCalo = false;
  if( 0 == hypos.size() )hasCalo = false;
  if( 1 < hypos.size() )hasBremCalo = true;
  filltuple &= tuple->column( prefix+"_hasCaloHypo", (int) hasCalo );
  if( !isPureNeutralCalo( P ) && m_brem )
    filltuple &= tuple->column( prefix+"_hasBremCaloHypo", (int) hasBremCalo );



  using namespace CaloDataType;
  // select hypos
  const LHCb::CaloHypo* hypo = NULL;
  const LHCb::CaloHypo* hypo2 = NULL;
  for( SmartRefVector<CaloHypo>::const_iterator ih = hypos.begin() ; hypos.end() != ih ; ++ih){
    if( !isPureNeutralCalo( P ) ){
      if( (*ih)->hypothesis() == LHCb::CaloHypo::EmCharged)hypo = *ih;
      else hypo2 = *ih;
    }else hypo = *ih;
  }
  //fill tuple
  for( int id = 0 ; id < Last ; ++id){
    int mask = ( P->charge() == 0 ) ? 0x1 : 0x2;
    if( useData( id , mask ) ){
      double val  =  (NULL != hypo && hasCalo)    ? m_estimator->data(hypo, (DataType) id ,0.) : 0.;
      filltuple &= tuple->column( prefix+"_CaloHypo_"+Name[id], val );
    }
    if( P->charge() != 0 && m_brem && useData( id , 0x1 ) ){
      double bval =  (NULL != hypo2 && hasBremCalo) ? m_estimator->data(hypo2,(DataType) id ,0.) : 0.;
      filltuple &= tuple->column( prefix+"_BremCaloHypo_"+Name[id], bval );
    }
  }
  return StatusCode(filltuple);
}

bool TupleToolCaloHypo::useData( int id , int mask){
  using namespace CaloDataType;
  std::string name = Name[id];
  int type = TypeMask[id];
  for(std::vector<std::string>::iterator i = m_dataList.begin() ; m_dataList.end() !=i ; ++i){
    if( *i == "All" && (type & mask) !=0 )return true;
    if( *i == name )return true;
  }
  return false;
}




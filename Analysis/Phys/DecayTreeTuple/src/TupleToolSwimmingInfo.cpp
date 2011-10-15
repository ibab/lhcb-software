// $Id: TupleToolSwimmingInfo.cpp,v 1.6 2010-03-04 14:02:03 xieyu Exp $
// Include files

// from Gaudi
#include "GaudiKernel/ToolFactory.h"

// local
#include "TupleToolSwimmingInfo.h"

#include "GaudiAlg/Tuple.h"
#include "GaudiAlg/TupleObj.h"

#include "Event/Particle.h"
//-----------------------------------------------------------------------------
// Implementation file for class : TupleToolSwimmingInfo
//
// 2011-10-15 : V Gligorov & R Aaij
//-----------------------------------------------------------------------------

// Declaration of the Tool Factory
// actually acts as a using namespace TupleTool
DECLARE_TOOL_FACTORY( TupleToolSwimmingInfo );

using namespace LHCb;
//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
TupleToolSwimmingInfo::TupleToolSwimmingInfo( const std::string& type,
    const std::string& name,
    const IInterface* parent )
: TupleToolBase ( type, name , parent )
{
  declareInterface<IParticleTupleTool>(this);
  declareProperty("ReportsLocation" , m_swimRelTableLoc  = "/Event/SwimmingMicroDST/SingleCandidate/P2TPRelations");
  declareProperty("ReportStage"     , m_swimRepsStage    = "Trigger");
}

//=============================================================================

StatusCode TupleToolSwimmingInfo::fill( const Particle*
    , const Particle* P
    , const std::string& head
    , Tuples::Tuple& tuple )
{
  const std::string prefix=fullName(head);
  const unsigned int maxTurns = 100 ;

  std::vector<double> raw, dec, tau,ip; 
  typedef std::map<std::string, std::vector<double> > MapType;
  MapType line_decisions;  

  P2TPRelation* relatePart;
  if (exist<P2TPRelation>(m_swimRelTableLoc) ) {
    relatePart = get<P2TPRelation>(m_swimRelTableLoc);
  } else return StatusCode::FAILURE;

  if( P ){
    bool test = true;
    if( !P->isBasicParticle() ) {
      P2TPRelation::Range range = relatePart->relations(P);
      if (range.size() == 0 or range.size() > 1) return StatusCode::FAILURE;
      LHCb::SwimmingReport* thisPartReport = range.begin()->to();

      tPoints turns  = thisPartReport->turningPoints(m_swimRepsStage); 
      for (tPoints::const_iterator itPoints = turns.begin(); itPoints != turns.end(); ++itPoints) {
        raw.push_back((*itPoints).raw());
        dec.push_back((*itPoints).dec());
        tau.push_back((*itPoints).tau());
        ip.push_back( (*itPoints).ip( ));
        //Decisions must not change between turning points
        std::vector<std::string> line_decisions_at_turning_point = (*itPoints).decisions();
        //if (itPoints == turns.begin()) {
        //  for ( std::vector<std::string>::const_iterator linename = line_decisions_at_turning_point.begin();
        //        linename != line_decisions_at_turning_point.end(); ++linename) {
            //line_decisions[*linename] = ();
        //    line_decisions[*linename].push_back((*itPoints).decision(*linename));
        //  } 
        //} else {
          for ( std::vector<std::string>::const_iterator linename = line_decisions_at_turning_point.begin();
                linename != line_decisions_at_turning_point.end(); ++linename) {
            line_decisions[*linename].push_back((*itPoints).decision(*linename));
          } 
        //}
      }
      test &= tuple->farray( prefix+"_TP_RAW", raw ,prefix+"_nTP",maxTurns );
      test &= tuple->farray( prefix+"_TP_DEC", dec ,prefix+"_nTP",maxTurns );
      test &= tuple->farray( prefix+"_TP_TAU", tau ,prefix+"_nTP",maxTurns );
      test &= tuple->farray( prefix+"_TP_IP" , ip  ,prefix+"_nTP",maxTurns );
      for (MapType::const_iterator it = line_decisions.begin(); it != line_decisions.end(); ++it) { 
        test &= tuple->farray( prefix+"_TP_DEC_"+(it->first), line_decisions[it->first], prefix+"_nTP",maxTurns );
      } 
      
    }

  }
  return StatusCode::FAILURE;
}

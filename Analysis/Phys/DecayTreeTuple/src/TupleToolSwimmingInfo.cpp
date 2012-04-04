// $Id: TupleToolSwimmingInfo.cpp,v 1.6 2010-03-04 14:02:03 xieyu Exp $
// Include files

// boost
#include <boost/foreach.hpp>
#include <boost/algorithm/string/replace.hpp>

// from Gaudi
#include "GaudiKernel/ToolFactory.h"

// local
#include "TupleToolSwimmingInfo.h"

#include "GaudiAlg/Tuple.h"
#include "GaudiAlg/TupleObj.h"

#include "Event/Particle.h"

using namespace LHCb;

//-----------------------------------------------------------------------------
// Implementation file for class : TupleToolSwimmingInfo
//
// 2011-10-15 : V Gligorov & R Aaij
//-----------------------------------------------------------------------------

// Declaration of the Tool Factory
// actually acts as a using namespace TupleTool
DECLARE_TOOL_FACTORY( TupleToolSwimmingInfo )

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
  declareProperty("UseExtraLocation", m_useExtraLoc = false);
  declareProperty("ExtraLocation", m_extraLoc = "");
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

  std::vector<double> raw, dec, tau, ip;
  typedef std::map<std::string, std::vector<double> > MapType;

  P2TPRelation* relatePart;
  if (exist<P2TPRelation>(m_swimRelTableLoc) ) {
    relatePart = get<P2TPRelation>(m_swimRelTableLoc);
    debug() << "Found Particle2TurningPointsRelation at " << m_swimRelTableLoc << endmsg;
  } else {
    warning() << "Could not get Particle2TurningPointsRelation at " << m_swimRelTableLoc << endmsg;
    return StatusCode::FAILURE;
  }

  if( P ){
    const Particle* thisP = P;
    if (m_useExtraLoc) {
      const Particles* extraPs = get<Particles>(m_extraLoc); 
      if (!extraPs) {
        warning() << "Could not get extra particles!" << endmsg;
        return StatusCode::FAILURE;
      }
      const Particle* extraP = *(extraPs->begin());
      if (extraP) {
        thisP = extraP; 
      } else {
        warning() << "Could not get extra particles!" << endmsg;
        return StatusCode::FAILURE;
      }
    
    }
    bool test = true;

    P2TPRelation::Range range = relatePart->relations(thisP);
    if (range.size() != 1) return StatusCode::FAILURE;

    LHCb::SwimmingReport* report = range.begin()->to();

    if( !thisP->isBasicParticle() ) {
      // Loop over turning points to fill decision names and insert vectors
      MapType line_decisions;
      const tPoints& turns = report->turningPoints(m_swimRepsStage);
      BOOST_FOREACH(const LHCb::TurningPoint& tp, turns) {
        BOOST_FOREACH(const std::string& name, tp.decisions()) {
          line_decisions.insert(make_pair(name, std::vector<double>()));
        }
      }
      // Loop over turning points to fill vectors
      BOOST_FOREACH(const LHCb::TurningPoint& tp, turns) {
        // Fill variables
        raw.push_back(tp.raw());
        dec.push_back(tp.dec());
        tau.push_back(tp.tau());
        ip.push_back (tp.ip() );
        //Decisions must not change between turning points
        BOOST_FOREACH(MapType::value_type& entry, line_decisions) {
          if (!tp.dec()) {
            entry.second.push_back(false);
          } else {
            entry.second.push_back(tp.decision(entry.first));
          }
        }
      }
      // Fill tuple from vectors
      test &= tuple->farray( prefix+"_"+m_swimRepsStage+"_TP_RAW", raw ,prefix+"_"+m_swimRepsStage+"_nTP",maxTurns );
      test &= tuple->farray( prefix+"_"+m_swimRepsStage+"_TP_DEC", dec ,prefix+"_"+m_swimRepsStage+"_nTP",maxTurns );
      test &= tuple->farray( prefix+"_"+m_swimRepsStage+"_TP_TAU", tau ,prefix+"_"+m_swimRepsStage+"_nTP",maxTurns );
      test &= tuple->farray( prefix+"_"+m_swimRepsStage+"_TP_IP" , ip  ,prefix+"_"+m_swimRepsStage+"_nTP",maxTurns );
      BOOST_FOREACH(const MapType::value_type& entry, line_decisions) {
        std::string branch = entry.first;
        boost::algorithm::replace_all(branch, "/", "_");
        test &= tuple->farray(prefix + "_TP_DEC_" + branch, entry.second,
                              prefix + "_nTP", maxTurns);
      }
    } else if (m_swimRepsStage == "Trigger") { //The each track part is only for the trigger
      // Loop over turning points to fill info names and insert vectors
      MapType track_infos;
      const tPoints& turns = report->turningPoints(m_swimRepsStage);
      BOOST_FOREACH(const LHCb::TurningPoint& tp, turns) {
        BOOST_FOREACH(const std::string& name, tp.infoNames()) {
          track_infos.insert(make_pair(name, std::vector<double>()));
        }
      }
      BOOST_FOREACH(const LHCb::TurningPoint& tp, turns) {
        // Fill infos
        BOOST_FOREACH(MapType::value_type& entry, track_infos) {
          if (!tp.dec()) {
            entry.second.push_back(false);
          } else {
            entry.second.push_back(tp.participated(entry.first, *thisP));
          }
        }
      }
      // Fill tuple with infos
      BOOST_FOREACH(const MapType::value_type& entry, track_infos) {
        test &= tuple->farray(prefix + "_"+m_swimRepsStage+"_TP_DEC_" + entry.first, entry.second,
                              prefix + "_"+m_swimRepsStage+"_nTP", maxTurns);
      }
    }
  }
  return StatusCode::SUCCESS;
}

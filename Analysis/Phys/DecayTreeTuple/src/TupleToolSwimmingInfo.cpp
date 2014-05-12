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

class SortDaughters
{
  public:
    SortDaughters() { }
    bool operator()(const LHCb::Particle* c1, const LHCb::Particle* c2)
    {
      int p1 = c1->particleID().pid();
      int p2 = c2->particleID().pid();
      if(p1 == p2)
      {
        p1 = c1->key();
        p2 = c2->key();
      }
      return p1 > p2;
    }
};


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
  const unsigned int maxTurns = 1000;

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

  // We need to find some reports. Any reports. Just so we can get a list of names, so we always consistently
  // set some value for all of the branches.
  // It's not (/no longer) acceptable to just return some error code without tidying up when we find a candidate
  // which wasn't swum
  P2TPRelation::Range allreports(relatePart->relations());
  if(allreports.size() == 0)
    return Error("P2TPRelations was empty");

  typedef std::set<std::string> stringSet;
  stringSet decisions, infonames;
  for(P2TPRelation::Range::const_iterator report_iter = allreports.begin(); report_iter != allreports.end(); report_iter++)
  {
    const SwimmingReport *report(report_iter->to());
    const tPoints& turns = report->turningPoints(m_swimRepsStage);
    BOOST_FOREACH(const LHCb::TurningPoint& tp, turns)
    {               
      BOOST_FOREACH(const std::string& name, tp.decisions())
      {
        decisions.insert(name);
      }
      BOOST_FOREACH(const std::string& name, tp.infoNames())
      {
        infonames.insert(name);
      }
    }
  }

  // Now we should have the information to know what all of our branches are called
  debug() << "Found decisions: ";
  for(stringSet::const_iterator dec_iter = decisions.begin(); dec_iter != decisions.end(); dec_iter++)
    debug() << *dec_iter << " ";
  debug() << endmsg;
  debug() << "Found infonames: ";
  for(stringSet::const_iterator inf_iter = infonames.begin(); inf_iter != infonames.end(); inf_iter++)
    debug() << *inf_iter << " ";
  debug() << endmsg;

  if( P )
  {
    P2TPRelation::Range range;
    const Particle *thisP = P;

    if (m_useExtraLoc)
    {
      const Particles* extraPs = get<Particles>(m_extraLoc); 
      if (!extraPs)
        return Error("Could not get extra particles!");

      // figure out which of the extra particles we have P2TPrelations for
      for(Particles::const_iterator ep_iter = extraPs->begin(); ep_iter != extraPs->end(); ep_iter++)
      {
        const Particle *extraP(*ep_iter);
        if(extraP)
        {
          range = relatePart->relations(extraP);
          if(range.size() == 1)
          {
            debug() << "Found the right extraParticle" << endmsg;
            thisP = extraP;
            break;
          }
          else
          {
            debug() << "Trying next extraparticle... (relatePart->relations(extraP).size() was " << relatePart->relations(extraP).size() << ")" << endmsg;
          }
        }
        else
        {
          return Error("Found a NULL particle in ExtraLocation");
        }
      }

      if(!thisP)
        return Error("None of the particles in ExtraLocation had associated turning points");
      // Now thisP is the particle (e.g. D0) which was swum
      // However it is not neccessarily the same pointer as P because
      // they might be flavour tagged differently
      // (e.g. combined with different soft pions to make D*+ candidates with different charged)
      //
      // We need to figure out if P and thisP are built from the same daughters or not
      SmartRefVector<LHCb::Particle>
        daughters_P(P->daughters()), // local copies to sort
        daughters_thisP(thisP->daughters());

      bool matchedReports = false;
      if(daughters_P.size() == daughters_thisP.size()) // probably not needed...
      {
        std::stable_sort(daughters_P.begin(), daughters_P.end(), SortDaughters());
        std::stable_sort(daughters_thisP.begin(), daughters_thisP.end(), SortDaughters());
        if(daughters_P == daughters_thisP) // member-wise comparison
          matchedReports = true;
      }

      if(matchedReports && thisP != P)
        debug() << "Decided " << *thisP << " and " << *P << " match even though " << thisP << " != " << P << endmsg;

      // if we have no match, then we can't sensibly fill swimming information and the 'range' variable just below will
      // be empty
      if(!matchedReports)
        thisP = P; // don't leave thisP pointing at some random particle from the extra location
      // if we have a match, we need to look up the turning points using thisP (in case it's not identical to P)
    }

    range = relatePart->relations(thisP);

    // range.size() will be zero if we failed to find any associated TPs -- in this case we don't fill anything and nTP=0
    // if matchedReports was true then range.size() > 0 and we will find the correct reports

    if(range.size() > 1)
      debug() << "Got range.size() > 1, not sure what to make of that" << endmsg;

    bool test(true);
    if( !P->isBasicParticle() )
    {
      // Loop over turning points to fill decision names and insert vectors
      MapType line_decisions;

      // This is just setting up some structures rather than actually filling turning point values
      // Even if we didn't swim this event, we still need to fill in the right branches in the tree
      // This is why 'decisions' was populated earlier...
      for(stringSet::const_iterator dec_iter = decisions.begin(); dec_iter != decisions.end(); dec_iter++)
        line_decisions.insert(make_pair(*dec_iter, std::vector<double>()));
     
      if(range.size() == 1)
      {
        // After all that, we do have swimming reports to associate with this event
        LHCb::SwimmingReport* report = range.begin()->to();
        if(!report)
          return Error("Got NULL LHCb::SwimmingReport");
        const tPoints& turns = report->turningPoints(m_swimRepsStage);
        // Loop over turning points to fill vectors
        BOOST_FOREACH(const LHCb::TurningPoint& tp, turns)
        {
          // Fill variables
          raw.push_back(tp.raw());
          dec.push_back(tp.dec());
          tau.push_back(tp.tau());
          ip.push_back (tp.ip() );
          // Decisions must not change between turning points
          BOOST_FOREACH(MapType::value_type& entry, line_decisions)
          {
            if (!tp.dec())
            {
             entry.second.push_back(false);
            }
            else
            {
              entry.second.push_back(tp.decision(entry.first));
            }
          }
        }
      }

      if(raw.size() > maxTurns)
      {
        warning() << "Have " << raw.size() << " turning points, which is > maxTurns = " << maxTurns << endmsg;
      }

      // Fill tuple from vectors
      test &= tuple->farray( prefix+"_"+m_swimRepsStage+"_TP_RAW", raw ,prefix+"_"+m_swimRepsStage+"_nTP",maxTurns );
      test &= tuple->farray( prefix+"_"+m_swimRepsStage+"_TP_DEC", dec ,prefix+"_"+m_swimRepsStage+"_nTP",maxTurns );
      test &= tuple->farray( prefix+"_"+m_swimRepsStage+"_TP_TAU", tau ,prefix+"_"+m_swimRepsStage+"_nTP",maxTurns );
      test &= tuple->farray( prefix+"_"+m_swimRepsStage+"_TP_IP" , ip  ,prefix+"_"+m_swimRepsStage+"_nTP",maxTurns );
      
      // If there is only one decision then the next part would just be duplicating ..._TP_DEC above.
      // This also makes D0->KSHH tuples have the same branches for D0 and D0bar, which is nice when you want to hadd them...
      if(line_decisions.size() > 1)
      {
        BOOST_FOREACH(const MapType::value_type& entry, line_decisions)
        {
          std::string branch(entry.first);
          boost::algorithm::replace_all(branch, "/", "_");
          test &= tuple->farray(prefix + "_" + m_swimRepsStage + "_TP_DEC_" + branch, entry.second, prefix + "_" + m_swimRepsStage + "_nTP", maxTurns);
        }
      }
    }
    else if (m_swimRepsStage == "Trigger")
    {
      //The each track part is only for the trigger
      // Loop over turning points to fill info names and insert vectors
      MapType track_infos;

      for(stringSet::const_iterator infoname_iter = infonames.begin(); infoname_iter != infonames.end(); infoname_iter++)
        track_infos.insert(make_pair(*infoname_iter, std::vector<double>()));

      if(range.size() == 1)
      {
        LHCb::SwimmingReport* report = range.begin()->to();
        if(!report)
          return Error("Got NULL LHCb::SwimmingReport");
        const tPoints& turns = report->turningPoints(m_swimRepsStage);
        BOOST_FOREACH(const LHCb::TurningPoint& tp, turns)
        {
          // Fill infos
          BOOST_FOREACH(MapType::value_type& entry, track_infos)
          {
            if (!tp.dec()) {
              entry.second.push_back(false);
            } else {
              entry.second.push_back(tp.participated(entry.first, *thisP));
            }
          }
        }
      }

      // Fill tuple with infos
      BOOST_FOREACH(const MapType::value_type& entry, track_infos)
      {
        test &= tuple->farray(prefix + "_"+m_swimRepsStage+"_TP_DEC_" + entry.first, entry.second,
                              prefix + "_"+m_swimRepsStage+"_nTP", maxTurns);
      }
    }
  }
  return StatusCode::SUCCESS;
}

// $Id: L0MuonCandidatesFromRaw.cpp,v 1.7 2007-10-09 23:37:24 jucogan Exp $
#include <algorithm>
#include <math.h>
#include <set>
#include <boost/dynamic_bitset.hpp>

#include "L0MuonCandidatesFromRaw.h"
#include "L0MuonKernel/L0MuonKernelFromXML.h"

/// Gaudi interfaces
#include "GaudiKernel/IChronoStatSvc.h"

/// Gaudi factories for algorithms 
#include "GaudiKernel/AlgFactory.h"

/// Utility classes
#include "GaudiKernel/IToolSvc.h" 
  
#include "Kernel/MuonTileID.h"

DECLARE_ALGORITHM_FACTORY( L0MuonCandidatesFromRaw );

L0MuonCandidatesFromRaw::L0MuonCandidatesFromRaw(const std::string& name,
                                         ISvcLocator* pSvcLocator) 
  :GaudiAlgorithm(name, pSvcLocator)
 
{
  
  m_configfile="L0MuonKernel.xml";

  m_ptParameters.push_back(40.);   // Precision on PT (MeV)
  m_ptParameters.push_back(7.);    // Number of bins in PT
  
  declareProperty( "ConfigFile"     , m_configfile      );
  declareProperty( "PtParameters"   , m_ptParameters    ); 
  declareProperty( "ProcVersion"    , m_procVersion = 0 );
  declareProperty( "Extension"      , m_extension = ""  );
  declareProperty( "WriteOnTES"     , m_writeOnTES = true);  
  declareProperty( "WriteL0ProcData", m_writeL0ProcData  = false);  
  declareProperty( "OutputMode"     , m_mode = 0 );
}


StatusCode L0MuonCandidatesFromRaw::initialize()
{

  StatusCode sc = GaudiAlgorithm::initialize(); // must be executed first
  if ( sc.isFailure() ) return sc;  // error printed already by GaudiAlgorithm

  IChronoStatSvc * svc = chronoSvc();
  svc->chronoStart("L0MuonTrigger Initialize");
  
  L0Muon::RegisterFactory::selectInstance(1);

  // Instanciate the MuonTrigger registers
  std::string xmlFileName = m_configfile;
//   char buf[4096];
//   char* format = "%s/L0MuonKernel_mode%d.xml";
//   sprintf(buf,format,m_configPath.c_str(),m_outputMode);
//   std::string xmlFileName = buf;
  info() <<  "XML file = " << xmlFileName << endmsg;
  L0Muon::L0MuonKernelFromXML(xmlFileName,false);
  debug() <<  "MuonTrigger build from xml "<< endmsg;

  svc->chronoStop("L0MuonCandidatesFromRaw Initialize");
  svc->chronoDelta("L0MuonCandidatesFromRaw Initialize",IChronoStatSvc::KERNEL);
  svc->chronoPrint("L0MuonCandidatesFromRaw Initialize");
  
  // L0MuonOutputs tool
  m_outputTool =  tool<L0MuonOutputs>( "L0MuonOutputs"  , "OutputTool" , this );
  
  m_totEvent = 0;
  m_errorEvent=0;

  return StatusCode::SUCCESS;			  					  		      
}



StatusCode L0MuonCandidatesFromRaw::execute()
{
  L0Muon::RegisterFactory::selectInstance(1);

  debug() << "-----------------------------------------------------------------" << endreq;
  debug() << "-- Start execution:" << endreq;
  IChronoStatSvc * svc = chronoSvc();
  svc->chronoStart("L0MuonTrigger Execute");

  debug() << "Chrono service started" << endreq;
  StatusCode sc;

  // Decode Raw banks
  debug() << "Write on TES ..." << endreq;
  sc = m_outputTool->decodeRawBanks(m_mode);
  if ( sc.isFailure() ) return sc;  // error printed already by GaudiAlgorithm

  // Write on TES
  if ( m_writeOnTES) {
    debug() << "Write on TES ..." << endreq;
    sc = m_outputTool->writeOnTES(m_procVersion,m_extension);
    if ( sc.isFailure() ) return sc;  // error printed already by GaudiAlgorithm
  }

  // Fill the container for the L0DU (L0ProcessorData)
  if ( m_writeL0ProcData) {
    debug() << "Fill L0ProcessorData ..." << endreq;
    sc = m_outputTool->writeL0ProcessorData(m_extension);
    if ( sc.isFailure() ) return sc;  
  }

  // If compare
  if ( m_writeOnTES  
       && ("" != m_extension) 
       && (exist<LHCb::L0MuonCandidates>( LHCb::L0MuonCandidateLocation::Default )) ) {

    //== Compare 
    
    LHCb::L0MuonCandidates* replayed = get<LHCb::L0MuonCandidates>( LHCb::L0MuonCandidateLocation::Default );
    LHCb::L0MuonCandidates* fromraw  = get<LHCb::L0MuonCandidates>( LHCb::L0MuonCandidateLocation::Default + m_extension );

    bool error = false;
   // If the number of candidate don't match
    if (replayed->size()!=fromraw->size()) {
      info() << "Error : replayed !=  from raw" <<endreq;
      info() << " size    : "<<replayed->size()<<" -VS- "<<fromraw->size()<<endreq;
      info() << " => Evt number : "<< m_totEvent<< endreq;
      error = true;
    } else {
      // Loop over candidates of both versions
      LHCb::L0MuonCandidates::const_iterator it_replayed = replayed->begin();
      LHCb::L0MuonCandidates::const_iterator it_fromraw = fromraw->begin();
      while ( replayed->end() != it_replayed && fromraw->end() != it_fromraw ) {
      
        if ( (*it_replayed)->pt()     != (*it_fromraw)->pt()    ||
             (*it_replayed)->theta()  != (*it_fromraw)->theta() ||
             (*it_replayed)->phi()    != (*it_fromraw)->phi()     ) {
        
          error = true;

          info() << "Error :   replayed !=  from raw  (both with "<<replayed->size()<<" candidates)" <<endreq;
          info() << " PT      : "<< (*it_replayed)->pt()    << " -VS- "  << (*it_fromraw)->pt()    << endreq;
          info() << " theta   : "<< (*it_replayed)->theta() << " -VS- "  << (*it_fromraw)->theta() << endreq;
          info() << " phi     : "<< (*it_replayed)->phi()   << " -VS- "  << (*it_fromraw)->phi()   << endreq;
          for (int sta=0; sta<3; ++sta) {  
            info() << " sta M"<<sta+1<<" ";
            std::vector<LHCb::MuonTileID> pads;
            std::vector<LHCb::MuonTileID>::iterator itpad;
            pads= (*it_replayed)->muonTileIDs(sta);
            for (itpad= pads.begin();itpad<pads.end();++itpad){
              info()<<" "<<(*itpad).toString();
            }
            info() << " -VS- ";
            pads  =  (*it_fromraw)->muonTileIDs(sta);
            for (itpad= pads.begin();itpad<pads.end();++itpad){
              info()<<" "<<(*itpad).toString();
            }
            info() <<endreq;
          }
          info() << " => Evt number : "<< m_totEvent<< endreq;
        
        } else {
          debug() << "Entry OK " << it_fromraw-fromraw->begin()<< endreq;
        }
        it_replayed++;
        it_fromraw++;
      } // End of Loop over candidates of both versions

      // If an error occured
      if (error) {
        info() << "Error : fromraw  ( "<<fromraw->size()<<" candidates)" <<endreq;
        for (LHCb::L0MuonCandidates::const_iterator it = fromraw->begin(); it!=fromraw->end(); ++it) {
          info() << " PT      : "<< (*it)->pt()    << endreq;
          info() << " theta   : "<< (*it)->theta() << endreq;
          info() << " phi     : "<< (*it)->phi()   << endreq;
          for (int sta=0; sta<3; ++sta) {  
            info() << " sta M"<<sta+1<<" ";
            std::vector<LHCb::MuonTileID> pads;
            std::vector<LHCb::MuonTileID>::iterator itpad;
            pads= (*it)->muonTileIDs(sta);
            for (itpad= pads.begin();itpad<pads.end();++itpad){
              info()<<" "<<(*itpad).toString();
            }
            info() <<endreq;
          } 
        }
        info() << "Error : replayed  ( "<<replayed->size()<<" candidates)" <<endreq;
        for (LHCb::L0MuonCandidates::const_iterator it = replayed->begin(); it!=replayed->end(); ++it) {
          info() << " PT      : "<< (*it)->pt()    << endreq;
          info() << " theta   : "<< (*it)->theta() << endreq;
          info() << " phi     : "<< (*it)->phi()   << endreq;
          for (int sta=0; sta<3; ++sta) {  
            info() << " sta M"<<sta+1<<" ";
            std::vector<LHCb::MuonTileID> pads;
            std::vector<LHCb::MuonTileID>::iterator itpad;
            pads= (*it)->muonTileIDs(sta);
            for (itpad= pads.begin();itpad<pads.end();++itpad){
              info()<<" "<<(*itpad).toString();
            }
            info() <<endreq;
          }
        }
      } // End if an error occured
    } // End else if the number of candidate don't match

    if (error)  ++m_errorEvent;
    
  }// End If compare

  // Release registers used by the converters
  sc = m_outputTool->releaseRegisters();
  if ( sc.isFailure() ) return sc;  // error printed already by GaudiAlgorithm
  
  svc->chronoStop("L0MuonCandidatesFromRaw Execute");
  svc->chronoDelta("L0MuonCandidatesFromRaw Execute", IChronoStatSvc::KERNEL);
  //if( MSG::DEBUG >= log.level() ) svc->chronoPrint("L0MuonTrigger Execute");

  ++m_totEvent;

  debug() << "-- Execution done." << endreq;
  debug() << "-----------------------------------------------------------------" << endreq;


  return StatusCode::SUCCESS;
  
}


StatusCode L0MuonCandidatesFromRaw::finalize()
{
  info() << "- ------------------------------------------------------------------"<<endmsg;
  info() << "- ========> Final summary of the L0Muon trigger (from raw) <========"<<endmsg;
  info() << "- Total number of events processed           : "
         <<format("%8d",m_totEvent)<<endmsg;
  m_outputTool->statTot(info());
  if ( "" != m_extension ) 
    info() << "- Number of events where with a difference with the reprocessing :"
           << m_errorEvent<<endmsg;
  info() << "- ------------------------------------------------------------------"<<endmsg;
  
  return GaudiAlgorithm::finalize();  // must be called after all other actions
}



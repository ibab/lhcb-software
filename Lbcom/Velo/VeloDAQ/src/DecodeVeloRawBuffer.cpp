// $Id: DecodeVeloRawBuffer.cpp,v 1.4 2006-02-23 18:56:35 krinnert Exp $

#include "GaudiKernel/AlgFactory.h"

#include "Event/RawEvent.h"
#include "Event/RawBank.h"
#include "Event/VeloLiteCluster.h"
#include "Event/VeloCluster.h"

#include "DecodeRawBankToLiteClusters.h"
#include "DecodeRawBankToClusters.h"

#include "DecodeVeloRawBuffer.h"


//-----------------------------------------------------------------------------
// Implementation file for class : DecodeVeloRawBuffer
//
// 2004-02-02 : Chris Parkes
// 2006-02-23 : David Jones, Kurt Rinnert
//              - complete rewrite
//-----------------------------------------------------------------------------

// Declaration of the Algorithm Factory
static const  AlgFactory<DecodeVeloRawBuffer>          Factory ;
const        IAlgFactory& DecodeVeloRawBufferFactory = Factory ; 


//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
DecodeVeloRawBuffer::DecodeVeloRawBuffer( const std::string& name,
                              ISvcLocator* pSvcLocator)
  : GaudiAlgorithm ( name , pSvcLocator ) 
{
  declareProperty("DecodeToVeloLiteClusters",m_decodeToVeloLiteClusters=true);
  declareProperty("DecodeToVeloClusters",m_decodeToVeloClusters=false);
  declareProperty("RawEventLocation",m_rawEventLocation=LHCb::RawEventLocation::Default);
  declareProperty("VeloLiteClustersLocation",m_veloLiteClusterLocation=LHCb::VeloLiteClusterLocation::Default);
  declareProperty("VeloClusterLocation",m_veloClusterLocation=LHCb::VeloClusterLocation::Default);
}


//=============================================================================
// Destructor
//=============================================================================
DecodeVeloRawBuffer::~DecodeVeloRawBuffer() {}; 

//=============================================================================
// Initialisation. Check parameters
//=============================================================================
StatusCode DecodeVeloRawBuffer::initialize() {

  StatusCode sc = GaudiAlgorithm::initialize(); // must be executed first
  if ( sc.isFailure() ) return sc;  // error printed already by GaudiAlgorithm

  debug () << "==> Initialise" << endreq;

  return StatusCode::SUCCESS;
};

//=============================================================================
// Main execution
//=============================================================================
StatusCode DecodeVeloRawBuffer::execute() {

  MsgStream  msg( msgSvc(), name() );

  // fetch raw bank in any case
  LHCb::RawEvent* rawEvent = get<LHCb::RawEvent>(m_rawEventLocation);

  if (!rawEvent) {
    msg << MSG::ERROR 
        << "Raw Event not found in " << m_rawEventLocation
        << endmsg;
  }

  const std::vector<LHCb::RawBank*>& banks = rawEvent->banks(LHCb::RawBank::Velo);

  // decode to lite clusters, if requested
  if (m_decodeToVeloLiteClusters) {
    decodeToVeloLiteClusters(banks);
  }

  // decode to clusters, if requested
  if (m_decodeToVeloClusters) {
    decodeToVeloClusters(banks);
  }

  return StatusCode::SUCCESS;
};

//=============================================================================
// Whatever needs to be done at the end
//=============================================================================
StatusCode DecodeVeloRawBuffer::finalize() {

  debug () << "==> Finalise" << endreq;

  return StatusCode::SUCCESS;
};

//=============================================================================
// Private helpers
//=============================================================================

void DecodeVeloRawBuffer::decodeToVeloLiteClusters(const std::vector<LHCb::RawBank*>& banks) const
{
  for (std::vector<LHCb::RawBank*>::const_iterator bi = banks.begin(); 
       bi != banks.end();
       ++bi) {
    
    const SiDAQ::buffer_word* rawBank = static_cast<const SiDAQ::buffer_word*>((*bi)->data());
    unsigned int sensorNumber = static_cast<unsigned int>((*bi)->sourceID());
   
    LHCb::VeloLiteCluster::FastContainer* fastCont = new LHCb::VeloLiteCluster::FastContainer();
    VeloDAQ::decodeRawBankToLiteClusters(rawBank,sensorNumber,fastCont);
    put(fastCont,m_veloLiteClusterLocation);
  }
 
  return;
}

void DecodeVeloRawBuffer::decodeToVeloClusters(const std::vector<LHCb::RawBank*>& banks) const 
{
  for (std::vector<LHCb::RawBank*>::const_iterator bi = banks.begin(); 
       bi != banks.end();
       ++bi) {
    
    const SiDAQ::buffer_word* rawBank = static_cast<const SiDAQ::buffer_word*>((*bi)->data());
    unsigned int sensorNumber = static_cast<unsigned int>((*bi)->sourceID());
   
    LHCb::VeloClusters* clusters = new LHCb::VeloClusters();
    VeloDAQ::decodeRawBankToClusters(rawBank,sensorNumber,clusters);
    put(clusters,m_veloClusterLocation);
  }
   
  return;
}

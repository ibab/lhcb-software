// $Id: DecodeVeloRawBuffer.cpp,v 1.7 2006-03-14 15:17:02 krinnert Exp $

#include "GaudiKernel/AlgFactory.h"

#include "Event/RawEvent.h"
#include "Event/RawBank.h"
#include "Event/VeloLiteCluster.h"
#include "Event/VeloCluster.h"

#include "DecodeRawBankToLiteClusters.h"
#include "DecodeRawBankToClusters.h"
#include "VeloClusterPtrLessThan.h"

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
  declareProperty("DumpVeloClusters",m_dumpVeloClusters=false);
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

    return StatusCode::FAILURE;
  }

  const std::vector<LHCb::RawBank*>& banks = rawEvent->banks(LHCb::RawBank::Velo);

  StatusCode sc;

  // decode to lite clusters, if requested, bail out if something goes wrong
  if (m_decodeToVeloLiteClusters) {
    sc = decodeToVeloLiteClusters(banks);
    if (sc.isFailure()) return sc;
  }

  // decode to clusters, if requested, bail out if something goes wrong
  if (m_decodeToVeloClusters) {
    sc = decodeToVeloClusters(banks);
    if (sc.isFailure()) return sc;
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

StatusCode DecodeVeloRawBuffer::decodeToVeloLiteClusters(const std::vector<LHCb::RawBank*>& banks) const
{
  LHCb::VeloLiteCluster::FastContainer* fastCont = new LHCb::VeloLiteCluster::FastContainer();

  for (std::vector<LHCb::RawBank*>::const_iterator bi = banks.begin(); 
       bi != banks.end();
       ++bi) {
    const SiDAQ::buffer_word* rawBank = static_cast<const SiDAQ::buffer_word*>((*bi)->data());
    unsigned int sensorNumber = static_cast<unsigned int>((*bi)->sourceID());
   
    VeloDAQ::decodeRawBankToLiteClusters(rawBank,sensorNumber,fastCont);

  }
  put(fastCont,m_veloLiteClusterLocation);
 
  return StatusCode::SUCCESS;
}

StatusCode DecodeVeloRawBuffer::decodeToVeloClusters(const std::vector<LHCb::RawBank*>& banks) const 
{
  LHCb::VeloClusters* clusters = new LHCb::VeloClusters();

  MsgStream  msg( msgSvc(), name() );

  // Number of bytes in bank, including 4 byte header but
  // *without* the padding bytes at the end. 
  // This is filled by the decoding function and should always 
  // be the same as RawBank::size().
  int byteCount;

  for (std::vector<LHCb::RawBank*>::const_iterator bi = banks.begin(); 
       bi != banks.end();
       ++bi) {
    
    const LHCb::RawBank* rb = *bi;
    const SiDAQ::buffer_word* rawBank = static_cast<const SiDAQ::buffer_word*>(rb->data());
    unsigned int sensorNumber = static_cast<unsigned int>(rb->sourceID());
   
    VeloDAQ::decodeRawBankToClusters(rawBank,sensorNumber,clusters,byteCount);

    if (rb->size() != byteCount) {
      msg << MSG::ERROR 
          << "Byte count mismatch between RawBank size and decoded bytes." 
          << " RawBank: " << rb->size() 
          << " Decoded: " << byteCount 
          << endmsg;
      return StatusCode::FAILURE;
    }

  }

  // finally sort the clusters (by sensor and strip number from channel ID)
  std::sort(clusters->begin(),clusters->end(),VeloDAQ::veloClusterPtrLessThan);

  put(clusters,m_veloClusterLocation);
   
  if (m_dumpVeloClusters) dumpVeloClusters(clusters);

  return StatusCode::SUCCESS;
}

void DecodeVeloRawBuffer::dumpVeloClusters(const LHCb::VeloClusters* clusters) const
{
  std::vector<float> adcValues;
  
  for (LHCb::VeloClusters::const_iterator ci =  clusters->begin(); 
       ci != clusters->end(); 
       ++ci) {

    const LHCb::VeloCluster* clu = *ci;
    
    unsigned int sensorNumber = clu->channelID().sensor();
    unsigned int centroidStrip = clu->channelID().strip();

    // interstrip position in 1/8 of strip pitch (as it is encoded in raw bank)
//     float interStripPos = static_cast<unsigned int>((clu->interStripFraction())*8.0)/8.0;
    double interStripPos = clu->interStripFraction();

    std::cout << "DEC::POSDUMP:"
              << " SN=" << sensorNumber
              << " CS=" << centroidStrip
              << " ISP=" << interStripPos
              << std::endl;

    std::cout << "DEC::ADCDUMP:"
              << " SN=" << sensorNumber;

    for (unsigned int adci=0; adci<clu->size(); ++adci) {
      std::cout << " " << clu->adcValue(adci)
                << "@" << clu->strip(adci);
    }
    std::cout << std::endl;
      
  }
  return;
}

// $Id: OTLUTDoublePulseTool.cpp,v 1.0 2011-10-18 abien Exp $

// Gaudi files
#include "GaudiKernel/ToolFactory.h"
#include "GaudiKernel/IHistogramSvc.h"
#include "GaudiKernel/IRndmGenSvc.h"
#include "GaudiKernel/RndmGenerators.h"
#include "GaudiKernel/SystemOfUnits.h"
#include <GaudiUtils/Aida2ROOT.h>
#include "boost/lexical_cast.hpp"
#include <boost/assign/list_of.hpp>

// local
#include "OTLUTDoublePulseTool.h"

/** @file OTLUTDoublePulseTool.cpp
 *
 *  Implementation of OT tool with LUT.
 *
 *  @author A. Bien
 *  @date   18/10/2011
 */

using namespace LHCb;

// Declaration of the tool Factory
DECLARE_TOOL_FACTORY( OTLUTDoublePulseTool );

OTLUTDoublePulseTool::OTLUTDoublePulseTool(const std::string& type,
                                           const std::string& name,
                                           const IInterface* parent) :
  GaudiTool( type, name, parent ),
  m_rgs(0),
  m_histos(0),
  m_spillNames()
{
  declareInterface<IOTDoublePulseTool>(this);

  // constructor
  std::vector<std::string> tmp1 = boost::assign::list_of("/PrevPrev/")
                                        ("/Prev/")
                                        ("/")
                                        ("/Next/")
                                        ("/NextNext/");

  std::vector<double> tmp2 = boost::assign::list_of(-50.0*Gaudi::Units::ns)
                                       (-25.0*Gaudi::Units::ns)
                                       (0.0*Gaudi::Units::ns)
                                       (25.0*Gaudi::Units::ns)
                                       (50.0*Gaudi::Units::ns);

  declareProperty("SpillVector", m_spillVector = tmp1                );
  declareProperty("SpillTimes" , m_spillTimes = tmp2                 );
  declareProperty("HistoPath"  , m_histo_path = "OTDoublePulse/"     );
  declareProperty("Shift"      , m_shift      = 7.0*Gaudi::Units::ns );
}

OTLUTDoublePulseTool::~OTLUTDoublePulseTool()
{
  //destructor
}

StatusCode OTLUTDoublePulseTool::initialize() 
{
  StatusCode sc = GaudiTool::initialize();
  if ( sc.isFailure() ) 
    return Error( "Failed to initialize OTLUTDoublePulseTool", sc );

  // Random number from a flat distribution between 0 and 1000
  sc = m_flat.initialize( randSvc() , Rndm::Flat( 0.0, 1.0) );
  if ( sc.isFailure() ) 
    return Error( "Failed to initialize flat random number generator", sc );

  // Read in the histograms:
  for( int i = 1; i < 10; ++i ) {
    std::string histoname = "TimeShiftMap_M"+boost::lexical_cast<std::string>(i);
    AIDA::IHistogram2D *aida = 
      get<AIDA::IHistogram2D> (histoSvc(), m_histo_path + histoname);
    if( 0==aida ) warning()<<"Could not find AIDA::IHistogram2D* "
                           << m_histo_path + histoname<<"."<<endmsg;
    m_histos.push_back( Gaudi::Utils::Aida2ROOT::aida2root(aida) );
  }

  // construct container names once
  std::vector<std::string>::const_iterator iSpillName = m_spillVector.begin();
  while (iSpillName!=m_spillVector.end()){
    // path in Transient data store
    std::string mcHitPath = "/Event"+(*iSpillName)+MCHitLocation::OT;
    m_spillNames.push_back(mcHitPath);
    ++iSpillName;
  } // iterSpillName


  return StatusCode::SUCCESS;
}

IRndmGenSvc* OTLUTDoublePulseTool::randSvc() const 
{
  if( 0 == m_rgs ) m_rgs = svc<IRndmGenSvc>( "RndmGenSvc", true );
  return m_rgs;
}

double OTLUTDoublePulseTool::findSpillTime(const LHCb::MCHit* aHit) const
{
  // Get the container name of the MCHit
  const ObjectContainerBase* parentCont = aHit->parent(); 
  const IRegistry* reg = parentCont->registry(); 

  // Loop over the spillNames to get the time of the spill
  unsigned int iSpill = 0;
  while( iSpill < m_spillNames.size() && 
         reg->identifier() != m_spillNames[iSpill] ) ++iSpill;

  return m_spillTimes[iSpill];
}


double OTLUTDoublePulseTool::timeOffset(const MCOTDeposit* deposit) const 
{
  const double probability = m_flat();
  const LHCb::MCHit* aHit = deposit->mcHit();
  
  // calculate the time of the main pulse in ns
  double pulsetime = deposit->time() - aHit->time() - findSpillTime(aHit) + m_shift;

  // Find histogram corresponding to the module number
  unsigned int moduleNumber = (deposit->channel()).module();
  TH2D* histo = m_histos[moduleNumber-1];

  // Convert the pulse time and probability to bin number
  int xBin = int(pulsetime*64./50.) + 1; // 64 bins in 50 ns
  int yBin = int(probability*1000.) + 1;

  // Obtain the time offset of the double pulse from the histo
  double pulseshift = histo->GetBinContent(xBin, yBin );
  
  return pulseshift; 
}

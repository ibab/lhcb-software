// $Id: STOfflinePosition.cpp,v 1.15 2009-01-09 16:15:36 jvantilb Exp $
 
// Kernel
#include "GaudiKernel/ToolFactory.h"
#include "GaudiKernel/cbrt.h"  // for cbrt on Windows
#include "GaudiKernel/IIncidentSvc.h"
 
// Event
#include "Event/STDigit.h"
#include "Event/STCluster.h"

// Boost
#include <boost/assign/std/vector.hpp>

// Mathematical tools
#include <math.h>
#include "LHCbMath/Lomont.h"

#include "Kernel/STFun.h"
#include "Kernel/STDataFunctor.h"

#include "STOfflinePosition.h"

using namespace boost::assign;
using namespace LHCb;
using namespace LHCb::Math;


DECLARE_TOOL_FACTORY( STOfflinePosition );
 
STOfflinePosition::STOfflinePosition(const std::string& type, 
                                     const std::string& name,
                                     const IInterface* parent) :
  ST::ToolBase( type, name, parent )
{
  //  m_errorVec += 0.22, 0.12, 0.24, 0.21;
  m_errorVec += 0.24, 0.13, 0.26, 0.23;
  declareProperty("ErrorVec",m_errorVec);
  declareProperty("SharingCorr",m_sharingCorr = 112.);
  declareProperty("MaxNtoCorr",m_maxNtoCorr = 4);
  declareProperty("trim", m_trim = 0.3);
  declareProperty("MergeClusters", m_mergeClusters = false );
  declareSTConfigProperty("InputData", m_clusterLocation , 
                          STClusterLocation::TTClusters);
  declareInterface<ISTClusterPosition>(this);
}

STOfflinePosition::~STOfflinePosition() 
{
  //destructer
}

StatusCode STOfflinePosition::initialize()
{
  StatusCode sc = ST::ToolBase::initialize();
  if (sc.isFailure()) return Error("Failed to initialize", sc);

  // Add incident at begin of each event
  if( m_mergeClusters ) incSvc()->addListener( this, IncidentType::BeginEvent );

  return StatusCode::SUCCESS; 
};

void STOfflinePosition::handle ( const Incident& incident )
{
  if ( IncidentType::BeginEvent == incident.type() ) m_clusters = 0;
}

ISTClusterPosition::Info STOfflinePosition::estimate(const STCluster* 
                                                     aCluster) const 
{
  STCluster::ADCVector adcVector;
  STChannelID firstChan;

  // Merge adc values of neighbouring clusters
  if( m_mergeClusters ) {

    // Get the clusters
    if ( !m_clusters ) m_clusters = get<STClusters>( m_clusterLocation ) ;

    if ( m_clusters ) {
      // Get the iterator belonging to the current cluster
      STClusters::const_iterator iterClus = 
        std::lower_bound( m_clusters->begin(), m_clusters->end(), aCluster, 
                          STDataFunctor::Less_by_Channel<const STCluster*>());

      // Find the left neighbour
      int firstChannel = (*iterClus)->firstChannel();
      --iterClus;
      while( iterClus >= m_clusters->begin() && 
             firstChannel - (*iterClus)->lastChannel() == 1 ) {
        firstChannel = (*iterClus)->firstChannel();
        --iterClus;
      }
      ++iterClus;

      // Add the adc values and save the first channel
      adcVector = (*iterClus)->stripValues();
      firstChan = (*iterClus)->firstChannel();      

      // Add the right neighbours
      int lastChannel = (*iterClus)->lastChannel();
      ++iterClus;
      while( iterClus != m_clusters->end() && 
             (*iterClus)->firstChannel() - lastChannel == 1 ) {
        adcVector.insert( adcVector.end(), (*iterClus)->stripValues().begin(),
                          (*iterClus)->stripValues().end() );
        lastChannel = (*iterClus)->lastChannel();
        ++iterClus;
      }
    }
  } else { // No merging of neighbouring clusters
    adcVector = aCluster->stripValues();
    firstChan = aCluster->firstChannel();
  }

  std::pair<double, unsigned int> info = STFun::position( adcVector, m_trim );
  double stripNum = info.first;

  STChannelID theChan = STChannelID(firstChan.type(), firstChan.station(),
                                    firstChan.layer(), firstChan.detRegion(),
                                    firstChan.sector(), 
                                    (unsigned int)stripNum+firstChan.strip());
  
  ISTClusterPosition::Info theInfo; 
  theInfo.strip = theChan;
  theInfo.fractionalPosition = stripFraction( stripNum - floor(stripNum),
                                              info.second );
  theInfo.fractionalError = error(info.second);
  theInfo.clusterSize = info.second;
                                                                             
  return theInfo;
}

ISTClusterPosition::Info
STOfflinePosition::estimate(const SmartRefVector<STDigit>& digits) const
{  
  std::pair<double, unsigned int> info = STFun::position(digits,m_trim);
  double stripNum = info.first;

  STChannelID firstChan = digits.front()->channelID();
  STChannelID theChan = STChannelID(firstChan.type(), firstChan.station(),
                                    firstChan.layer(), firstChan.detRegion(),
                                    firstChan.sector(), (unsigned int)stripNum);
                                                                             
  ISTClusterPosition::Info theInfo; 
  theInfo.strip = theChan;
  theInfo.fractionalPosition = stripFraction( stripNum - floor(stripNum),
                                              info.second );
  theInfo.fractionalError = error(info.second);
  theInfo.clusterSize = info.second;

  return theInfo;
}

double STOfflinePosition::error(const unsigned int nStrips) const
{ 
 // estimate of error
 return (nStrips < m_errorVec.size() ? 
         m_errorVec[nStrips-1] : m_errorVec.back());
}

double STOfflinePosition::stripFraction(const double stripNum,
                                        const unsigned int clusterSize) const
{
  // 'S- shape correction' for non-linear charge sharing
  double corStripPos = stripNum - floor(stripNum);
  if ( (clusterSize>1) && ((int)clusterSize < m_maxNtoCorr) ) {
    corStripPos = this->chargeSharingCorr(corStripPos);
  }

  return corStripPos;
}

double STOfflinePosition::chargeSharingCorr(const double origDist) const
{
  // non-linear charge sharing correction
  double newDist = origDist-0.5;

  // FIXME: No correction should indeed be applied for symmetric, uneven-sized
  //        clusters (i.e. origDist==0.0), but this makes the correction
  //        function non-continuous. Try to find a better function that still
  //        leaves the zero intact. --- JvT 09.01.2009

  // No correction in case of symmetric, uneven-sized clusters
  if (m_sharingCorr > 0.0 && !lomont_compare_double(origDist, 0.0, 100) ) {
    if (newDist<0.) {
      newDist = -cbrt(-newDist/m_sharingCorr);
    }
    else {
      newDist = cbrt(newDist/m_sharingCorr);
    }
  } // apply corr

  return newDist+0.5;
}

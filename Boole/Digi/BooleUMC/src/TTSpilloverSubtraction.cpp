// Gaudi
#include "GaudiKernel/AlgFactory.h"
#include "GaudiKernel/ToolFactory.h"
// LHCbKernel includes
#include "Kernel/STChannelID.h"
#include "Kernel/STAlgBase.h"
// Event
#include "Event/STDigit.h"
// Local
#include "TTSpilloverSubtraction.h"
// Boost
#include "boost/assign/list_of.hpp"
#include <boost/foreach.hpp>
using namespace LHCb;

DECLARE_ALGORITHM_FACTORY(TTSpilloverSubtraction)

TTSpilloverSubtraction::TTSpilloverSubtraction(const std::string& name,
                                     ISvcLocator* pSvcLocator):
  ST::AlgBase(name, pSvcLocator)
{
  std::vector<std::string> tmp = boost::assign::list_of("/")("/Prev/");
  declareProperty("SpillVector", m_spillNames = tmp);   
  declareSTConfigProperty("InputLocation", m_inputLocation, 
                                           STDigitLocation::TTDigits);

  // Do not subtract Spillover if charge in previous crossing below this value
  declareProperty("PrevChargeCut", m_q_prev_cut =8.5);

  // Subtract this fraction of charge measured in previous bunch crossing
  //   ( if < 0 then set current charge to zero )

  // Before separate sectors. 
  //  declareProperty("SpilloverFraction",  m_q_spillover_frac = boost::assign::list_of(0.3333)(0.3333)(0.3333)(0.3333));
  // After separate sectors.
  std::vector<double> tmp2 = boost::assign::list_of(0.3946)(0.4243)(0.5133)(0.4993);
  declareProperty("SpilloverFraction",  m_q_spillover_frac = tmp2);

  // Saturation value for previous charge (kill current if reached)
  declareProperty("ChargeSaturationCut", m_q_saturation = 126.5 );

}

TTSpilloverSubtraction::~TTSpilloverSubtraction() {}

StatusCode TTSpilloverSubtraction::initialize()
{
  StatusCode sc = ST::AlgBase::initialize();
  if(sc.isFailure()) return Error("Failed to initialize", sc);

  m_spillPaths.clear();
  // Load spill paths
  std::vector<std::string>::const_iterator iSpillName = m_spillNames.begin();
  while (iSpillName != m_spillNames.end()) {
    m_spillPaths.push_back("/Event"+(*iSpillName)+m_inputLocation);
    ++iSpillName;
  }

  return StatusCode::SUCCESS;
}

StatusCode TTSpilloverSubtraction::execute()
{
    // Get input containers
    if( !exist<STDigits>(m_spillPaths[0]) ){
      return Error(" No STDigits for current sampling time ",StatusCode::SUCCESS,100);
    }    
    STDigits* digitContCentr = get<STDigits>(m_spillPaths[0]);
    if( !exist<STDigits>(m_spillPaths[1]) ){
      return Error(" No STDigits for previous sampling time (TT)",StatusCode::SUCCESS,100);
    }    
    STDigits* digitContPrev  = get<STDigits>(m_spillPaths[1]);
    for(LHCb::STDigits::iterator ic = digitContCentr->begin(); 
        ic != digitContCentr->end(); ic++) {
      LHCb::STDigit* dCentr = *ic;
      LHCb::STDigit* dPrev = digitContPrev->object(dCentr->key());

      STChannelID id = dCentr->key();
	  int secLen = -1;
      // For 1 Modules:
      int A[] = {3,9,15,4,10,16};
      int Asize = 6;    
      int B[] = {7,13,19,8,14,20};
      int Bsize = 6;    
      if (id.isTT()){    
          if(( (id.station() == 1) && (id.detRegion()==2) && (IsWithin(A,Asize,id.sector())) ) ||
             ( (id.station() == 2) && (id.detRegion()==2) && (IsWithin(B,Bsize,id.sector())) ) ){
			  secLen = 0; // Corresponds to 1-sensor sectors.
		  }
      }

      // For 2 Modules:
      int C[] = {2,8,14,5,11,17};
      int Csize = 6;    
      int D[] = {6,12,18,9,15,21};
      int Dsize = 6;    
      if (id.isTT()){    
          if(( (id.station() == 1) && (id.detRegion()==2) && (IsWithin(C,Csize,id.sector())) ) ||
             ( (id.station() == 2) && (id.detRegion()==2) && (IsWithin(D,Dsize,id.sector())) ) ){
			  secLen = 1; // Corresponds to 2-sensor sectors.
          }       
      }

      // For 3 Modules:
      int E[] = {2,6,10,14,18,22,
                 3,7,11,15,19,23};
      int Esize = 12;
      int Z[] = {2,3,24,25};
      int Zsize = 4;    
      if (id.isTT()){    
          if(( (id.station() == 1) && (id.detRegion()==1) && (IsWithin(E,Esize,id.sector())) ) ||
             ( (id.station() == 1) && (id.detRegion()==3) && (IsWithin(E,Esize,id.sector())) ) ||
             ( (id.station() == 2) && (id.detRegion()==1) && (IsWithin(E,Esize,id.sector())) ) ||
             ( (id.station() == 2) && (id.detRegion()==2) && (IsWithin(Z,Zsize,id.sector())) ) || 
             ( (id.station() == 2) && (id.detRegion()==3) && (IsWithin(E,Esize,id.sector())) ) ){
			  secLen = 2; // Corresponds to 3-sensor sectors.
          }
      }

      // For 4 Modules:
      int F[] = {1,5,9,13,17,21,
                 4,8,12,16,20,24};
      int Fsize = 12;
      int G[] = {1,7,13,6,12,18};
      int Gsize = 6;
      int H[] = {1,5,11,17,23,
                 4,10,16,22,26};
      int Hsize = 10;
    
      if (id.isTT()){    
          if(( (id.station() == 1) && (id.detRegion()==1) && (IsWithin(F,Fsize,id.sector())) ) ||
             ( (id.station() == 1) && (id.detRegion()==2) && (IsWithin(G,Gsize,id.sector())) ) ||
             ( (id.station() == 1) && (id.detRegion()==3) && (IsWithin(F,Fsize,id.sector())) ) ||
             ( (id.station() == 2) && (id.detRegion()==1) && (IsWithin(F,Fsize,id.sector())) ) ||
             ( (id.station() == 2) && (id.detRegion()==2) && (IsWithin(H,Hsize,id.sector())) ) ||
             ( (id.station() == 2) && (id.detRegion()==3) && (IsWithin(F,Fsize,id.sector())) ) ){
			  secLen = 3; // Corresponds to 4-sensor sectors.
          }
      }

      
	  
	  if( dPrev ){
        double q_prev = dPrev->depositedCharge();
        if( q_prev > m_q_prev_cut ){
          if( m_q_spillover_frac[secLen] < 0.0 ){
            dCentr->setDepositedCharge(0.0);
          } else if ( q_prev < m_q_saturation ) {
            dCentr->setDepositedCharge(dCentr->depositedCharge() - q_prev*m_q_spillover_frac[secLen]);
		  /*// Path Test	
		  std::vector<std::string>::const_iterator iPathTest = m_spillNames.begin();
		  while (iPathTest != m_spillNames.end()) {
			cout << iPathTest->c_str() << endl;
			++iPathTest;
		  }*/
          } else {
            dCentr->setDepositedCharge(0.0);
          }          
        }
      }
    }


  return StatusCode::SUCCESS;
}

/*===========================================================================================*/
/*===================Function to find if a sector is within a particular region==============*/


bool TTSpilloverSubtraction::IsWithin(const int* a, const int a_size, const int test){

    bool b = false;
    int i = 0;
    
    while((!b) && (i < a_size))
    {
    if (a[i] == test){
            b = true; 
    }
        i++;    
    }

    return b;
}

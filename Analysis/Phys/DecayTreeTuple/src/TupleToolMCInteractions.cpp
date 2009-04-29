// $Id: TupleToolMCInteractions.cpp,v 1.1 2009-04-29 17:09:48 rlambert Exp $
// Include files

// from Gaudi
#include "GaudiKernel/ToolFactory.h"

// local
#include "TupleToolMCInteractions.h"

#include "Event/GenHeader.h" 
#include "Event/MCHeader.h" 
#include "Kernel/ParticleID.h" 

#include "GaudiAlg/Tuple.h"
#include "GaudiAlg/ITupleTool.h"

#include "GaudiKernel/IRegistry.h" // IOpaqueAddress

//-----------------------------------------------------------------------------
// Implementation file for class : GenerationTupleTool
//
// 2008-07-01 : Patrick Koppenburg
//-----------------------------------------------------------------------------

using namespace Gaudi;
using namespace LHCb;

// Declaration of the Tool Factory
DECLARE_TOOL_FACTORY( TupleToolMCInteractions );

//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
TupleToolMCInteractions::TupleToolMCInteractions( const std::string& type,
					const std::string& name,
					const IInterface* parent )
  : GaudiTool ( type, name , parent )
    ,m_mean(0.)
    ,m_adjustmean(0.)
    ,m_normaliseAt(0)
    ,m_prefix("EVT_Int")
{
  declareInterface<IEventTupleTool>(this);
  
  // overwrite the mean value of interactions for this event
  declareProperty( "Mean", m_mean=0. );

  // calculate a scaling factor to approximate this number of interactions per event
  declareProperty( "AdjustMean", m_adjustmean=0. );
  
  // calculate a scaling factor to approximate this number of interactions per event
  declareProperty( "NormaliseAt", m_normalise=0. );
  
  // calculate a scaling factor to approximate this number of interactions per event
  declareProperty( "Prefix", m_prefix="EVT_Int" );
}

//=============================================================================

StatusCode TupleToolMCInteractions::initialize() {
  if( ! GaudiTool::initialize() ) return StatusCode::FAILURE;

  return StatusCode::SUCCESS;
}

//=============================================================================

StatusCode TupleToolMCInteractions::fill( Tuples::Tuple& tuple ) {

  unsigned int n =1;
  verbose() << "getting gen header" << endmsg;
  const LHCb::GenHeader* mch = get<LHCb::GenHeader>(LHCb::GenHeaderLocation::Default);
  verbose() << "gen header OK? " << (mch!=NULL) << endmsg;
  

  if(mch && mch->collisions().size()>0)  n = mch->collisions().size() ;
  else //count the number of MC Primary vertices
    {
      verbose() << "getting MCHeader" << endmsg;
      const LHCb::MCHeader* mch = get<LHCb::MCHeader>(LHCb::MCHeaderLocation::Default);
      verbose() << "mc header OK? " << (mch!=NULL) << endmsg;
      if(mch) n = mch->primaryVertices().size() ;
    }
  
  double mean=m_mean;
  if(mean==0.)
    {
      verbose() << "calculating mean" << endmsg;
      //extract it from the GenHeader
      
      //as in the IPileUpTool for FixedLuminosity.{h,cpp}
      if(mch && mch->crossingFreq() && mch->luminosity() && mch->totCrossSection()) mean=mch->luminosity()*mch->totCrossSection()/mch->crossingFreq();
      //if it isn't in the GenHeader, then make a default, the DC06 mean
      else mean=(2.e32 /Gaudi::Units::cm2/Gaudi::Units::s)*(102.4 * Gaudi::Units::millibarn)/(30.0 * Gaudi::Units::megahertz);
      
      verbose() << "calculated mean " << mean << endmsg;
      //if it isn't in the GenHeader, then make a default
    }
  
  verbose() << "Filling tuples" << endmsg;
  bool test = true;
  test &= tuple->column( m_prefix + "_I" , n );
  test &= tuple->column( "_Mean" , mean );
  test &= tuple->column( "_Prob" , poisson(mean,n) );
  
  if(m_adjustmean!=0.)
    {
      verbose() << "Filling adjusted tuples" << endmsg;
      double normalise=1;
      if(m_normaliseAt) normalise=weight(mean,m_adjustmean,m_normaliseAt);
      test &= tuple->column( m_prefix + "_AdjustMean" , m_adjustmean );
      test &= tuple->column( m_prefix + "_AdjustProb" , poisson(m_adjustmean,n) );
      test &= tuple->column( m_prefix + "_AdjustWeight" , weight(mean,m_adjustmean,n) );
    }

  return StatusCode(test);
}

//=============================================================================

double TupleToolMCInteractions::poisson(const double mu, const unsigned int n)
{
  return exp(-mu)*power(mu,n)/factorial(n);
}

double TupleToolMCInteractions::weight(const double mu1, const double mu2, const unsigned int n)
{
  return exp(mu1-mu2)*power(mu2/mu1,n);
}

double TupleToolMCInteractions::factorial(const unsigned int n)
{
  double nfact=1.;
  for(unsigned int i=1;i<=n;i++) nfact*=i;
  return nfact;
}

double TupleToolMCInteractions::power(const double a, const unsigned int x)
{
  if(x==0) return 1;
  double npow=a;
  for(unsigned int i=2;i<=x;i++) npow*=a;
  return npow;
}

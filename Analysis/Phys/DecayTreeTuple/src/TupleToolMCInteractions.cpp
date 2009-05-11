// $Id: TupleToolMCInteractions.cpp,v 1.3 2009-05-11 12:05:11 rlambert Exp $
// Include files

// from Gaudi
#include "GaudiKernel/ToolFactory.h"

// local
#include "TupleToolMCInteractions.h"

#include "Event/GenHeader.h" 
#include "Event/MCHeader.h" 
#include "Kernel/ParticleID.h" 
#include <Event/RecVertex.h>

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
    ,m_useRecPV(false)
    ,m_fillDetails(false)
{
  declareInterface<IEventTupleTool>(this);
  
  // overwrite the mean value of interactions for this event
  declareProperty( "Mean", m_mean=0. );

  // calculate a scaling factor to approximate this number of interactions per event
  declareProperty( "AdjustMean", m_adjustmean=0. );
  
  // normalise the scaling factor such that it is equal to 1 at this value of I
  declareProperty( "NormaliseAt", m_normaliseAt=0 );
  
  // prefix to give to the variables, in case you want to use two copies of this tool
  declareProperty( "Prefix", m_prefix="EVT_Int" );
  
  // use the #of reconstructed PVs, rather than the MC Collisions.
  declareProperty( "UseRecPV", m_useRecPV=false );
  
  // fill extra information on MCPV, MC Collisions and Reconstructed PVs
  declareProperty( "FillDetails", m_fillDetails=false );
}

//=============================================================================

StatusCode TupleToolMCInteractions::initialize() {
  if( ! GaudiTool::initialize() ) return StatusCode::FAILURE;

  return StatusCode::SUCCESS;
}

//=============================================================================

StatusCode TupleToolMCInteractions::fill( Tuples::Tuple& tuple ) {

  unsigned int n =1;
  unsigned int MCI =-1;
  unsigned int MCPV =-1;
  unsigned int RecPV =-1;
  verbose() << "getting gen header" << endmsg;
  const LHCb::GenHeader* gh = get<LHCb::GenHeader>(LHCb::GenHeaderLocation::Default);
  verbose() << "gen header OK? " << (gh!=NULL) << endmsg;
  

  if(gh && gh->collisions().size()>0)  
    {
      MCI = gh->collisions().size() ;
      verbose() << "retrieved I from genHeader" << endmsg;
    }
  
  if(MCI==0 || m_fillDetails==true)
    {
      
      verbose() << "getting MCHeader" << endmsg;
      const LHCb::MCHeader* mch = get<LHCb::MCHeader>(LHCb::MCHeaderLocation::Default);
      verbose() << "mc header OK? " << (mch!=NULL) << endmsg;
      
      if(mch) 
	{
	  verbose() << "retrieved MCPVs from MCHeader" << endmsg;
	  MCPV = mch->primaryVertices().size() ;
	}
    }
  
  if(m_fillDetails==true || m_useRecPV)
    {
      verbose() << "getting PV container" << endmsg;
      const RecVertex::Container* PV = get<LHCb::RecVertex::Container>(LHCb::RecVertexLocation::Primary);
      verbose() << "PV container OK? " << (PV!=NULL) << endmsg;
      
      if(PV) 
	{
	  verbose() << "retrieved PVs from " << LHCb::RecVertexLocation::Primary  << endmsg;
	  RecPV = PV->size() ;
	}
    }
  if(m_useRecPV && RecPV>=0) n=RecPV;
  else if(!m_useRecPV && MCI > 0) n=MCI;
  else if(!m_useRecPV && MCPV > 0) n=MCPV;
  else warning() << "could not retrieve number of interactions, filling as if n=1" << endmsg;
  
  
  double mean=m_mean;
  if(mean==0.)
    {
      verbose() << "calculating mean" << endmsg;
      //extract it from the GenHeader
      
      //as in the IPileUpTool for FixedLuminosity.{h,cpp}
      if(gh && gh->crossingFreq() && gh->luminosity() && gh->totCrossSection())
	{
	  verbose() << "using genheader " << mean << endmsg;
	  mean=gh->luminosity()*gh->totCrossSection()/gh->crossingFreq();
	}
      
      //if it isn't in the GenHeader, then make a default, the DC06 mean
      else 
	{
	  verbose() << "using default for DC06 data " << mean << endmsg;
	  mean=(2.e32 /Gaudi::Units::cm2/Gaudi::Units::s)*(102.4 * Gaudi::Units::millibarn)/(30.0 * Gaudi::Units::megahertz);
	}
      
      
      verbose() << "calculated mean " << mean << endmsg;
      //if it isn't in the GenHeader, then make a default
    }
  
  verbose() << "Filling tuples" << endmsg;
  bool test = true;
  test &= tuple->column( m_prefix + "_I" , n );
  test &= tuple->column( m_prefix + "_Mean" , mean );
  test &= tuple->column( m_prefix + "_Prob" , poisson(mean,n) );
  
  if(m_adjustmean!=0.)
    {
      verbose() << "Filling adjusted tuples" << endmsg;
      double normalise=1;
      if(m_normaliseAt) normalise=weight(mean,m_adjustmean,m_normaliseAt);
      test &= tuple->column( m_prefix + "_AdjustMean" , m_adjustmean );
      test &= tuple->column( m_prefix + "_AdjustProb" , poisson(m_adjustmean,n) );
      test &= tuple->column( m_prefix + "_AdjustWeight" , weight(mean,m_adjustmean,n)/normalise );
      if(m_fillDetails) test &= tuple->column( m_prefix + "_NormalisedAt" , m_normaliseAt );
      if(m_fillDetails) test &= tuple->column( m_prefix + "_NormalisedTo" , normalise );
    }

  if(m_fillDetails)
    {
      verbose() << "Filling detailed tuples" << endmsg;
      double normalise=1;
      test &= tuple->column( m_prefix + "_nMCI" , MCI );
      test &= tuple->column( m_prefix + "_nMCPV" ,MCPV );
      test &= tuple->column( m_prefix + "_nRecPV" , RecPV );
    }

  verbose() << "returning with code " << test << endmsg;
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

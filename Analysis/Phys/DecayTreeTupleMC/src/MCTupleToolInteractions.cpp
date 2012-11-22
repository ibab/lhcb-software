// $Id: MCTupleToolInteractions.cpp,v 1.2 2010-01-26 15:39:25 rlambert Exp $
// Include files

// from Gaudi
#include "GaudiKernel/ToolFactory.h"

// local
#include "MCTupleToolInteractions.h"

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
DECLARE_TOOL_FACTORY( MCTupleToolInteractions )

//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
  MCTupleToolInteractions::MCTupleToolInteractions( const std::string& type,
                                                    const std::string& name,
                                                    const IInterface* parent )
    : TupleToolBase ( type, name , parent )
    ,m_mean(0.)
    ,m_adjustmean(0.)
    ,m_normaliseAt(0)
    ,m_useRecPV(false)
{
  declareInterface<IEventTupleTool>(this);

  // overwrite the mean value of interactions for this event
  declareProperty( "Mean", m_mean=0. );

  // calculate a scaling factor to approximate this number of interactions per event
  declareProperty( "AdjustMean", m_adjustmean=0. );

  // normalise the scaling factor such that it is equal to 1 at this value of I
  declareProperty( "NormaliseAt", m_normaliseAt=0 );

  // prefix to give to the variables, in case you want to use two copies of this tool
  declareProperty( "ExtraName", m_extraName="EVT_Int" );

  // use the #of reconstructed PVs, rather than the MC Collisions.
  declareProperty( "UseRecPV", m_useRecPV=false );

  //deprecated, use Verbose
  // fill extra information on MCPV, MC Collisions and Reconstructed PVs
  //declareProperty( "FillDetails", m_fillDetails=false );

  // change the default PV location
  declareProperty( "RecPVLocation", m_RecPVLocation=LHCb::RecVertexLocation::Primary );
}

//=============================================================================

StatusCode MCTupleToolInteractions::initialize() {
  if( ! TupleToolBase::initialize() ) return StatusCode::FAILURE;

  return StatusCode::SUCCESS;
}

//=============================================================================

StatusCode MCTupleToolInteractions::fill( Tuples::Tuple& tuple )
{
  const std::string prefix=fullName();
  unsigned int n =1;
  int MCI =-1;
  int MCPV =-1;
  int RecPV =-1;
  verbose() << "getting gen header" << endmsg;
  const LHCb::GenHeader* gh = get<LHCb::GenHeader>(LHCb::GenHeaderLocation::Default);
  verbose() << "gen header OK? " << (gh!=NULL) << endmsg;


  if(gh && gh->collisions().size()>0)
  {
    MCI = gh->collisions().size() ;
    verbose() << "retrieved I from genHeader" << endmsg;
  }

  if(MCI<=0 || isVerbose())
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

  if(isVerbose() || m_useRecPV)
  {
    verbose() << "getting PV container" << endmsg;
    const RecVertex::Container* PV = get<LHCb::RecVertex::Container>(m_RecPVLocation);
    verbose() << "PV container OK? " << (PV!=NULL) << endmsg;

    if(PV)
    {
      verbose() << "retrieved PVs from " << (m_RecPVLocation)  << endmsg;
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
  test &= tuple->column( prefix + "_I" , n );
  test &= tuple->column( prefix + "_Mean" , mean );
  test &= tuple->column( prefix + "_Prob" , poisson(mean,n) );

  if(m_adjustmean!=0.)
  {
    verbose() << "Filling adjusted tuples" << endmsg;
    double normalise=1;
    if(m_normaliseAt) normalise=weight(mean,m_adjustmean,m_normaliseAt);
    test &= tuple->column( prefix + "_AdjustMean" , m_adjustmean );
    test &= tuple->column( prefix + "_AdjustProb" , poisson(m_adjustmean,n) );
    test &= tuple->column( prefix + "_AdjustWeight" , weight(mean,m_adjustmean,n)/normalise );
    if(isVerbose()) test &= tuple->column( prefix + "_NormalisedAt" , m_normaliseAt );
    if(isVerbose()) test &= tuple->column( prefix + "_NormalisedTo" , normalise );
  }

  if(isVerbose())
  {
    verbose() << "Filling detailed tuples" << endmsg;
    test &= tuple->column( prefix + "_nMCI" , MCI );
    test &= tuple->column( prefix + "_nMCPV" ,MCPV );
    test &= tuple->column( prefix + "_nRecPV" , RecPV );
  }

  verbose() << "returning with code " << test << endmsg;
  return StatusCode(test);
}

//=============================================================================

double MCTupleToolInteractions::poisson(const double mu, const unsigned int n)
{
  return exp(-mu)*power(mu,n)/factorial(n);
}

double MCTupleToolInteractions::weight(const double mu1, const double mu2, const unsigned int n)
{
  return exp(mu1-mu2)*power(mu2/mu1,n);
}

double MCTupleToolInteractions::factorial(const unsigned int n)
{
  double nfact=1.;
  for(unsigned int i=1;i<=n;i++) nfact*=i;
  return nfact;
}

double MCTupleToolInteractions::power(const double a, const unsigned int x)
{
  if(x==0) return 1;
  double npow=a;
  for(unsigned int i=2;i<=x;i++) npow*=a;
  return npow;
}

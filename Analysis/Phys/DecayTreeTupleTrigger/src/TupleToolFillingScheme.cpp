// $Id: TupleToolFillingScheme.cpp,v 0.1 2015-08-11 12:22:42 bursche Exp $
// Include files

// from Gaudi
#include "GaudiKernel/ToolFactory.h"
#include "GaudiAlg/GaudiAlgorithm.h"
// local
#include "TupleToolFillingScheme.h"
#include "Event/ODIN.h"

//-----------------------------------------------------------------------------
// Implementation file for class : TupleToolFillingScheme
//
// 2015-08-15 : Albert Bursche
//-----------------------------------------------------------------------------

//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
TupleToolFillingScheme::TupleToolFillingScheme( const std::string& type,
                                        const std::string& name,
                                        const IInterface* parent )
  : TupleToolBase ( type, name, parent )
  ,m_conv(NULL)
  ,m_condFilling(NULL)
  ,m_lhcFS("Conditions/Online/LHCb/LHCFillingScheme")
{
  declareInterface<IEventTupleTool>(this);
  declareProperty( "BXOffset" , m_BXOffset = 0 );
 }

//=============================================================================
StatusCode TupleToolFillingScheme::initialize() {
  StatusCode sc = TupleToolBase::initialize(); // must be executed first
  if ( sc.isFailure() ) return sc; // error printed already by GaudiAlgorithm
  m_condFilling = new Condition();
  if (this->existDet<Condition>(m_lhcFS))
    registerCondition(m_lhcFS, m_condFilling, &TupleToolFillingScheme::i_cacheFillingData);
  else
    warning() << "Could not find Filling Scheme in DB!" << endmsg;
  return StatusCode::SUCCESS;
}

//=============================================================================
// Destructor
//=============================================================================
TupleToolFillingScheme::~TupleToolFillingScheme() {}

//=============================================================================
StatusCode TupleToolFillingScheme::fill( Tuples::Tuple& tup )
{
  const std::string prefix = fullName();
  bool test = true;
  //Get bunch crossing ID from ODIN
  LHCb::ODIN* odin;
  if( exist<LHCb::ODIN>(LHCb::ODINLocation::Default) ){
    odin = get<LHCb::ODIN> (LHCb::ODINLocation::Default);
  }else{
    // should remain flagged as an error and stop the run
    return Error("ODIN cannot be loaded", StatusCode::FAILURE);
  }
  long bxid = odin->bunchId();
  
  debug() << "ODIN BCID: " << bxid << endmsg;

  //logic from  FilterFillingScheme::processDB
  const unsigned int max_buckets = m_B1FillingScheme.size();
  assert(max_buckets == m_B2FillingScheme.size());
  debug() << "max_buckets: " << max_buckets << endmsg;
  if (max_buckets)
    {
  
      // ODIN starts counting at 1 - so we have to subtract 1 always to index into the string
      unsigned int index = (m_BXOffset + bxid + max_buckets - 1) % max_buckets;
      unsigned int lastBB, lastBE, lastEB ;
      lastBB = lastBE = lastEB = 0;
      bool isBeamBeam =  (m_B1FillingScheme[index] == '1') && (m_B2FillingScheme[index] == '1');
      for(int i=1;lastBB==0||lastBE==0||lastEB==0;i++)
	{
	  if (m_B1FillingScheme[(index-i) % max_buckets] == '1' &&  m_B2FillingScheme[(index-i) % max_buckets] == '1')
	    if (!lastBB) lastBB=i;
	  if (m_B1FillingScheme[(index-i) % max_buckets] == '0' &&  m_B2FillingScheme[(index-i) % max_buckets] == '1')
	    if (!lastEB) lastEB=i;
	  if (m_B1FillingScheme[(index-i) % max_buckets] == '1' &&  m_B2FillingScheme[(index-i) % max_buckets] == '0')
	    if (!lastBE) lastBE=i;
	}
      
      test &= tup->column( prefix+"EmptyCrossingsToBeamBeam" ,(int)lastBB );
      test &= tup->column( prefix+"EmptyCrossingsToBeamEmpty",(int)lastBE );
      test &= tup->column( prefix+"EmptyCrossingsToEmptyBeam",(int)lastEB );
      test &= tup->column( prefix+"IsBeamBeam", isBeamBeam);
    }
  else
    {
      test &= tup->column( prefix+"EmptyCrossingsToBeamBeam" ,(int) -1);
      test &= tup->column( prefix+"EmptyCrossingsToBeamEmpty",(int) -1);
      test &= tup->column( prefix+"EmptyCrossingsToEmptyBeam",(int) -1);
      test &= tup->column( prefix+"IsBeamBeam", false);
    }

  return StatusCode(test);
}

//=========================================================================
// Extract data from Filling scheme
//=========================================================================
StatusCode TupleToolFillingScheme::i_cacheFillingData() {
  // data from /Conditions/Online/LHCb/LHCFillingScheme
  //if ( msgLevel(MSG::DEBUG) ) debug() << "callback Filling:" << endmsg;
  m_B1FillingScheme = (std::string) m_condFilling->param<std::string>("B1FillingScheme");
  m_B2FillingScheme = (std::string) m_condFilling->param<std::string>("B2FillingScheme");
  debug() <<   "B1FillingScheme: " << m_B1FillingScheme 
	  << "\nB2FillingScheme: " << m_B2FillingScheme  << endmsg;


  
  
  return StatusCode::SUCCESS;
}

// Declaration of the Tool Factory
DECLARE_TOOL_FACTORY( TupleToolFillingScheme )

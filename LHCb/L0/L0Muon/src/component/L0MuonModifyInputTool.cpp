// Include files 

// from Gaudi
#include "GaudiKernel/IRndmGen.h"
#include "GaudiKernel/IRndmGenSvc.h"

// local
#include "L0MuonModifyInputTool.h"

//-----------------------------------------------------------------------------
// Implementation file for class : L0MuonModifyInputTool
//
// 2013-01-21 : Julien Cogan
//-----------------------------------------------------------------------------

// Declaration of the Tool Factory
DECLARE_TOOL_FACTORY( L0MuonModifyInputTool )

//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
L0MuonModifyInputTool::L0MuonModifyInputTool( const std::string& type,
                                              const std::string& name,
                                              const IInterface* parent )
  : GaudiTool ( type, name , parent ),
    m_randSvc ( NULL)
{
  
  declareInterface<IL0MuonModifyInputTool>(this);

  m_params.clear();

  m_params.push_back(1.);
  m_params.push_back(1.);
  m_params.push_back(1.);
  m_params.push_back(1.);

  m_params.push_back(1.);
  m_params.push_back(1.);
  m_params.push_back(1.);
  m_params.push_back(1.);

  m_params.push_back(1.);
  m_params.push_back(1.);
  m_params.push_back(1.);
  m_params.push_back(1.);

  m_params.push_back(1.);
  m_params.push_back(1.);
  m_params.push_back(1.);
  m_params.push_back(1.);

  m_params.push_back(1.);
  m_params.push_back(1.);
  m_params.push_back(1.);
  m_params.push_back(1.);

  declareProperty("Params",m_params);

}
//=============================================================================
// Destructor
//=============================================================================
L0MuonModifyInputTool::~L0MuonModifyInputTool() {} 

//=============================================================================
// Initialization
//=============================================================================
StatusCode L0MuonModifyInputTool::initialize() 
{
  
  m_randSvc = svc<IRndmGenSvc>( "RndmGenSvc", true );
  //StatusCode sc = m_rnd.initialize(randSvc(), Rndm::Flat(0.,1.));
   if ( m_randSvc == NULL ) {
     return Error("Can not initialize the random generator !");;
  }
  return StatusCode::SUCCESS;
}


StatusCode L0MuonModifyInputTool::modifyInput(std::vector<LHCb::MuonTileID> &digits)
{
  //int debug_nOriginal = digits.size();
  //int debug_n = 0;

  Rndm::Numbers rnd ( m_randSvc, Rndm::Flat(0,1) );

  for (std::vector<LHCb::MuonTileID>::iterator it_digit=digits.begin(); it_digit!=digits.end();){
    if (rnd()>m_params[it_digit->station()*4+it_digit->region()]) {
      it_digit = digits.erase(it_digit);
      //++debug_n;
    } else {
      ++it_digit;
    }
  }

  //int debug_nModified = digits.size();
  //info()<<"*** debug : digit size reduced by "<< debug_n<<" from "<<debug_nOriginal<<" to "<<debug_nModified<<endmsg;

  return StatusCode::SUCCESS;
}


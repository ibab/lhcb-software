// Gaudi
#include "GaudiKernel/ToolFactory.h"

// local
#include "STPedestalSimTool.h"

#include "Kernel/LHCbConstants.h"

// boost
#include "boost/assign/list_of.hpp"

#include "Kernel/STChannelID.h"

DECLARE_TOOL_FACTORY(  STPedestalSimTool )

STPedestalSimTool::STPedestalSimTool( const std::string& type, 
                                          const std::string& name,
                                          const IInterface* parent ) :
  GaudiTool(type, name, parent)
{
  // constructer
  declareProperty("ParamValues", m_param = boost::assign::list_of(127.8)(2.45e-3)(9.83e-4 )(-3.25e-6));

  declareInterface<ISTPedestalSimTool>(this);
}

STPedestalSimTool::~STPedestalSimTool()
{ 
  //destructor
}

double STPedestalSimTool::pedestal(const LHCb::STChannelID& chan) const
{
  const unsigned int beetleStrip = (chan.strip() - 1u)% LHCbConstants::nStripsInBeetle;  
  return pedvalue(beetleStrip);
}

double STPedestalSimTool::pedvalue(const unsigned int i)  const{
  // cubic param of the baseline
  return(m_param[0] + i*(m_param[1] + i * (m_param[2] + i *m_param[3]  )));
}

// $Id: $
// Include files

// from Gaudi
#include "GaudiKernel/ToolFactory.h"

// local
#include "VertexFunctionToolRootMini.h"


/** @class VertexFunctionToolRootMini VertexFunctionToolRootMini.h
 *   This tool implements IVertexFunctionTool. See the interface class for a description
 *   of the purpose of these classes.
 *   This class uses the Minuit based maximum Vf finder algorithm.
 *
 *  @author Julien Cogan and Mathieu Perrin-Terrin
 *  @date   2012-07-27
 */

// Declaration of the Tool Factory
DECLARE_TOOL_FACTORY( VertexFunctionToolRootMini )

//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
VertexFunctionToolRootMini::VertexFunctionToolRootMini( const std::string& type,
                                                        const std::string& name,
                                                        const IInterface* parent )
: VertexFunctionTool ( type, name, parent )
{ 
  const char* minName = "Minuit2";
  const char* algoName = "";
  m_min = ROOT::Math::Factory::CreateMinimizer(minName, algoName);
}

//=============================================================================

void VertexFunctionToolRootMini::computeValueMax(LHCb::RecVertex & V, Gaudi::XYZPoint & PMax, double & Max){

  m_min->Clear();

  m_min->SetMaxFunctionCalls(m_max_finder_max_iteration); // for Minuit/Minuit2
  m_min->SetMaxIterations(10000);  // for GSL
  m_min->SetTolerance(m_max_finder_min_step);
  m_min->SetPrintLevel(-1);

  //====================================================================
  // This function finds the nearest local maximum around the vertex
  // fitted postion. It uses ROOT standard minimizer.
  //====================================================================

  
  // VfForMax Vf(this);

  // ROOT::Math::Functor f(&Vf.computeValueAt2,3);
  //ROOT::Math::Functor f(&VertexFunctionTool::computeValueAt2,3);
  Vf_Max f(this);

  double step[3] = {m_step,m_step,m_step};

  //starting point
  double variable[3] = {V.position().X(),V.position().Y(),V.position().Z()};
  //std::cout<<"Probe Value Max Functor :"<<f.DoEval(variable)<<std::endl;
  m_min->SetFunction(f);
  m_min->SetVariable(0,"x",variable[0], step[0]);
  m_min->SetVariable(1,"y",variable[1], step[1]);
  m_min->SetVariable(2,"z",variable[2], step[2]);

  // temp - to limits minuit2 printouts - waiting for a real fix
  // Minuit2:mini seems to sporadically send mysterious std messages which we
  // cannot control... So forcibly intercept them all here and send to /dev/null
  int original_stdout(0), original_stderr(0);
  original_stdout = dup(fileno(stdout));
  fflush(stdout);
  freopen("/dev/null","w",stdout);
  original_stderr = dup(fileno(stderr));
  fflush(stderr);
  freopen("/dev/null","w",stderr);

  m_min->Minimize();

// put std back to normal
  fflush(stdout);
  dup2(original_stdout,fileno(stdout));
  close(original_stdout);
  fflush(stderr);
  dup2(original_stderr,fileno(stderr));
  close(original_stderr);

  const double *xs = m_min->X();
  PMax.SetXYZ(xs[0],xs[1],xs[2]);
  Max = -m_min->MinValue();

  //info()<<"VertexFunctionToolRootMini::computeValueMax for "<<&V<<" new computation = "<<Max<<endmsg;
  //delete(min);
  
}

StatusCode VertexFunctionToolRootMini::finalize()
{
  StatusCode sc = VertexFunctionTool::finalize();
  if ( sc.isFailure() ) return sc;
  
  delete(m_min);
  
  return sc;
  
}

//=============================================================================
// Destructor
//=============================================================================
VertexFunctionToolRootMini::~VertexFunctionToolRootMini() {}

//=============================================================================

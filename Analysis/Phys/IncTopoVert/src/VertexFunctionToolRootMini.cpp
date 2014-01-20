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
{ }

//=============================================================================

void VertexFunctionToolRootMini::computeValueMax(LHCb::RecVertex & V, Gaudi::XYZPoint & PMax, double & Max){

  //====================================================================
  // This function finds the nearest local maximum around the vertex
  // fitted postion. It uses ROOT standard minimizer.
  //====================================================================

  const char* minName = "Minuit2";
  const char* algoName = "";
  //  int randomSeed = -1;
  ROOT::Math::Minimizer* min =
    ROOT::Math::Factory::CreateMinimizer(minName, algoName);
  min->SetMaxFunctionCalls(m_max_finder_max_iteration); // for Minuit/Minuit2
  min->SetMaxIterations(10000);  // for GSL
  min->SetTolerance(m_max_finder_min_step);
  min->SetPrintLevel(-1);

  // VfForMax Vf(this);

  // ROOT::Math::Functor f(&Vf.computeValueAt2,3);
  //ROOT::Math::Functor f(&VertexFunctionTool::computeValueAt2,3);
  Vf_Max f(this);

  double step[3] = {m_step,m_step,m_step};

  //starting point
  double variable[3] = {V.position().X(),V.position().Y(),V.position().Z()};
  //std::cout<<"Probe Value Max Functor :"<<f.DoEval(variable)<<std::endl;
  min->SetFunction(f);
  min->SetVariable(0,"x",variable[0], step[0]);
  min->SetVariable(1,"y",variable[1], step[1]);
  min->SetVariable(2,"z",variable[2], step[2]);

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

  min->Minimize();

// put std back to normal
  fflush(stdout);
  dup2(original_stdout,fileno(stdout));
  close(original_stdout);
  fflush(stderr);
  dup2(original_stderr,fileno(stderr));
  close(original_stderr);

  const double *xs = min->X();
  PMax.SetXYZ(xs[0],xs[1],xs[2]);
  Max = -min->MinValue();

  //info()<<"VertexFunctionToolRootMini::computeValueMax for "<<&V<<" new computation = "<<Max<<endmsg;

}

//=============================================================================
// Destructor
//=============================================================================
VertexFunctionToolRootMini::~VertexFunctionToolRootMini() {}

//=============================================================================

// Include files 
#include <stdio.h> 

// from Gaudi

// local
#include "FwdParameters.h"

//-----------------------------------------------------------------------------
// Implementation file for class : FwdParameters
//
// 27/11/2001 : Olivier Callot
//-----------------------------------------------------------------------------

//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
FwdParameters::FwdParameters() : m_fit(1)
{
}

//=============================================================================
// Destructor
//=============================================================================
FwdParameters::~FwdParameters() {}

//=========================================================================
//  Initialisation, give the size and the parameters
//=========================================================================
void FwdParameters::init(const std::string& title, const std::vector<double>& param)
{
  m_title = title;
  m_par.clear(); m_par.assign(std::begin(param), std::end(param));
  m_grad.clear(); m_grad.resize(m_par.size(), 0.);
  m_fit = LinParFit<double>(m_par.size());
}

void FwdParameters::addEvent(double delta)
{ if (!m_par.empty()) m_fit.accumulate(delta, m_grad); }

//=========================================================================
//  Get the resultant modified parameters, log the results
//=========================================================================
bool FwdParameters::updateParameters(MsgStream& log)
{
  if (m_par.empty()) return false;
  log << MSG::INFO << endmsg 
      << "** " << m_title << "Params **" << endmsg 
      << endmsg;
  const bool okay = m_fit.solve();
  log << m_fit << endmsg;
  if (!okay) return false;
  // figure out number of digits needed for parameter number
  unsigned w = 1 + int(std::floor(std::log(m_fit.size()) / std::log(10.)));
  for (unsigned i = 0; i < m_fit.size(); ++i) {
    log << "    [" << std::setw(w) << i << "] " <<
      std::scientific << std::setw(12) << std::setprecision(4) << m_par[i] << " + " <<
      std::scientific << std::setw(12) << std::setprecision(4) << m_fit[i] << " ( " <<
      std::fixed << std::setw( 8) << std::setprecision(3) << (m_fit[i] / ((0. != m_par[i]) ? m_par[i] : 1.)) << " ) => " <<
      std::scientific << std::setw(12) << std::setprecision(4) << (m_par[i] + m_fit[i]) << endmsg;
    m_par[i] += m_fit[i];
  }
  m_fit.clear();
  return true;
}

//=========================================================================
//  Print on cout the new input line
//=========================================================================
void FwdParameters::printParams(const std::string& prefix)
{
  if (m_par.empty()) return;
  std::cout << prefix << "."
            << m_title << "Params = { " << m_par[0];
  for (unsigned i = 1; i < m_par.size(); ++i) {
    std::cout << ", " << m_par[i];
  }
  std::cout << " };" << std::endl;
}

//=========================================================================
//  Print on cout the new input line
//=========================================================================
void FwdParameters::printPythonParams(const std::string& prefix)
{
  if (m_par.empty()) return;
  std::cout << prefix << "()."
            << m_title << "Params = [ "<< m_par[0];
  for (unsigned i = 1; i < m_par.size(); ++i) {
    std::cout << ", " << m_par[i];
  }
  std::cout << " ]" << std::endl;
}

//=============================================================================

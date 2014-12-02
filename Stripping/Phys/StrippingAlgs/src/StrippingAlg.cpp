#include "Kernel/SelectionLine.h"
#include "GaudiKernel/AlgFactory.h"
#include "GaudiKernel/VectorMap.h"
#include "Event/Particle.h"

class StrippingAlg : public Selection::Line
{

public:

  StrippingAlg( const std::string& name, ISvcLocator* pSvcLocator );
  StatusCode initialize();

private:

  int numberOfCandidates() const;
  std::pair<std::string,unsigned> id() const;

private:

  void SetupSelections();
  std::pair<std::string,unsigned> SetupID();
  std::pair<std::string,unsigned> m_id;
  std::string m_outputLocation;

};

StrippingAlg::StrippingAlg( const std::string& name, ISvcLocator* pSvcLocator )
  : Selection::Line(name,pSvcLocator)
{
  declareProperty("OutputLocation", m_outputLocation);
}

StatusCode StrippingAlg::initialize()
{
  const StatusCode sc = Selection::Line::initialize();
  if ( sc.isFailure() ) return sc;

  SetupSelections();
  m_id = std::make_pair(std::string(decisionName()), 1);
  return sc;
}

int StrippingAlg::numberOfCandidates() const
{
  const LHCb::Particle::Range parts = getIfExists<LHCb::Particle::Range>(m_outputLocation);
  const int num = parts.size();

  if ( msgLevel(MSG::VERBOSE))
    verbose() << "Selection " << m_outputLocation << " finds " << num
              << " candidates" << endmsg ;

  return num;
}


void StrippingAlg::SetupSelections() { }

std::pair<std::string,unsigned> StrippingAlg::id() const
{
  return m_id;
}


// Declaration of the Algorithm Factory
DECLARE_ALGORITHM_FACTORY( StrippingAlg )


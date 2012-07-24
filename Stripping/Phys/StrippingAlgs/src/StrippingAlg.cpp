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

  unsigned int numberOfCandidates() const;
  unsigned int numberOfCandidates(const Algorithm*) const;
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
  //  m_id = SetupID();

  m_id = std::make_pair(std::string(decisionName()), 1);

  return sc;
}

unsigned int StrippingAlg::numberOfCandidates() const
{
  const LHCb::Particle::Range parts = getIfExists<LHCb::Particle::Range>(m_outputLocation);
  const unsigned int num = parts.size();

  if ( msgLevel(MSG::VERBOSE))
    verbose() << "Selection " << m_outputLocation << " finds " << num
              << " candidates" << endmsg ;

  return num;
}

unsigned int StrippingAlg::numberOfCandidates(const Algorithm* /*algorithm*/) const
{
  return 0 ;
}

void StrippingAlg::SetupSelections() { }

std::pair<std::string,unsigned> StrippingAlg::id() const
{
  return m_id;
}

std::pair<std::string,unsigned> StrippingAlg::SetupID()
{
  // TODO: do this during initialize, 'lock' updates after initialize...

  const std::string *major = 0;
  static const std::string major1("StrippingSelectionID");
  if (decisionName().find("Stripping") == std::string::npos)
  {
    error() << " Could not find Stripping in decision " << decisionName() << " ???? " << endmsg;
  }
  else
  {
    major = &major1;
  }
  boost::optional<IANNSvc::minor_value_type> key = annSvc().value(*major,decisionName());

  if (!key)
  {
    warning() << " DecisionName=" << decisionName() << " not known under major " << major << endmsg;
    return std::make_pair(std::string(),0);
  }
  return *key;
}

// Declaration of the Algorithm Factory
DECLARE_ALGORITHM_FACTORY( StrippingAlg )


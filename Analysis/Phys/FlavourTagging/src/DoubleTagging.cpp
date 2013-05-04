// $Id: DoubleTagging.cpp,v 1.4 2006-10-24 10:16:44 jpalac Exp $
// Include files
// local
#include "DoubleTagging.h"

//-----------------------------------------------------------------------------
// Implementation file for class : DoubleTagging
//
// 2006-01-28 : Christopher Peter Barnes
// 2006-05-15 : Onuora Awunor
//-----------------------------------------------------------------------------

double efferr(double eff, int tot);

using namespace LHCb ;
using namespace Gaudi::Units;

// Declaration of the Algorithm Factory
DECLARE_ALGORITHM_FACTORY( DoubleTagging )

//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
  DoubleTagging::DoubleTagging( const std::string& name,
                                ISvcLocator* pSvcLocator)
    : DaVinciAlgorithm ( name , pSvcLocator )
{
  declareProperty( "MuonMissTagProb",m_Wm           = 0.32);
  declareProperty( "MuonMissTagProbError",m_SigmaWm = 0.017);
  declareProperty( "ElectronMissTagProb",m_We       = 0.33);
  declareProperty( "ElectronMissTagProbError",m_SigmaWe = 0.017);
  declareProperty( "OSKaonMissTagProb",m_Wosk           = 0.37);
  declareProperty( "OSKaonMissTagProbError",m_SigmaWosk = 0.017);
  declareProperty( "OSKaonMuonMissTagProb",m_Woskm      = 0.22);
  declareProperty( "OSKaonMuonMissTagProbError",m_SigmaWoskm = 0.017);
  declareProperty( "OSKElectronMissTagProb",m_Woske          = 0.26);
  declareProperty( "OSKElectronMissTagProbError",m_SigmaWoske= 0.017);
  declareProperty( "VertexMissTagProb",m_Wv                  = 0.42);
  declareProperty( "VertexMissTagProbError",m_SigmaWv        = 0.017);
  declareProperty( "CombinedMissTagProb",m_Wcomb             = 0.36);
  declareProperty( "CombinedMissTagProbError",m_SigmaWcomb   = 0.017);

  declareProperty( "OutputTagLocation", m_TagLocation = "/Event/Phys/Tags");
  declareProperty( "OutputOSTagLocation", m_TagLocationOS = "/Event/Phys/OSTags");
}

//=============================================================================
// Destructor
//=============================================================================
DoubleTagging::~DoubleTagging() {}

//=============================================================================
// Initialization
//=============================================================================
StatusCode DoubleTagging::initialize() 
{
  const StatusCode sc = DaVinciAlgorithm::initialize();
  if (sc.isFailure()) return sc;

  m_ntotal = m_ndt_total = m_nag_total = 0;

  m_oscombine = tool<ICombineTaggersTool> ("CombineTaggersOSTDR",this);

  inputW.push_back(m_Wm);
  inputW.push_back(m_We);
  inputW.push_back(m_Wosk);
  inputW.push_back(m_Woskm);
  inputW.push_back(m_Woske);
  inputW.push_back(m_Wv);

  inputSigmaW.push_back(m_SigmaWm);
  inputSigmaW.push_back(m_SigmaWe);
  inputSigmaW.push_back(m_SigmaWosk);
  inputSigmaW.push_back(m_SigmaWoskm);
  inputSigmaW.push_back(m_SigmaWoske);
  inputSigmaW.push_back(m_SigmaWv);

  ncategories = 6;
  for(size_t i=0;i<ncategories;++i)
  {
    ndoubletagged.push_back(0);
    nagree.push_back(0);
    Wk.push_back(0);
    SigmaWk.push_back(0);

    debug() << "Input W      " << i+1 << " = " << inputW[i] << endmsg;
    debug() << "Input SigmaW " << i+1 << " = " << inputSigmaW[i] << endmsg;
  }

  return sc;
}

//=============================================================================
// Main execution
//=============================================================================
StatusCode DoubleTagging::execute() 
{
  
  const Particle::Range parts = this->particles();
  if(parts.empty())
  {
    if ( msgLevel(MSG::DEBUG) )
      debug() << "No B candidates" << endmsg;
    setFilterPassed(false);
    return StatusCode::SUCCESS;
  }

  FlavourTags*  tags   = new FlavourTags();
  FlavourTags*  OStags = new FlavourTags();
  put(tags,m_TagLocation);
  put(OStags,m_TagLocationOS);

  for(Particle::Range::const_iterator icandB = parts.begin();
      icandB != parts.end(); ++icandB)
  {

    if((*icandB)->particleID().hasBottom())
    {
      ++m_ntotal;
      if ( msgLevel(MSG::DEBUG) )
        debug() << "Running tagging on candidate with PID = " 
                << (*icandB)->particleID() << endmsg;
      FlavourTag* Tag   = new FlavourTag;
      FlavourTag* OSTag = new FlavourTag;

      if(flavourTagging()->tag(*Tag,*icandB) != 1 )
      {
        if ( msgLevel(MSG::DEBUG) )
          debug() << "All Tagging failed" << endmsg;
        delete Tag;
        delete OSTag;
        continue;
      }
      else
      {
        tags->insert(Tag);
        OStags->insert(OSTag);

        int sskaondec = 0;
        std::vector<Tagger*> ptaggers;
        std::vector<Tagger> taggers = Tag->taggers();

        if ( msgLevel(MSG::DEBUG) )
          debug() << "Total number of taggers ran in BTaggingTool = "
                  << taggers.size() << endmsg;

        for(size_t i=0;i<taggers.size();++i)
        {
          if ( msgLevel(MSG::DEBUG) )
            debug() << "Type of taggers ran in BTaggingTool =        "
                    << taggers[i].type() << endmsg;
          if(taggers[i].type() == Tagger::SS_Kaon)
          {
            sskaondec = taggers[i].decision();
            if ( msgLevel(MSG::DEBUG) )
              debug() << "SS Kaon decision = " << sskaondec << endmsg;
          }
          else ptaggers.push_back( &(taggers[i]) );
        }
        int signalType=1;

        warning() << " WARNING: make combination assuming Bu or Bd (with SSpion!!) fix it!!!"
                  << signalType<<endmsg;
        
        const unsigned int category = m_oscombine->combineTaggers(*OSTag,ptaggers,signalType);
        const std::vector<Tagger> OStaggers = OSTag->taggers();

        if( sskaondec == 0 || category == 0 ) continue;

        for(size_t i=0;i<ncategories;++i)
        {
          if(category == i+1)
          {
            ++ndoubletagged[i];
            ++m_ndt_total;
            if(OSTag->decision() == sskaondec)
            {
              ++nagree[i];
              ++m_nag_total;
            }
            break;
          }
        }

      }
    }
  }
  setFilterPassed(true);   // Mandatory. Set to true if event is accepted.
  return StatusCode::SUCCESS;
}

//=============================================================================
//  Finalize
//=============================================================================
StatusCode DoubleTagging::finalize()
{

  for(size_t i=0;i<ncategories;++i)
  {
    debug() << "ndoubletagged = " << i+1 << " = " << ndoubletagged[i] << endmsg;
    debug() << "nagree =        " << i+1 << " = " << nagree[i] << endmsg;

    Wk[i] = calculateWk(ndoubletagged[i],nagree[i],inputW[i]);
    SigmaWk[i] = calculateSigmaWk(ndoubletagged[i],nagree[i],inputW[i],inputSigmaW[i]);
  }

  double Wktot = 0;
  double SigmaWktot = 0;
  combineMeasurements(Wk,SigmaWk,Wktot,SigmaWktot);

  info() << "********************TAGEVAL************************" << endmsg;
  info() << "Total selected =      " << m_ntotal << endmsg;
  info() << "Total double tagged = " << m_ndt_total << endmsg;
  info() << "Total agree         = " << m_nag_total << endmsg;

  for(int k=0;k<6;++k) printInfo(k);

  info() << "**********************RESULT***********************" << endmsg;
  info() << "*****************CATEGORY APPROACH*****************" << endmsg;
  info() << "Output Wk =       " << Wktot << endmsg;
  info() << "Output SigmWk =   " << SigmaWktot << endmsg;
  info() << "*****************COMBINED APPROACH*****************" << endmsg;
  info() << "Input W =         " << m_Wcomb << endmsg;
  info() << "Input SigmaW =    " << m_SigmaWcomb << endmsg;
  info() << "Output Wk =       " << calculateWk(m_ndt_total,m_nag_total,m_Wcomb) << endmsg;
  info() << "Output SigmWk =   " << calculateSigmaWk(m_ndt_total,m_nag_total,m_Wcomb,m_SigmaWcomb) << endmsg;

  return DaVinciAlgorithm::finalize();
}

//================================================================
double DoubleTagging::calculateWk(int ndt,int nag,double Wtagger)
{
  if(!ndt || !nag) return -1;
  const double fraction = ((double) nag)/ndt;
  return (1 - fraction - Wtagger)/(1 - 2*Wtagger);
}

//================================================================
double DoubleTagging::calculateSigmaWk(int ndt,int nag,double Wtagger,double SigmaWtagger)
{
  if(!ndt || !nag) return -1;
  const double frac = ((double) nag)/ndt;
  const double wmfrac = (1 - 2*Wtagger);
  const double fracfac = (1 - 2*frac);

  const double fracvariance = frac*(1-frac)/ndt;
  const double fcontrvar = fracvariance/(wmfrac*wmfrac);
  const double Wmcontrvar = 
    (fracfac*fracfac/(wmfrac*wmfrac*wmfrac*wmfrac))*SigmaWtagger*SigmaWtagger;

  return std::sqrt(Wmcontrvar + fcontrvar);
}

//================================================================
double efferr(double eff, int tot)
{
  return std::sqrt( eff*(1-eff)/tot);
}

//================================================================
void DoubleTagging::combineMeasurements(std::vector<double>& Wks,
                                        std::vector<double>& SigmaWks,
                                        double& Wktot, double& SigmaWktot)
{
  double invarsum = 0;
  Wktot = 0;
  for(size_t i=0;i<Wks.size();++i)
  {
    if( (SigmaWks[i] != 0) && (Wks[i] > 0) ){
      invarsum += 1/(SigmaWks[i]*SigmaWks[i]);
      Wktot += Wks[i]/(SigmaWks[i]*SigmaWks[i]);
    }
  }
  if(invarsum > 0)
  {
    Wktot = Wktot/invarsum;
    SigmaWktot = std::sqrt(1/invarsum);
  }
  else
  {
    Wktot = 0;
    SigmaWktot = 0;
  }
  return;
}

//================================================================
void DoubleTagging::printInfo(int i)
{
  info() << "********************CAT "<< i+1 << " **************************" << endmsg;
  info() << "Input W =      " << inputW[i] << endmsg;
  info() << "Input SigmaW = " << inputSigmaW[i] << endmsg;
  info() << "Doubletagged = " << ndoubletagged[i] << endmsg;
  info() << "Agree =        " << nagree[i] << endmsg;
  info() << "Wk =           " << Wk[i] << endmsg;
  info() << "SigmaWk =      " << SigmaWk[i] << endmsg;
}

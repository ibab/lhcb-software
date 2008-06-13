
#include "CrudeSampler.h"

#include <ctime>

#include "Data.h"
#include "ThreePointCircleProposerB.h"
#include "RectilinearCPQuantizer.h"
#include "NimTypeRichModel.h"
#include "EventDescription.h"
#include "CLHEP/Random/RandFlat.h"
#include "Utils/MessageHandler.h"
#include "Utils/CheckForNan.h"

using namespace Lester;

CrudeSampler::CrudeSampler(boost::shared_ptr<Lester::NimTypeRichModel> ntrm)
  : m_ntrm(ntrm) { }

std::ostream & CrudeSampler::printMeTo(std::ostream & os) const 
{
  return os << "CrudeSampler[]";
}

std::ostream & operator<<(std::ostream & os, const CrudeSampler & obj) 
{
  return obj.printMeTo(os);
}

boost::shared_ptr<GenRingF::GenericResults>
CrudeSampler::fit(const GenRingF::GenericInput & input) throw (CouldNotFit)
{
  try
  {

    const Lester::NimTypeRichModel & ntrm = *m_ntrm;

    Lester::Data data;
    data.hits.reserve( input.hits.size() );
    for (GenRingF::GenericInput::GenericHits::const_iterator it = input.hits.begin();
         it != input.hits.end();
         ++it) 
    {
      data.hits.push_back(Lester::Hit(it->x(),it->y()));
    }

    RectilinearCPQuantizer rcpq(ntrm);
    Lester::ThreePointCircleProposerB p ( data,
                                          rcpq,
                                          ntrm.circleMeanRadiusParameter*0.1,
                                          ntrm.circleMeanRadiusParameter*0.1,
                                          ntrm );

    Lester::EventDescription initialPoint;

    // Try to set it up as a "reasonable" blank slate from which to start the finder
    assert(initialPoint.circs.empty());

    Lester::EventDescription currentPoint = initialPoint;
    double currentLogProb; // initialised in next statement
    try
    {
      currentLogProb = ntrm.totalLogProbOfEventDescriptionGivenData(currentPoint, data);
      Lester::messHandle().debug() << "Current point has logProb " << currentLogProb << Lester::endmsg;
    }
    catch (Lester::LogarithmicTools::LogOfZero &)
    {
      Lester::messHandle().debug() << "Initial point was not sufficiently good.  I refuse to carry on at line " 
                                   << __LINE__ << " in " << __FILE__ << Lester::endmsg;
      throw CouldNotFit("The first point (i.e. the initial conditions) for the 'fit' was very bad.");
    }

    boost::shared_ptr<GenRingF::GenericResults> ansP(new GenRingF::GenericResults);
    GenRingF::GenericResults & ans = *ansP;

    // run options
    if ( configuration.hasParam("ScaleNumItsByHits") )
    {

      const double targetIts          = configuration.checkThenGetParam("TargetIterations");
      const double numHitsAtTargetIts = configuration.checkThenGetParam("TargetHits");
      const double absMaxIts          = configuration.checkThenGetParam("AbsMaxIts");
      const double absMinIts          = configuration.checkThenGetParam("AbsMinIts");

      double runIts = data.hits.size() * (targetIts/numHitsAtTargetIts);
      if      ( runIts > absMaxIts ) { runIts = absMaxIts; }
      else if ( runIts < absMinIts ) { runIts = absMinIts; }

      // start time
      const clock_t startTime = clock();

      // do the fitting
      unsigned int nIts = 0;
      while ( nIts < runIts )
      {
        ++nIts;
        //Lester::messHandle().verbose() << "Iteration " << nIts << "/" << runIts << Lester::endmsg;
        doTheWork ( currentPoint, currentLogProb, p, ntrm, data );
        //Lester::messHandle().verbose() << "  -> " << currentPoint << " " << currentLogProb << Lester::endmsg;
      }

      // stop time
      const clock_t stopTime = clock();

      // fill results
      ans.timeTaken = 1000*(stopTime-startTime)/CLOCKS_PER_SEC;
      ans.numIterations = nIts;

    }
    else if ( configuration.hasParam("RunForFixedIterations") )
    {
      const double runIts = configuration.checkThenGetParam("AbsMaxNumIts");

      // start time
      const clock_t startTime = clock();

      // do the fitting
      unsigned int nIts = 0;
      while ( nIts < runIts )
      {
        ++nIts;
        doTheWork(currentPoint,currentLogProb,p,ntrm,data);
      }

      // stop time
      const clock_t stopTime = clock();

      // fill results
      ans.timeTaken = 1000*(stopTime-startTime)/CLOCKS_PER_SEC;
      ans.numIterations = nIts;

    }
    else if ( configuration.hasParam("RunForFixedTime") )
    {
      const double runTime = configuration.checkThenGetParam("AbsMaxTime");

      // start time
      const clock_t startTime = clock();

      // do the fitting
      unsigned int nIts = 0;
      while ( clock() - startTime<runTime*CLOCKS_PER_SEC )
      {
        ++nIts;
        doTheWork(currentPoint,currentLogProb,p,ntrm,data);
      }

      // stop time
      const clock_t stopTime = clock();

      // fill results
      ans.timeTaken = 1000*(stopTime-startTime)/CLOCKS_PER_SEC;
      ans.numIterations = nIts;

    }
    else
    {
      throw CouldNotFit("Unspecified Fit Mode");
    }

    currentPoint.fill(ans, input, m_ntrm);
    return ansP;

  } 
  catch ( const CouldNotFit & excpt )
  {
    throw excpt;
  }
  catch ( const std::exception & excpt ) 
  {
    throw CouldNotFit( excpt.what() );
  } 
  catch (...) 
  {
    throw CouldNotFit( "An unknown exception occurred. Bad" );
  }

}

void CrudeSampler::doTheWork ( Lester::EventDescription & currentPoint,
                               double & currentLogProb,
                               Lester::ThreePointCircleProposerB & p,
                               const Lester::NimTypeRichModel & ntrm,
                               const Lester::Data & data )
{
  data.doNothing();

  // suggest insert/remove/jitter each with prob 1/3:
  const double insProb = 0.4;
  const double remProb = 0.1;
  //const double jitProb = 1.-(insProb+remProb);

  const double           d = RandFlat::shoot();
  const bool proposeInsert = d<insProb;
  const bool proposeRemove = (proposeInsert?false:(d<insProb+remProb));
  const bool proposeJitter = !(proposeInsert||proposeRemove);

  Lester::EventDescription proposal = currentPoint;
  double qReverseOverQForward       = 1;
  bool keepForSure                  = false;

  if (proposeInsert)
  {
    //Lester::messHandle().verbose("Proposing insertion");
    // what shall we propose adding?
    const Lester::CircleParams & c = p.sample();
    proposal.circs.push_back(c);
    const double qForward = insProb*(p.probabilityOf(c));
    const double qReverse = remProb;
    qReverseOverQForward = qReverse/qForward;
  }
  else if (proposeRemove)
  {
    const unsigned int siz = currentPoint.circs.size();
    if (siz==0)
    {
      //Lester::messHandle().verbose("Proposing crummy removal");
      // leave as we are}
      keepForSure = true;
    }
    else
    {
      //Lester::messHandle().verbose("Proposing removal");
      const int toGo = RandFlat::shootInt(siz);
      proposal.circs[toGo] = proposal.circs[siz-1];
      proposal.circs.pop_back();
      const double qForward = remProb;
      const double qReverse = insProb*p.probabilityOf(currentPoint.circs[toGo]);
      qReverseOverQForward = qReverse/qForward;
    };
  }
  else
  {
    assert(proposeJitter);
    const unsigned int siz = currentPoint.circs.size();
    if (siz==0) 
    {
      //Lester::messHandle().verbose("Proposing crummy jitter");
      // leave as we are!
      keepForSure = true;
    } 
    else 
    {
      //Lester::messHandle().verbose("Proposing jitter");
      //qReverseOverQForward = 1;
      const int toJitter = RandFlat::shootInt(siz);
      proposal.circs[toJitter] = proposal.circs[toJitter].jitterSymm1();
    }
  }

  bool acceptedProposal = false;
  try 
  {
    if (keepForSure) 
    {
      // do nothing -- treat as a failed proposal.
    } 
    else 
    {
      const double proposedLogProb = ntrm.totalLogProbOfEventDescriptionGivenData(proposal,data);
      const double rhoMax = std::exp(proposedLogProb-currentLogProb) * qReverseOverQForward;
      acceptedProposal = Lester::lfin(rhoMax) && (RandFlat::shoot()<rhoMax);

      if (acceptedProposal) 
      {
        currentPoint = proposal;
        currentLogProb = proposedLogProb;

      }
    }
  } 
  catch (...) 
  {
    // infinitely unlikely point: Reject!
  };

  if (acceptedProposal) 
  {
    static unsigned int count=0;
    ++count;
    count %=30;
  };

  // Lester::messHandle().debug() << "CurrentPoint " << currentPoint << Lester::endmsg;
}

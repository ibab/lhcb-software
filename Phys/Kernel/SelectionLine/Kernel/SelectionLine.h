// $Id: SelectionLine.h,v 1.2 2010-01-18 15:44:53 graven Exp $
#ifndef SELECTION_LINE_H
#define SELECTION_LINE_H 1

// Include files
// from Gaudi
#include "Kernel/IANNSvc.h"
#include "GaudiKernel/IIncidentListener.h"
#include "GaudiAlg/GaudiHistoAlg.h"
#include "boost/array.hpp"

// Forward declarations
class StatEntity;
class ISequencerTimerTool;
class Algorithm;
class IJobOptionsSvc;
class IAlgManager;

/** @class Line Line.h
 *  Sequencer for (Hlt) processing, adapted from GaudiSequencer,
 *  but more specific...
 *
 *  @author Gerhard Raven
 *  @date   2008-09-13
 */

namespace Selection {

    class Line : public GaudiHistoAlg
               , virtual public IIncidentListener

    {
    public:
      /// Standard constructor
      Line( const std::string& name, ISvcLocator* pSvcLocator );
      
      virtual ~Line( ); ///< Destructor

      virtual StatusCode initialize();    ///< Algorithm initialization
      virtual StatusCode execute   ();    ///< Algorithm execution

      void handle(const Incident&);

      void resetExecuted();         ///< Called before an event processing

      // id for this line -- return 0 if error
      virtual std::pair<std::string,unsigned int> id() const = 0; 
      // @TODO/FIXME: for now, just use inheritance to force _some_
      //  implementation -- should be done smarter
      // retrieve numberOfCandidates from subalgo 
      virtual unsigned int numberOfCandidates(const Algorithm*) const = 0;
      // retrieve numberOfCandidates from decision algo 
      virtual unsigned int numberOfCandidates() const = 0;


      class Stage {
      public:
        Stage(Line& parent, const std::string& name)
        : m_parent(parent)
        , m_timerTool(0) 
        , m_algorithm(0)
        , m_property(name, std::string())
        , m_reverse(false)
        , m_dirty(true)
        , m_initialized(false)
        {  m_property.declareUpdateHandler(&Stage::updateHandler,this); }

        Stage( Stage& rhs )
        : m_parent(rhs.m_parent)
        , m_timerTool(rhs.m_timerTool) 
        , m_algorithm(0)
        , m_property( rhs.m_property )
        , m_reverse(false)
        , m_dirty(true)
        , m_initialized(false)
        { m_property.declareUpdateHandler(&Stage::updateHandler,this); }

        ~Stage() { if (m_algorithm) m_algorithm->release();}; ///< Destructor
        bool passed() const            { return algorithm()?algorithm()->filterPassed():true; }
            
        StatusCode execute(ISequencerTimerTool* = 0);
        StatusCode initialize(ISequencerTimerTool* = 0);
        std::string toString() const { return name(); }
        StatusCode  fromString(const std::string& name);
        const std::string& name()     const { return m_property.value(); }
        void resetExecuted() const          { if (algorithm()) algorithm()->resetExecuted(); }
        StringProperty& property()          { return m_property; }
      private:
        void setTimer( int nb )              { m_timer = nb;       }
        Algorithm* algorithm()        const  { return m_algorithm; }
        
        bool       reverse()          const  { return m_reverse;   }
        int        timer()            const  { return m_timer;     }
        void updateHandler(Property& prop);

        Line&    m_parent;
        ISequencerTimerTool* m_timerTool;      ///< Pointer to the timer tool
        Algorithm*  m_algorithm;   ///< Algorithm pointer
        
        StringProperty  m_property;
        bool        m_reverse;     ///< Indicates that the flag has to be inverted
        bool        m_dirty;
        bool        m_initialized;
        int         m_timer;       ///< Timer number fo rthis algorithm
      };

    protected:
      // utilities for derived classes to use...
      const std::string& decisionName() { return m_decision; }
      // retrieve (recursive!) list of sub algorithms
      std::vector< const Algorithm* > algorithms() const ;

      IANNSvc&     annSvc() const;

    private:
      //TODO: move into DecReport...
      enum stage { initial=          0,  // i.e. did not pass 'prescale
                   prescaled =       1,  // i.e. did not pass 'seed'
                   odin      =       2,  // i.e. did not pass 'filter0'
                   l0du      =       3,  // i.e. did not pass 'filter1'
                   hlt       =       4,  // i.e. did not pass 'filter2'
                   filter1ed =       5,  // i.e. did not pass 'filter3'
                   filter2ed =       6,  // i.e. did not pass 'postscale'
                   postscaled =      7 ,
                   nStages = postscaled };
      const std::string& transition( const stage &s) const {
            static std::vector<std::string> s_map;
            if (s_map.empty()) {
                s_map.push_back("Prescale");
                s_map.push_back("ODIN");
                s_map.push_back("L0DU");
                s_map.push_back("HLT");
                s_map.push_back("Filter0");
                s_map.push_back("Filter1");
                s_map.push_back("Postscale");
            }
            return s_map[s];
      };
      
      typedef std::vector<std::pair<const Algorithm*,unsigned> > SubAlgos;
      SubAlgos retrieveSubAlgorithms() const;

      /** Decode a vector of string. */
      StatusCode decodeNames(  );

      Algorithm* getSubAlgorithm(const std::string& name);

      
      /** Private copy, assignment operator. This is not allowed **/
      Line( const Line& a );
      Line& operator=( const Line& a );
      
      
      boost::array<Stage*,nStages> m_stages; ///< List of algorithms to process.
      SubAlgos          m_subAlgo;        ///< list of subalgorithms and their sub-count
      ISequencerTimerTool* m_timerTool;      ///< Pointer to the timer tool
      IJobOptionsSvc* m_jos;                 ///< Pointer to job options service
      IAlgManager* m_algMgr;                 ///< Pointer to algorithm manager
      AIDA::IHistogram1D* m_errorHisto;
      AIDA::IHistogram1D *m_timeHisto;
      static const double m_timeHistoLowerBound = -3;
      
      AIDA::IHistogram1D *m_stepHisto;
      AIDA::IProfile1D *m_candHisto;
      mutable IANNSvc *m_hltANNSvc;
      StatEntity *m_acceptCounter;
      StatEntity *m_errorCounter;
      StatEntity *m_slowCounter;
      std::string m_outputContainerName;
      std::string m_decision;
      std::string s_ANNSvc;
      std::vector<std::string> m_incidents;  ///< Incidents to be flagged in HltDecReport if they occurs during processing
      bool m_ignoreFilter;                   ///< True if one continues always.
      bool m_measureTime;                    ///< Flag to measure time
      bool m_returnOK;                       ///< Forces the sequencer to return a good status
      bool m_acceptOnError;                  ///< Forces accept if error
      bool m_acceptOnIncident;               ///< Forces accept if incident
      bool m_acceptIfSlow;                   ///< Forces accept if event is slow
      bool m_caughtIncident;                 
      int  m_timer;                          ///< Timer number for the sequencer
      int  m_maxAcceptOnError;               ///< quota to avoid runaway accepts in case of persistent errors..
      int  m_nAcceptOnError;
      unsigned m_slowThreshold;
    };
}
#endif // Line_H

// $Id: SelectionLine.h,v 1.2 2010-01-18 15:44:53 graven Exp $
#ifndef SELECTION_LINE_H
#define SELECTION_LINE_H 1

// Include files
// from Gaudi
#include "Kernel/IANNSvc.h"
#include "GaudiKernel/IIncidentListener.h"
#include "GaudiAlg/GaudiHistoAlg.h"
#include <array>
#include <memory>

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
      
      ~Line() override = default; ///< Destructor

      StatusCode initialize() override;    ///< Algorithm initialization
      StatusCode execute   () override;    ///< Algorithm execution

      void handle(const Incident&) override;

      // id for this line -- return 0 if error
      virtual const std::pair<std::string,unsigned int>& id() const = 0; 
      // retrieve function which provides the numberOfCandidates for subalgo -- or nullptr if not supported
      virtual std::unique_ptr<std::function<unsigned()>> numberOfCandidates(const Algorithm*) const;
      // retrieve numberOfCandidates from decision algo 
      virtual int numberOfCandidates() const = 0;


      class Stage {
      public:
        Stage(Line& parent, const std::string& name)
        : m_parent(parent)
        , m_property(name, std::string{})
        {  m_property.declareUpdateHandler(&Stage::updateHandler,this); }

        Stage( Stage& rhs )
        : m_parent(rhs.m_parent)
        , m_timerTool(rhs.m_timerTool) 
        , m_property( rhs.m_property )
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
        ISequencerTimerTool* m_timerTool = nullptr;      ///< Pointer to the timer tool
        Algorithm*  m_algorithm = nullptr;   ///< Algorithm pointer
        
        StringProperty  m_property;
        bool        m_reverse = false;     ///< Indicates that the flag has to be inverted
        bool        m_dirty = true;
        bool        m_initialized = false;
        int         m_timer;       ///< Timer number for this algorithm
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
                   hlt1      =       5,  // i.e. did not pass 'filter2'
                   hlt2      =       6,  // i.e. did not pass 'filter2'
                   filter1ed =       7,  // i.e. did not pass 'filter3'
                   filter2ed =       8,  // i.e. did not pass 'postscale'
                   postscaled =      9 ,
                   nStages = postscaled };
      const std::string& transition( const stage &s) const {
            static const std::vector<std::string> s_map = { 
                {"Prescale"}, {"ODIN"}, {"L0DU"}, {"HLT"}, {"HLT1"}, {"HLT2"},
                {"Filter0"}, {"Filter1"}, {"Postscale"} } ;
            return s_map[s];
      };
      
      typedef std::vector<std::tuple<const Algorithm*,
                                     unsigned,
                                     std::unique_ptr<std::function<unsigned()>>>> SubAlgos;
      SubAlgos retrieveSubAlgorithms() const;

      /** Decode a vector of string. */
      StatusCode decodeNames(  );

      Algorithm* getSubAlgorithm(const std::string& name);

      
      /** copy, assignment is not allowed **/
      Line( const Line& a ) = delete;
      Line& operator=( const Line& a ) = delete;
      
      
      std::array< std::unique_ptr<Stage>, nStages > m_stages; ///< List of algorithms to process.
      SubAlgos          m_subAlgo;        ///< list of subalgorithms and their sub-count
      ISequencerTimerTool* m_timerTool;      ///< Pointer to the timer tool
      IJobOptionsSvc* m_jos;                 ///< Pointer to job options service
      IAlgManager* m_algMgr;                 ///< Pointer to algorithm manager
      AIDA::IHistogram1D* m_errorHisto;
      AIDA::IHistogram1D *m_timeHisto;
      AIDA::IHistogram1D *m_ncandHisto;
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
      bool m_turbo;                          ///< Line is a 'turbo' line -- flag this in decreport
      int  m_timer;                          ///< Timer number for the sequencer
      int  m_maxAcceptOnError;               ///< quota to avoid runaway accepts in case of persistent errors..
      int  m_nAcceptOnError;
      unsigned m_slowThreshold;
      size_t m_insertion_hint = -1;
    };
}
#endif // Line_H

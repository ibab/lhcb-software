#ifndef HLTOUTPUTSEQUENCE_H
#define HLTOUTPUTSEQUENCE_H 1

// Include files
// from Gaudi
#include <GaudiAlg/GaudiHistoAlg.h>
#include <GaudiAlg/ISequencerTimerTool.h>

/** @class HltOutputSequence HltOutputSequence.h
 *
 *
 *  @author Roel Aaij
 *  @date   2016-03-26
 */
class HltOutputSequence : public GaudiHistoAlg {
public:
   /// Standard constructor
   HltOutputSequence(const std::string& name, ISvcLocator* svcLocator);

   virtual ~HltOutputSequence( ) = default;

   StatusCode initialize() override;
   StatusCode execute() override;
   StatusCode finalize() override;

private:

   std::string m_writerType;
   bool m_outputPerGroup;
   bool m_ignoreFilters;
   std::map<std::string, std::vector<std::string>> m_forcedStreams;

   using PropMap = std::map<std::string, std::string>;
   PropMap m_writerProperties;
   PropMap m_streamProperties;
   PropMap m_groupProperties;

   // Get the member algorithms
   StatusCode decodeNames();

   class AlgorithmEntry final {
   public:

      AlgorithmEntry(Algorithm* alg, std::vector<SmartIF<IAlgorithm>>&& filters)
         : m_algorithm{alg}, m_filters{std::move(filters)} { }

      const std::vector<SmartIF<IAlgorithm>>& filters() const {
         return m_filters;
      }

      std::string type() const { return m_algorithm->type(); }
      Algorithm* algorithm() const { return m_algorithm; }

   private:

      Algorithm* m_algorithm = nullptr;
      std::vector<SmartIF<IAlgorithm>> m_filters;

   };

   std::vector<AlgorithmEntry> m_entries;

};
#endif // HLTOUTPUTSEQUENCE_H

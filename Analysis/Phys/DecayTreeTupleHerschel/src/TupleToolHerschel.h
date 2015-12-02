#ifndef TUPLETOOLHERSCHEL_H
#define TUPLETOOLHERSCHEL_H 1

// Include files
// from Gaudi
#include "Kernel/IEventTupleTool.h"            // Interface
#include "DecayTreeTupleBase/TupleToolBase.h"

/** @class TupleToolHerschel TupleToolHerschel.h
 *
 *  Fills Herschel information.
 *
 *  @author Dan Johnson
 *  @date   2015-07-08
 */
class TupleToolHerschel : public TupleToolBase, 
                           virtual public IEventTupleTool
{

public:

  /// Standard constructor
  TupleToolHerschel( const std::string& type,
                      const std::string& name,
                      const IInterface* parent);
  virtual StatusCode initialize();///< Algorithm initialization
  virtual ~TupleToolHerschel( ); ///< Destructor
  virtual StatusCode fill( Tuples::Tuple& );///< Fill tuple
  
protected:
  /// Digits location
  std::string m_DigitsLocation;
  /// Conditions database information
  Condition* m_cond;
  /// Number of B-side crate
  unsigned int m_crateB;
  /// Number of F-side crate
  unsigned int m_crateF;
  /// Channel mappings
  std::vector<int> m_masksB0;
  std::vector<int> m_masksB1;
  std::vector<int> m_masksB2;
  std::vector<int> m_masksF1;
  std::vector<int> m_masksF2;

  std::vector<int> m_channelsB0;
  std::vector<int> m_channelsB1;
  std::vector<int> m_channelsB2;
  std::vector<int> m_channelsF1;
  std::vector<int> m_channelsF2;

  std::vector<int> m_sparesB0;
  std::vector<int> m_sparesB1;
  std::vector<int> m_sparesB2;
  std::vector<int> m_sparesF1;
  std::vector<int> m_sparesF2;

  /// Channel numbers for each quadrant
  std::vector<std::vector<unsigned int> > m_channels;
  /// Masked flags for each quadrant
  std::vector<std::vector<bool> > m_masked;
  /// Retrieve the mapping from the conditions database.
  StatusCode cacheMapping();
  /// Setup the mapping for a given station.
  bool mapChannels(const std::vector<int>& channels,
                   const std::vector<int>& refs,
                   const std::vector<int>& masks,
                   const unsigned int station, const bool bwd);
  /// Print the channel mapping for information.
  void printMapping() const;  
};

#endif 


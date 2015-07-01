// $Id: $
#ifndef SIMPLETRENDWRITER_H
#define SIMPLETRENDWRITER_H 1

// Include files
// from Gaudi
#include "GaudiAlg/GaudiTool.h"
#include "Trending/ISimpleTrendWriter.h"            // Interface
#include "Trending/ITrendingTool.h"

/** @class SimpleTrendWriter SimpleTrendWriter.h
 *
 *
 *  @author Olivier Callot
 *  @date   2011-03-24
 */
class SimpleTrendWriter : public GaudiTool, virtual public ISimpleTrendWriter {
public:
  /// Standard constructor
  SimpleTrendWriter( const std::string& type,
                     const std::string& name,
                     const IInterface* parent);

  virtual ~SimpleTrendWriter( ); ///< Destructor

  virtual StatusCode initialize();

  virtual void setPartitionAndName( std::string& partition, std::string& name );

  virtual void startEvent();

  virtual void addEntry( std::string tag, double value );

  virtual void setAverageTime( unsigned int seconds );

  virtual void setMaxTimeNoWrite( unsigned int seconds );

  virtual void saveEvent();

  virtual void close();

protected:

private:
  ITrendingTool* m_trend;
  std::string m_fileName;
  bool m_fileIsOpen;
  bool m_tagChanged;
  std::vector<std::string> m_tags;
  std::vector<float>       m_values;
};
#endif // SIMPLETRENDWRITER_H

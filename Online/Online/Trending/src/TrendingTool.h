// $Id: TrendingTool.h,v 1.1.1.1 2010-06-11 09:58:56 ocallot Exp $
#ifndef TRENDINGTOOL_H 
#define TRENDINGTOOL_H 1

// Include files
// from Gaudi
#include "GaudiAlg/GaudiTool.h"
#include "Trending/ITrendingTool.h"            // Interface
#include "Trending/TrendWriter.h"            // Tool for writing

/** @class TrendingTool TrendingTool.h
 *  
 *
 *  @author Olivier Callot
 *  @date   2010-06-08
 */
class TrendingTool : public GaudiTool, virtual public ITrendingTool {
public: 

#include "Trending/TrendParams.h"

  /// Standard constructor
  TrendingTool( const std::string& type, 
                const std::string& name,
                const IInterface* parent);

  virtual ~TrendingTool( ); ///< Destructor

  bool openWrite( std::string name, std::vector<std::string> tags );

  bool setThresholds( std::vector<float> thresholds );
  
  bool setThreshold( std::string tag, float thr );
  
  void setAverageTime( unsigned int seconds );

  void setMaxTimeNoWrite( unsigned int seconds );

  void addValue( std::string tag, float value );

  bool write( std::vector<float> values, unsigned int time = 0 );
  
  void closeFile();
  
  bool openRead( std::string name );

  bool tags( std::vector<std::string>& tags, int version = -1 );
  
  bool select( unsigned int startTime, unsigned int endTime, std::string tag );
  
  bool nextValue( unsigned int& time, float& value );

  bool nextEvent( unsigned int& time, std::vector<float>& data );
  
  std::string timeString( unsigned int time );

  int tagVersion();

  unsigned int firstTimeThisTag();

private:

  bool loadTags ( int wantedVersion );

  bool nextDataBlock( );

  bool getDataContaining ( unsigned int time );
  
  unsigned int unpackAnEvent ( );
  
private:
  TrendWriter              m_trendWriter;

  bool                     m_forWriting;
  Header                   m_tagHeader;
  std::vector<std::string> m_tags;
  std::vector<float>       m_thresholds;
  std::vector<float>       m_lastData;
  int                      m_nbTag;
  int                      m_nbMask;
  DirectoryRecord          m_dir;
  DataRecord               m_data;
  long                     m_firstDirAddress;
  long                     m_tagAddressInFile;
  long                     m_dirAddressInFile;
  long                     m_dataAddressInFile;
  FILE*                    m_file;
  int                      m_ptDir;
  int                      m_ptData;
  bool                     m_hasSelection;
  int                      m_requestedTagNumber;
  unsigned int             m_maxTime;
  int                      m_tagNumber;
  unsigned int             m_lastTime;
  unsigned int             m_firstTimeThisTag;
};
#endif // TRENDINGTOOL_H

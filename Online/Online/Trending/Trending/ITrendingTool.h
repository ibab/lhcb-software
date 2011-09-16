// $Id: ITrendingTool.h,v 1.1.1.1 2010-06-11 09:58:56 ocallot Exp $
#ifndef TRENDING_ITRENDINGTOOL_H
#define TRENDING_ITRENDINGTOOL_H 1

// Include files
// from STL
#include <string>

// from Gaudi
#include "GaudiKernel/IAlgTool.h"

static const InterfaceID IID_ITrendingTool ( "ITrendingTool", 2, 1 );

/** @class ITrendingTool ITrendingTool.h Trending/ITrendingTool.h
 *  This is the abstract interface for the trending tool.
 *  It has the public methods for writing and reading Trending files.
 *
 *  @author Olivier Callot
 *  @date   2010-06-08
 */
class ITrendingTool : virtual public IAlgTool {
public:

  // Return the interface ID
  static const InterfaceID& interfaceID() { return IID_ITrendingTool; }

  /**
   * Open a trend file for writing.
   * @param name     is the file name. It will be put in the standard Trend directory.
   * @param tags     is a vector of names of the variables to store.
   * @return false if a problem, message in the log file.
   */
  virtual bool openWrite( std::string name, std::vector<std::string> tags ) = 0;

  /**
   * Set the thresholds for storing new values. Default is no threshold
   * @param thresholds: Vector of thresholds, same size as the tags.
   * @return false if a problem, message in the log file.
   */
  virtual bool setThresholds( std::vector<float> thresholds ) = 0;

  /**
   * Set the threshold for a single tag.
   * @param tag : name of the tag to which a threshold should be applied
   * @param thr : value of teh threshold
   * @return fals in case of problems, message in the log file.
   */
  virtual bool setThreshold( std::string tag, float thr ) = 0;

  /**
   * Set the time onto which one average before writing an entry
   * @param seconds: size of the averaging interval in seconds.
   */
  virtual void setAverageTime( unsigned int seconds ) = 0;

  /**
   * Set the maximum time without saving to file
   * @param seconds: maximum time without writing to file.
   */
  virtual void setMaxTimeNoWrite( unsigned int seconds ) = 0;

  /**
   * Add the value of the specified tag to the average. Store if time has expired
   * @param tag   : name of the tag
   * @param value : value to be added for this tag
   */
  virtual void addValue( std::string tag, float value ) = 0;

  /**
   * Store an array of values at the specified time
   * @param values : Array of values, same size as the tag.
   * @param time : Time for these values. By default this is now.
   * @return false if a problem. Message in the log file.
   */
  virtual bool write( std::vector<float> values, unsigned int time = 0 ) = 0;

  /**
   * Close the file. This is done anyway in the destructor
   */
  virtual void closeFile() = 0;

  /**
   * Open a Trending file for reading. The file must exist.
   * @param name  The file name as specified for writing
   * @return false if a problem, message in the log file.
   */
  virtual bool openRead( std::string name ) = 0;

  /**
   * Returns the list of tags for the specified version, default to last version
   * @param tags    : The requested tags
   * @param version : Optional version number
   * @return false if a problem, message in the log file.
   */
  virtual bool tags( std::vector<std::string>& tags, int version = -1 ) = 0;

  /**
   * select the tag for which one wants to retive the values
   * @param startTime : Starting time
   * @param endTime   : End of time range, defaults to now
   * @param tag     : tag of the variable. Absent means no selection, use then nextBlock
   * @return false if a problem, message in the log file.
   */
  virtual bool select( unsigned int startTime, unsigned int endTime = 0, std::string tag = "" ) = 0;

  /**
   * returns in turns all the values for teh selected tag.
   * @param time : Time of the entry returned
   * @param value : value of the tag at this time
   * @return true as long as there is a new value.
   */
  virtual bool nextValue( unsigned int& time, float& value ) = 0;

  /**
   * returns in turns all the entries.
   * @param time : Time of the entry returned
   * @param value : vector of value at this time
   * @return true as long as there is a new value.
   */
  virtual bool nextEvent( unsigned int& time, std::vector<float>& values ) = 0;

  /**
   * returns a string containing the time in readable format
   * @param time : integer valeu
   * @return a string
   */
  virtual std::string timeString( unsigned int time ) = 0;

  /**
   * returns the version of the tags currently in use
   */
  virtual int tagVersion() = 0;

  /**
   * returns the first time for this tag version
   */
  virtual unsigned int firstTimeThisTag() = 0;

};
#endif // TRENDING_ITRENDINGTOOL_H

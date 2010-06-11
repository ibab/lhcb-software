// $Id: ITrendingTool.h,v 1.1.1.1 2010-06-11 09:58:56 ocallot Exp $
#ifndef TRENDING_ITRENDINGTOOL_H
#define TRENDING_ITRENDINGTOOL_H 1

// Include files
// from STL
#include <string>

// from Gaudi
#include "GaudiKernel/IAlgTool.h"

static const InterfaceID IID_ITrendingTool ( "ITrendingTool", 1, 0 );

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
   * @param version  defines the version of the list of tags used. If tgas are changed, a new version
   *                 should be specified.
   * @return false if a problem, message in the log file.
   */
  virtual bool openWrite( std::string name, std::vector<std::string> tags, int version = 1 ) = 0;

  /**
   * Set the thresholds for storing new values. Default is no threshold
   * @param thresholds: Vector of thresholds, same size as the tags.
   * @return false if a problem, message in the log file.
   */
  virtual bool setThresholds( std::vector<float> thresholds ) = 0;

  /*
   * Set the threshold for a single tag.
   * @param tag : name of the tag to which a threshold should be applied
   * @param thr : value of teh threshold
   * @return fals in case of problems, message in the log file.
   */
  virtual bool setThreshold( std::string tag, float thr ) = 0;

  /**
   * Store an array of values at the specified time
   * @param values : Array of values, same size as the tag.
   * @param time : Time for these values. By default this is now.
   * @return false if a problem. Message in the log file.
   */
  virtual bool write( std::vector<float> values, int time = 0 ) = 0;

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
  virtual bool select( int startTime, int endTime = 0, std::string tag = "" ) = 0;

  /**
   * returns in turns all the values for teh selected tag.
   * @param time : Time of the entry returned
   * @param value : value of the tag at this time
   * @return true as long as there is a new value.
   */
  virtual bool nextValue( int& time, float& value ) = 0;

  /**
   * returns in turns all the entries.
   * @param time : Time of the entry returned
   * @param value : vector of value at this time
   * @return true as long as there is a new value.
   */
  virtual bool nextEvent( int& time, std::vector<float>& values ) = 0;

  /**
   * returns a string containing the time in readable format
   * @param time : integer valeu
   * @return a string
   */
  virtual std::string timeString( int time ) = 0;

};
#endif // TRENDING_ITRENDINGTOOL_H

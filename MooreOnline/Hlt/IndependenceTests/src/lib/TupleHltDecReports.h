// $Id: $
#ifndef TUPLEHLTDECREPORTS_H 
#define TUPLEHLTDECREPORTS_H 1

// Include files
// from Gaudi
#include <GaudiAlg/GaudiTupleAlg.h>


/** @class TupleHltDecReports TupleHltDecReports.h
 *  Algorithm to write HltDecReports to a tuple
 *
 *  @author Roel Aaij
 *  @date   2010-03-23
 */
class TupleHltDecReports : public GaudiTupleAlg {
public: 

   /// Standard constructor
   TupleHltDecReports( const std::string& name, ISvcLocator* pSvcLocator );

   virtual ~TupleHltDecReports( ); ///< Destructor

   virtual StatusCode initialize();    ///< Algorithm initialization

   virtual StatusCode execute   ();    ///< Algorithm execution

   virtual StatusCode finalize  ();    ///< Algorithm finalization

private:

   // properties
   std::string m_reportLocation;

};
#endif // TUPLEHLTDECREPORTS_H

// $Id: TupleToolFillingScheme.h,v 1.7 2010-09-09 12:22:42 pkoppenb Exp $
#ifndef TUPLETOOLFILLINGSCHEME_H
#define TUPLETOOLFILLINGSCHEME_H 1

// Include files
// from Gaudi
#include "DecayTreeTupleBase/TupleToolBase.h"
#include "Kernel/IEventTupleTool.h"            // Interface

#include "Event/Track.h"
#include "Event/RecSummary.h"
#include "HltDAQ/IReportConvert.h"
#include "HltDAQ/ReportConvertTool.h"

// CondDB
#include "DetDesc/Condition.h"
#include "GaudiKernel/IDetDataSvc.h"


/** @class TupleToolFillingScheme TupleToolFillingScheme.h
 *
 *  Fills Reco stats, from RecSummary
 *
 *  @author Patrick Koppenburg, Fatima Soomro, Jibo He
 *  @date   2009-02-11
 */
class TupleToolFillingScheme : public TupleToolBase, 
                           virtual public IEventTupleTool
{

public:

  /// Standard constructor
  TupleToolFillingScheme( const std::string& type,
                      const std::string& name,
                      const IInterface* parent);

  virtual ~TupleToolFillingScheme( ); ///< Destructor
  virtual StatusCode fill( Tuples::Tuple& );///< Fill tuple
  virtual StatusCode initialize();
private:
  /// for converting objects in to summaries
  IReportConvert* m_conv;
  int getPreviousBunchCrossing();
  Condition * 	m_condFilling;
  StatusCode i_cacheFillingData();
  std::string m_lhcFS;
  std::string m_B1FillingScheme;
  std::string m_B2FillingScheme;
  int m_BXOffset;
};

#endif // TUPLETOOLFILLINGSCHEME_H


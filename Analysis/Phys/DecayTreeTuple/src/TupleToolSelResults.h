// $Id: TupleToolSelResults.h,v 1.2 2010-01-26 15:39:26 rlambert Exp $
#ifndef TUPLETOOLSELRESULTS_H 
#define TUPLETOOLSELRESULTS_H 1

// Include files
// from Gaudi
#include "DecayTreeTupleBase/TupleToolBase.h"
#include "Kernel/IEventTupleTool.h"            // Interface

/** @class TupleToolSelResults TupleToolSelResults.h
 *  
 *  Fill entries for a given list of algorithm names.
 *
 * @code
 * EventTuple.TupleToolSelResults.Selections = [ "Hlt2", "Hlt2", "MySelection" ]
 * @endcode
 *
 * Warning : It will create fields "Hlt2", "Hlt2", "MySelection" in the Tuple. There might be clashes.
 *
 *  @author Patrick Koppenburg
 *  @date   2009-02-11
 */
class ICheckSelResults ;
class TupleToolSelResults : public TupleToolBase, virtual public IEventTupleTool {
public: 
  /// Standard constructor
  TupleToolSelResults( const std::string& type, 
                       const std::string& name,
                       const IInterface* parent);

  virtual ~TupleToolSelResults( ); ///< Destructor
  virtual StatusCode initialize(); ///< init
  virtual StatusCode fill( Tuples::Tuple& ); ///< Fill tuple

protected:

private:
  ICheckSelResults* m_selTool ; ///< CheckselResults tool
  std::vector<std::string> m_selections ; ///< list of algorithm names
  //std::string m_head ; ///< head, just in case there are clashes

};
#endif // TUPLETOOLSELRESULTS_H

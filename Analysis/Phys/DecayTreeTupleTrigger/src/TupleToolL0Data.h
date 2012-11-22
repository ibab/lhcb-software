// $Id: $
#ifndef TUPLETOOLL0DATA_H
#define TUPLETOOLL0DATA_H 1

// Include files
// from Gaudi
#include "DecayTreeTupleBase/TupleToolBase.h"
// Interfaces
#include "Kernel/IEventTupleTool.h"
#include "CaloUtils/CaloAlgUtils.h"
#include "L0Interfaces/IL0DUFromRawTool.h"
#include "L0Interfaces/IL0DUConfigProvider.h"
#include "L0Interfaces/IL0DUEmulatorTool.h"

/** @class TupleToolL0Data TupleToolL0Data.h
 *
 *
 *  @author Olivier Deschamps
 *  @date   2011-06-08
 */
class TupleToolL0Data : public TupleToolBase,
                        virtual public IEventTupleTool
{

public:

  /// Standard constructor
  TupleToolL0Data( const std::string& type,
                   const std::string& name,
                   const IInterface* parent );

  virtual ~TupleToolL0Data( ); ///< Destructor
  virtual StatusCode fill(Tuples::Tuple& );
  virtual StatusCode initialize();

private:

  std::string rename(const std::string& name);

private:

  std::vector<std::string> m_list;
  IL0DUFromRawTool* m_l0;
  IL0DUConfigProvider* m_l0conf;
  IL0DUEmulatorTool* m_l0emu;
  bool m_scale;
  std::vector<std::string> m_tcks;

};

#endif // TUPLETOOLL0DATA_H

// $Id: RootNTupleCnv.h,v 1.3 2010/08/24 23:30:32 frankb Exp $
//====================================================================
// NTuple converter class definition
//--------------------------------------------------------------------
//
//  Author     : M.Frank
//
//====================================================================
#ifndef GAUDIROOTCNV_ROOTNTUPLECNV_H
#define GAUDIROOTCNV_ROOTNTUPLECNV_H 1

// Include files
#include "SerializeStatCnv.h"

// Forward declarations
class INTuple;

/*
 *  Gaudi namespace declaration
 */
namespace Gaudi {

  /** @class SerializeNTupleCnv SerializeNTupleCnv.h GaudiSerialize/SerializeNTupleCnv.h
   *
   * NTuple converter class definition for NTuples writted/read using ROOT.
   *
   * Description:
   * Definition of the generic converter for row wise and column wise
   * Ntuples.
   *
   * @author  M.Frank
   * @version 1.0
   */
  class GAUDI_API SerializeNTupleCnv: public SerializeStatCnv {
  protected:

    /// Update the transient object: NTuples end here when reading records
    //StatusCode i__updateObjRoot(SerializeAddress* rpA, INTuple* tupl, TTree* tree, SerializeDataConnection* con);

  public:
    /// Standard constructor
    /**
     * @param clid        [IN]  Class ID of the object type to be converted.
     * @param db          [IN]  Pointer to POOL database interface
     * @param svc         [IN]  Pointer to Gaudi service locator
     *
     * @return Reference to SerializeNTupleCnv object
     */
    SerializeNTupleCnv(long typ, const CLID& clid, ISvcLocator* svc)
      : SerializeStatCnv(typ, clid, svc)      {}

    /// Standard destructor
    virtual ~SerializeNTupleCnv() {}

    /** Converter overrides: Convert the transient object
     * to the requested representation.
     *
     * @param pObj        [IN]  Valid pointer to DataObject
     * @param refpAdd     [OUT] Location to store pointer to the
     *                          object's opaque address
     *
     * @return Status code indicating success or failure
     */
    virtual StatusCode createRep(   DataObject* pObj,IOpaqueAddress*& refpAdd);

    /// Resolve the references of the converted object.
    /**
     * @param pAdd       [IN]   Valid pointer to the object's opaque address
     * @param pObj       [IN]   Valid pointer to DataObject
     *
     * @return Status code indicating success or failure
     */
    virtual StatusCode fillRepRefs( IOpaqueAddress* pAdd,DataObject* pObj);

    /** Converter overrides: Update the references of an
     * updated transient object.
     *
     * @param pAddress    [IN]  Valid pointer to the object's opaque address
     * @param refpObject [OUT]  Location to store pointer to DataObject
     *
     * @return Status code indicating success or failure
     */
    virtual StatusCode createObj(   IOpaqueAddress*   pAddress,
				    DataObject*&      refpObject);

    /** Converter overrides: Update the references of an
     * updated transient object.
     *
     * @param pAddress   [IN]   Valid pointer to the object's opaque address
     * @param pObject    [IN]   Valid pointer to DataObject
     *
     * @return Status code indicating success or failure
     */
    virtual StatusCode fillObjRefs( IOpaqueAddress*  /* pAddress */,
				    DataObject*      /* pObject  */)
      {  return StatusCode::SUCCESS;                                      }

    /** Update the transient object: NTuples end here when reading records
     *
     * @param pAddress   [IN]   Valid pointer to the object's opaque address
     * @param pObject    [IN]   Valid pointer to DataObject
     *
     * @return Status code indicating success or failure
     */
    virtual StatusCode updateObj(   IOpaqueAddress*   pAddress,
				    DataObject*       pObject);

    /** Converter overrides: Update the references of an
     * updated transient object.
     *
     * @param pAddress   [IN]   Valid pointer to the object's opaque address
     * @param pObject    [IN]   Valid pointer to DataObject
     *
     * @return Status code indicating success or failure
     */
    virtual StatusCode updateObjRefs( IOpaqueAddress* /* pAddress */,
				      DataObject*     /* pObject  */ )
      {  return StatusCode::SUCCESS;                                      }

    /** Converter overrides: Update the converted representation
     * of a transient object.
     *
     * @param pAddress   [IN]   Valid pointer to the object's opaque address
     * @param pObject    [IN]   Valid pointer to DataObject
     *
     * @return Status code indicating success or failure
     */
    virtual StatusCode updateRep(     IOpaqueAddress* /* pAddress */,
				      DataObject*     /* pObject  */)
      {  return StatusCode::SUCCESS;                                      }

    /** Converter overrides: Update the converted representation of a
     * transient object.
     *
     * @param pAddress   [IN]   Valid pointer to the object's opaque address
     * @param pObject    [IN]   Valid pointer to DataObject
     *
     * @return Status code indicating success or failure
     */
    virtual StatusCode updateRepRefs( IOpaqueAddress* /* pAddress */,
				      DataObject*     /* pObject  */ )
      {  return StatusCode::SUCCESS;                                      }
  };
}         // End namespace Gaudi

#endif    // GAUDIROOTCNV_ROOTNTUPLECNV_H
// $Id: SerializeNTupleCnv.cpp,v 1.13 2010/09/27 15:43:53 frankb Exp $
//------------------------------------------------------------------------------
//
// Implementation of class :  SerializeNTupleCnv
//
// Author :                   Markus Frank
//
//------------------------------------------------------------------------------

#define ALLOW_ALL_TYPES
// Include files
#include "GaudiSerialize/SerializeAddress.h"
//#include "SerializeNTupleCnv.h"

#include "GaudiKernel/NTuple.h"
#include "GaudiKernel/SmartIF.h"
#include "GaudiKernel/SmartRef.h"
#include "GaudiKernel/INTupleSvc.h"
#include "GaudiKernel/IRegistry.h"
#include "GaudiKernel/CnvFactory.h"
#include "GaudiKernel/SmartDataPtr.h"
#include "GaudiKernel/StreamBuffer.h"
#include "Event/RawEvent.h"

#include <limits>
#include <memory>

#define S_OK   StatusCode::SUCCESS
#define S_FAIL StatusCode::FAILURE

using namespace Gaudi;
using namespace LHCb;
using namespace std;

DECLARE_SERIALIZE_CNV_FACTORY(SerializeNTupleCnv, CLID_RowWiseTuple)
DECLARE_SERIALIZE_CNV_FACTORY(SerializeNTupleCnv, CLID_ColumnWiseTuple)

static inline istream& loadLong(istream& is)    {
  long i;
  is >> i;
  return is;
}
static inline istream& operator>>(istream& is, IOpaqueAddress*& /*pObj*/)
{  return loadLong(is);          }

template<class TYP> static
StatusCode createItem (INTuple* tuple, istream& is,const string& name,const TYP& null)  {
  string idxName;
  long len, ndim, dim[4], hasIdx, idxLow, idxLen;
  long dim1 = 1, dim2 = 1;
  INTupleItem* it = 0;
  char c;
  is >> len    >> c
    >> ndim   >> c
    >> hasIdx >> c;
  if ( hasIdx )  {
    getline(is, idxName, ';') >> idxLow >> c >> idxLen >> c;
  }
  for ( int i = 0; i < ndim; i++ )
    is >> dim[i] >> c;

  TYP low = null, high = null;
  is >> low >> c >> high >> c;
  is >> c;
  switch( ndim )
  {
  case 0:
    it = NTuple::_Item<TYP>::create (tuple, name, typeid(TYP), low, high, null);
    break;
  case 1:
    dim1 = (hasIdx) ? idxLen : dim[0];
    it = NTuple::_Array<TYP>::create (tuple,
      name,
      typeid(TYP),
      idxName,
      dim1,
      low,
      high,
      null);
    break;
  case 2:
    dim1 = (hasIdx) ? idxLen : dim[0];
    dim2 = (hasIdx) ? dim[0] : dim[1];
    it = NTuple::_Matrix<TYP>::create ( tuple,
      name,
      typeid(TYP),
      idxName,
      dim1,
      dim2,
      low,
      high,
      null);
    break;
  default:
    return S_FAIL;
  }
  return tuple->add(it);
}

template <class T> static inline void putRange(ostream& os, NTuple::_Data<T>* it)   {
  const NTuple::Range<T>& x = it->range();
  os << x.lower() << ';' << x.upper() << ';';
}

#if 0
/// Debug dump ....
static inline string _tr(const string& s) {
  string local = s;
  char* p = (char*)local.c_str();
  if ( strncmp(p,"<local>",7)==0 ) p += 7;
  for(;*p;++p)
    if ( !isalnum(*p) ) *p = '_';
  return local;
}
#endif

// Converter overrides: Update the references of an updated transient object.
StatusCode
SerializeNTupleCnv::createObj(IOpaqueAddress* pAddr, DataObject*& refpObject)   {
  StatusCode status = S_FAIL;
  const string* par = pAddr->par();
  if ( !par[0].empty() )      {
    SmartIF<INTupleSvc> ntupleSvc(dataProvider());
    if ( ntupleSvc.isValid() )  {
      char c;
      CLID clid;
      int siz, typ;
      string title;
      NTuple::Tuple* nt = 0;
      istringstream is(par[0]);
      getline(is, title, ';') >> clid >> c >> siz >> c;
      status = ntupleSvc->create(clid, title, nt);
      for ( int j = 0; j < siz && status.isSuccess(); j++ ) {
	is >> c;
	getline(is, title, ';') >> typ >> c;
	switch ( typ )
          {
          case DataTypeInfo::UCHAR:
            status = createItem(nt, is, title, (unsigned char)0);
            break;
          case DataTypeInfo::USHORT:
            status = createItem(nt, is, title, (unsigned short)0);
            break;
          case DataTypeInfo::UINT:
            status = createItem(nt, is, title, (unsigned int)0);
            break;
          case DataTypeInfo::ULONG:
            status = createItem(nt, is, title, (unsigned long)0);
            break;
          case DataTypeInfo::CHAR:
            status = createItem(nt, is, title, char(0));
            break;
          case DataTypeInfo::SHORT:
            status = createItem(nt, is, title, short(0));
            break;
          case DataTypeInfo::INT:
            status = createItem(nt, is, title, int(0));
            break;
          case DataTypeInfo::LONG:
            status = createItem(nt, is, title, long(0));
            break;
          case DataTypeInfo::BOOL:
            status = createItem(nt, is, title, false);
            break;
          case DataTypeInfo::FLOAT:
            status = createItem(nt, is, title, float(0.0));
            break;
          case DataTypeInfo::DOUBLE:
            status = createItem(nt, is, title, double(0.0));
            break;
            /*
	      case DataTypeInfo::NTCHAR:
	      case DataTypeInfo::LONG_NTCHAR:
	      status =
	      createItem(nt, is, title, (char*)0);
	      break;
	      case DataTypeInfo::STRING:
	      case DataTypeInfo::LONG_STRING:
	      status =
	      createItem(nt, is, title, string(""));
	      break;
            */
          case DataTypeInfo::OBJECT_ADDR:
            status = createItem(nt, is, title, (IOpaqueAddress*)0);
            break;
          case DataTypeInfo::POINTER:
            status = createItem(nt, is, title, (void*)0);
            break;
          case DataTypeInfo::UNKNOWN:
          default:
            status = S_FAIL;
            break;
          }
	if ( !status.isSuccess() )  {
	  log() << MSG::FATAL
		<< "Error connecting (Unknown) column:" << j << endmsg
		<< par[0] << endmsg;
	  return makeError("createObj[NTuple]> Cannot determine column!");
	}
      }
      if ( status.isSuccess() )  {
	refpObject  = nt;
      }
      else {
	refpObject = 0;
	if ( nt ) nt->release();
      }
    }
  }
  return status;
}

// Update the transient object: NTuples end here when reading records
StatusCode SerializeNTupleCnv::updateObj(IOpaqueAddress* pAddr, DataObject* pObj)  {
  INTuple* tupl = dynamic_cast<INTuple*>(pObj);
  SerializeAddress* rpA = dynamic_cast<SerializeAddress*>(pAddr);
  if ( 0 != tupl && 0 != rpA )  {
#if 0

    SerializeDataConnection* con = (SerializeDataConnection*)rpA->ipar()[0];
    if ( con )   {
      TTree* tree = rpA->section;
      if ( tree ) {
        con->resetAge();
        if ( con->tool()->refs() )
          return i__updateObjRoot(rpA,tupl,tree,con);
#ifdef __POOL_COMPATIBILITY
        // POOL compatibility mode:
        return i__updateObjPool(rpA,tupl,tree,con);
#else
        return makeError("Failed to access reference branch for data tree:"+rpA->par()[1]);
#endif
      }
      return makeError("Failed to access data tree:"+pAddr->par()[1]);
    }
    return makeError("updateObj> Failed to access data source!");
  }
  return makeError("updateObj> Invalid Tuple reference.");
}

// Update the transient object: NTuples end here when reading records
StatusCode SerializeNTupleCnv::i__updateObjSerialize(SerializeAddress* rpA, INTuple* tupl, TTree* tree, SerializeDataConnection* con)  {
  typedef INTuple::ItemContainer Cont;
  const string*   par = rpA->par();
  unsigned long* ipar = const_cast<unsigned long*>(rpA->ipar());
  ++ipar[1];
  if ( Long64_t(ipar[1]) <= tree->GetEntries() ) {
    GenericAddress* pA = 0;
    Cont& it = tupl->items();
    size_t k, n = it.size();
    vector<SerializeRef*> paddr(n);
    vector<SerializeRef>  addr(n);
    for(k = 0; k < n; ++k)      {
      Cont::value_type j = it[k];
      switch( j->type() )
      {
      case DataTypeInfo::OBJECT_ADDR:
        paddr[k] = &addr[k];
        tree->SetBranchAddress(j->name().c_str(),&paddr[k]);
        break;
      default:
        break;
      }
    }

    ULong64_t last = (ULong64_t)tree->GetEntries();
    ISelectStatement* sel = tupl->selector();
    if ( sel ) {
      string criteria = (sel && (sel->type() & ISelectStatement::STRING))
        ? sel->criteria() : string("");
      if ( !(criteria.length() == 0 || criteria == "*") )  {
        if ( rpA->select == 0 ) {
          log() << MSG::DEBUG << "Selection criteria: " << criteria << "  "  << ipar[1] << endmsg;
          rpA->select = new TTreeFormula(tree->GetName(),criteria.c_str(), tree);
        }
        rpA->select->SetTree(tree);
        for( ; ipar[1] < last; ++ipar[1]) {	// loop on all selected entries
          tree->LoadTree(ipar[1]);
          rpA->select->GetNdata();
          if ( fabs(rpA->select->EvalInstance(0)) > std::numeric_limits<float>::epsilon() ) {
            break;
          }
          log() << MSG::DEBUG << par[0] << "/" << par[1] << " SKIP Entry: " << ipar[1] << endmsg;
        }
      }
    }
    if ( ipar[1] < last ) {
      unsigned long entry = ipar[1];
      if ( tree->GetEntry(entry) > 1 )   {
        SerializeRef *r = 0;
        string  *spar = 0;
        for(k = 0; k < n; ++k)      {
          Cont::value_type j = it[k];
          switch( j->type() )
          {
          case DataTypeInfo::OBJECT_ADDR:
            r = paddr[k];
            pA = (*(GenericAddress**)j->buffer());
            if ( pA ) { // Fill only if item is connected!
              spar = (string*)pA->par();
              ipar = (unsigned long*)pA->ipar();
              log() << MSG::DEBUG;
              pair<const SerializeRef*,const SerializeDataConnection::ContainerSection*> ls = con->getMergeSection(tree->GetName(),entry);
              if ( ls.first )  {
                if ( ls.first->dbase >= 0 ) {
                  // Now patch the references and links 'en block' to be efficient
                  // First the leafs from the TES

                  r->dbase     += ls.first->dbase;
                  r->container += ls.first->container;
                  r->link      += ls.first->link;

                  if ( log().isActive() ) {
                    log() << "Refs: LS [" << entry << "] -> "
                      << ls.first->dbase << "," << ls.first->container
                      << "," << ls.first->link
                      << "," << ls.first->entry
                      << " DB:" << con->getDb(r->dbase)
                      << endmsg;
                  }
                }
              }
              spar[0] = con->getDb(r->dbase);
              spar[1] = con->getCont(r->container);
              spar[2] = con->getLink(r->link);
              ipar[0] = 0;
              ipar[1] = r->entry;
              pA->setClID(r->clid);
              pA->setSvcType(r->svc);
              break;
            }
            break;
          default:
            break;
          }
        }
        return StatusCode::SUCCESS;
      }
      log() << MSG::ERROR << "Failed to read data from NTuple tree." << endmsg;
      return StatusCode::FAILURE;
    }
    log() << MSG::INFO << "End of input Ntuple." << endmsg;
#endif
    return StatusCode::FAILURE;
  }
  return StatusCode::FAILURE;
}

/// Convert the transient object to the requested representation.
StatusCode SerializeNTupleCnv::createRep(DataObject* pObj, IOpaqueAddress*& pAddr)  {
  IRegistry* pRegistry = pObj->registry();
  if ( 0 != pRegistry )  {
    pAddr = pRegistry->address();
    if ( 0 != pAddr ) {
      return S_OK;
    }
    const INTuple* nt = dynamic_cast<const INTuple*>(pObj);
    if ( 0 != nt )  {
      const INTuple::ItemContainer& items = nt->items();
      ostringstream os;
      size_t item_no;
      string desc;
      os << nt->title()<<';'<<pObj->clID()<<';'<<items.size()<< ';';

      for(item_no = 0; item_no < items.size(); ++item_no ) {
        INTupleItem* it = items[item_no];
        const string& n = it->name();
        os << '{'
          << n               << ';'
          << it->type()      << ';'
          << it->length()    << ';'
          << it->ndim()      << ';'
          << it->hasIndex()  << ';';
        if ( it->hasIndex() )   {
          os << it->index() << ';';
          INTupleItem* itm = it->indexItem();
          switch( itm->type() )     {
          case DataTypeInfo::UCHAR:
            putRange(os, dynamic_cast<NTuple::_Data<unsigned char>*>(itm));
            break;
          case DataTypeInfo::USHORT:
            putRange(os, dynamic_cast<NTuple::_Data<unsigned short>*>(itm));
            break;
          case DataTypeInfo::UINT:
            putRange(os, dynamic_cast<NTuple::_Data<unsigned int>*>(itm));
            break;
          case DataTypeInfo::ULONG:
            putRange(os, dynamic_cast<NTuple::_Data<unsigned long>*>(itm));
            break;
          case DataTypeInfo::CHAR:
            putRange(os, dynamic_cast<NTuple::_Data<char>*>(itm));
            break;
          case DataTypeInfo::SHORT:
            putRange(os, dynamic_cast<NTuple::_Data<short>*>(itm));
            break;
          case DataTypeInfo::INT:
            putRange(os, dynamic_cast<NTuple::_Data<int>*>(itm));
            break;
          case DataTypeInfo::LONG:
            putRange(os, dynamic_cast<NTuple::_Data<long>*>(itm));
            break;
          default:
            return makeError("NTuple:"+pRegistry->name()+" Column "+it->index()+
              " is not a valid index column!");
          }
        }
        for ( long k = 0; k < it->ndim(); k++ )  {
          os << it->dim(k) << ';';
        }
        switch(it->type())
        {
        case DataTypeInfo::STRING:
        case DataTypeInfo::NTCHAR:
        case DataTypeInfo::OBJECT_ADDR:
        case DataTypeInfo::POINTER:
	  os << 0 << ';' << 0 << ';';
          break;
        case DataTypeInfo::UCHAR:
          putRange(os, dynamic_cast<NTuple::_Data<unsigned char>*>(it));
          break;
        case DataTypeInfo::USHORT:
          putRange(os, dynamic_cast<NTuple::_Data<unsigned short>*>(it));
          break;
        case DataTypeInfo::UINT:
          putRange(os, dynamic_cast<NTuple::_Data<unsigned int>*>(it));
          break;
        case DataTypeInfo::ULONG:
          putRange(os, dynamic_cast<NTuple::_Data<unsigned long>*>(it));
          break;
        case DataTypeInfo::CHAR:
          putRange(os, dynamic_cast<NTuple::_Data<char>*>(it));
          break;
        case DataTypeInfo::SHORT:
          putRange(os, dynamic_cast<NTuple::_Data<short>*>(it));
          break;
        case DataTypeInfo::INT:
          putRange(os, dynamic_cast<NTuple::_Data<int>*>(it));
          break;
        case DataTypeInfo::LONG:
          putRange(os, dynamic_cast<NTuple::_Data<long>*>(it));
          break;
        case DataTypeInfo::BOOL:
          putRange(os, dynamic_cast<NTuple::_Data<bool>*>(it));
          break;
        case DataTypeInfo::FLOAT:
          putRange(os, dynamic_cast<NTuple::_Data<float>*>(it));
          break;
        case DataTypeInfo::DOUBLE:
          putRange(os, dynamic_cast<NTuple::_Data<double>*>(it));
          break;
        case DataTypeInfo::UNKNOWN:
        default:
          return makeError("Create item[FAIL]: "+it->name());
        }
        os << '}';
      }
      string spar[]   = { os.str(), pRegistry->identifier() };
      unsigned long ipar[] = { ~0x0u, 0x0u };
      log() << MSG::INFO << "NTUPLE:" << spar[1] << endmsg
	    << spar[0] << endmsg;
      StatusCode status = m_dbMgr->createAddress(repSvcType(),pObj->clID(),spar,ipar,pAddr);
      if ( !status.isSuccess() )  {
	pAddr->release();
	pAddr = 0;
      }
      return status;
    }
  }
  return S_FAIL;
}

// Resolve the references of the converted object.
StatusCode SerializeNTupleCnv::fillRepRefs(IOpaqueAddress* pAddr, DataObject* pObj)   {
  INTuple* tupl = dynamic_cast<INTuple*>(pObj);
  if ( tupl && pAddr )  {
    string top = topLevel(pObj->registry())+"/RawTupleData";
    SmartDataPtr<RawEvent> raw(dataProvider(),top);
    if ( 0 != raw ) {
      INTuple::ItemContainer& it = tupl->items();
      const string& id = pAddr->par()[1];
      unsigned long* ipar = const_cast<unsigned long*>(pAddr->ipar());
      size_t len, k, n = it.size();
      size_t   mx   = numeric_limits<unsigned short>::max()-sizeof(RawBank);
      RawBank* bank = raw->createBank(0, RawBank::GaudiSerialize, objType(), mx);
      auto_ptr<unsigned int> bankbuffer((unsigned int*)bank);
      char* start = bank->begin<char>(), *ptr = start+id.length()+1+sizeof(long);
      unsigned short* hdr = (unsigned short*)ptr;

      *(long*)start = ipar[1];
      memcpy(start+sizeof(long),id.c_str(),id.length()+1);
      ptr += sizeof(unsigned short)*(n+1);

      *hdr++ = n;
      for(k = 0; k < n; ++k)      {
	INTuple::ItemContainer::value_type j = it[k];
	const void* buff = j->buffer();
	switch(j->type())    {
	case DataTypeInfo::STRING:
	  buff = ((string*)buff)->c_str();
	  len = buff ? ((string*)buff)->length()+1 : 1;
	  break;
	case DataTypeInfo::NTCHAR:
	  len = buff ? ::strlen((const char*)buff) + 1 : 1;
	  break;
	case DataTypeInfo::UCHAR:
	case DataTypeInfo::USHORT:
	case DataTypeInfo::UINT:
	case DataTypeInfo::ULONG:
	case DataTypeInfo::CHAR:
	case DataTypeInfo::SHORT:
	case DataTypeInfo::INT:
	case DataTypeInfo::LONG:
	case DataTypeInfo::BOOL:
	case DataTypeInfo::FLOAT:
	case DataTypeInfo::DOUBLE:
	  len = j->size();
	  break;
	case DataTypeInfo::OBJECT_ADDR: {
	  len = 0;
	  if ( buff && *(void**)buff ) {
	    IOpaqueAddress* pA = *(IOpaqueAddress**)buff;
	    const string* sp = pA->par();
	    len = 2+sp[0].length()+sp[1].length()+2*sizeof(int);
	  }
	  break;
	}
	case DataTypeInfo::POINTER:
	  len = 0;
	  break;
	case DataTypeInfo::UNKNOWN:
	default:
	  return makeError("Cannot save NTuple item type [FAIL]: "+j->name()+" Cannot save data type");
	}

	log() << MSG::INFO << topLevel(pObj->registry()) << " "
	      << "Item[" << k << "]:" << j->name() << ":" << j->length() << " " << j->size() << " " << len
	      << "[" << j->type() << "] --> " << j->buffer() << endmsg;

	if ( (ptr-start)+len < mx ) {
	  *hdr++ = len;
	  if ( j->type() == DataTypeInfo::OBJECT_ADDR && len > 0 )  {
	    IOpaqueAddress* pA = *(IOpaqueAddress**)buff;
	    const unsigned long* ip = pA->ipar();
	    const string* sp = pA->par();
	    *(int*)ptr = int(ip[0]);
	    *(int*)(ptr+=sizeof(int)) = int(ip[1]);
	    ::strcpy(ptr+=sizeof(int),sp[0].c_str());
	    ::strcpy(ptr+=sp[0].length()+1,sp[1].c_str());
	    ptr+=sp[1].length()+1;
	  }
	  else {
	    ::memcpy(ptr,buff,len);
	    ptr += len;
	  }
	}
	else {
	  // NTuple too long!
	  return makeError("Cannot save NTuple [FAIL]: "+id+" Out of memory!");
	}
      }
      len = ptr - start;
      bank->setSize(len);
      raw->adoptBank(bank,true);
      bankbuffer.release();
      ++ipar[1];
      for(k = 0; k < n; ++k) it[k]->reset();
      return StatusCode::SUCCESS;
    }
    return makeError("Failed to access RAW data object in store:"+top);
  }
  return makeError("fillRepRefs> Invalid Tuple reference.");
}


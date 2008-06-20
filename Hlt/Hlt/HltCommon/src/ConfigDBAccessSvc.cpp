#include "ConfigDBAccessSvc.h"
#include <sstream>
#include <map>
#include <memory>
#include <utility>


#include "GaudiKernel/SvcFactory.h"


#include "RelationalAccess/IConnectionService.h"
#include "RelationalAccess/ISessionProxy.h"
#include "RelationalAccess/ITransaction.h"
#include "RelationalAccess/ITable.h"
#include "RelationalAccess/IQuery.h"
#include "RelationalAccess/IQueryDefinition.h"
#include "RelationalAccess/ICursor.h"
#include "RelationalAccess/ISchema.h"
#include "RelationalAccess/ITablePrivilegeManager.h"
#include "RelationalAccess/TableDescription.h"
#include "RelationalAccess/ITableDataEditor.h"
#include "RelationalAccess/IBulkOperation.h"

#include "CoralBase/AttributeListSpecification.h"
#include "CoralBase/AttributeList.h"
#include "CoralBase/Attribute.h"
#include "CoralBase/AttributeSpecification.h"
#include "CoralBase/TimeStamp.h"

namespace {
    template <typename T> struct id { T& operator()(T& t) { return t; } };
    template <typename T> struct to_str { std::string operator()(const T& t) { return t.str(); } };
    template <typename T, typename OP > std::string join(T i, const T& end, OP op) {
       static const std::string x("'");
       std::string out;
       while (i != end ) {
         out += ' ' + op(*i) + '\n';
         ++i;
       }
       return out;
    }
    template <typename T> std::string join(T i, const T& end) {
       static const std::string x("'");
       std::string out;
       while (i != end ) {
         out += x + i->first + "':" + i->second + '\n';
         ++i;
       }
       return out;
    }

    class Transaction {
        public:
            Transaction(coral::ISessionProxy& session,bool readonly=true) : m_trans(session.transaction()) { 
               if (!m_trans.isActive()) m_trans.start(readonly); }
            void abort() { m_trans.rollback(); }
            void commit() { m_trans.commit(); }
            ~Transaction() { if (m_trans.isActive())  m_trans.commit(); }
        private:
            coral::ITransaction& m_trans;
    };

    template <typename T> struct table_traits;

    template <>
    struct table_traits<PropertyConfig> {
        static const std::string& tableName() {
            static std::string s("PropertyConfigs"); return s; 
        }
        static void read(const coral::AttributeList& row, PropertyConfig& x) {
            std::stringstream ss;
            ss << "Name: " << row["Name"].data<std::string>() << '\n';
            ss << "Kind: " << row["Kind"].data<std::string>() << '\n';
            ss << "Type: " << row["Type"].data<std::string>() << '\n';
            ss << "Properties: [\n" << row["Properties"].data<std::string>() << "]\n";
            ss >> x;
        }
        static void write(const PropertyConfig& config, coral::AttributeList& row)  {
            row["Name"]          .setValue<std::string>( config.name());
            row["Kind"]          .setValue<std::string>( config.kind());
            row["Type"]          .setValue<std::string>( config.type());
            const PropertyConfig::Properties&  props = config.properties();
            row["Properties"]    .setValue<std::string>( join( props.begin(),props.end() ) );
        }
        static void addSchema(coral::TableDescription& table) {
            table.insertColumn( "Name",
                               coral::AttributeSpecification::typeNameForId( typeid(std::string) ),
                               128, false );
            table.setNotNullConstraint( "Name" );

            table.insertColumn( "Type",
                               coral::AttributeSpecification::typeNameForId( typeid(std::string) ),
                               128, false );
            table.setNotNullConstraint( "Type" );

            table.insertColumn( "Kind",
                               coral::AttributeSpecification::typeNameForId( typeid(std::string) ),
                               32, false );
            table.setNotNullConstraint( "Kind" );

            table.insertColumn( "Properties",
                               coral::AttributeSpecification::typeNameForId( typeid(std::string) ),
                               1024, false );

        }
    };

    template <>
    struct table_traits<ConfigTreeNode> {
        static const std::string& tableName() {
            static std::string s("ConfigTreeNodes"); return s; 
        }
        static void read(const coral::AttributeList& row, ConfigTreeNode& x) {
            std::stringstream ss;
            ss << "Label: " << row["Label"].data<std::string>() << '\n';
            ss << "Leaf: "  << row["Leaf"].data<std::string>() << '\n';
            ss << "Nodes: [\n" << row["Nodes"].data<std::string>() << "]\n";
            ss >> x;
        }
        static void write(const ConfigTreeNode& config, coral::AttributeList& row) {
            row["Label"]         .setValue<std::string>( config.label() );
            row["Leaf"]          .setValue<std::string>( config.leaf().str() );
            const ConfigTreeNode::NodeRefs&  nodes = config.nodes();
            row["Nodes"]         .setValue<std::string>( join( nodes.begin(),nodes.end(),to_str<ConfigTreeNode::digest_type>() ) );
        }
        static void addSchema(coral::TableDescription& table) {
            table.insertColumn( "Label",
                               coral::AttributeSpecification::typeNameForId( typeid(std::string) ),
                               128 );

            table.insertColumn( "Leaf",
                               coral::AttributeSpecification::typeNameForId( typeid(std::string) ),
                               32 );
        //    table.setNotNullConstraint( "Leaf" );

            table.insertColumn( "Nodes",
                               coral::AttributeSpecification::typeNameForId( typeid(std::string) ),
                               1024,
                               false );
        }
    };
}

// Factory implementation
DECLARE_SERVICE_FACTORY(ConfigDBAccessSvc)


//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
ConfigDBAccessSvc::ConfigDBAccessSvc( const std::string& name, ISvcLocator* pSvcLocator)
  : Service ( name , pSvcLocator )
  ,m_coolConfSvc(0)
{
  declareProperty("Connection", m_connection = "sqlite_file:/tmp/config.db");
  declareProperty("ReadOnly", m_readOnly = true );
  declareProperty("CreateSchema", m_createSchema = false);
}

//=============================================================================
// Destructor
//=============================================================================
ConfigDBAccessSvc::~ConfigDBAccessSvc() {}

//=============================================================================
// queryInterface
//=============================================================================
StatusCode ConfigDBAccessSvc::queryInterface(const InterfaceID& IID,
                                             void** iface) {
  if ( IConfigAccessSvc::interfaceID().versionMatch(IID) )   {
    *iface = (IConfigAccessSvc*)this;
    addRef();
    return StatusCode::SUCCESS;
  }
  return Service::queryInterface(IID,iface);
}

//=============================================================================
// Initialization
//=============================================================================
StatusCode ConfigDBAccessSvc::initialize() {
  info() << "Initialize" << endmsg;
  StatusCode sc = Service::initialize();
  if ( !sc.isSuccess() )   return sc;
  sc = setProperties();
  if ( !sc.isSuccess() )   return sc;


  sc = openConnection();
  if ( !sc.isSuccess() )   return sc;
  if ( m_createSchema )  {
       Transaction transaction(*m_session,false);
       createTable<PropertyConfig>();
       createTable<ConfigTreeNode>();
  }
  return sc;
}

//=============================================================================
// Finalization
//=============================================================================
StatusCode ConfigDBAccessSvc::finalize() {
  info() << "Finalize" << endmsg;
  if (m_coolConfSvc) { 
    m_coolConfSvc->release(); m_coolConfSvc=0;
  }
  return Service::finalize();
}


template <typename T>
boost::optional<T> 
ConfigDBAccessSvc::read(const typename T::digest_type& ref) {
    Transaction transaction(*m_session,true);
    coral::AttributeList condData;
    condData.extend<std::string>("ref");
    condData["ref"].setValue<std::string>(ref.str());
    coral::IQuery* q = m_session->nominalSchema().tableHandle( table_traits<T>::tableName() ).newQuery();
    q->setCondition( std::string(" id = :ref "), condData);
    q->limitReturnedRows(1); // we know we only get zero or one matches
    coral::ICursor& c = q->execute();
    T x;
    size_t  nrow(0);
    while ( c.next() ) {
        table_traits<T>::read(c.currentRow(),x);
        ++nrow;
    }
    return nrow==1 ? x : boost::optional<T>();
}


template <typename T>
typename T::digest_type
ConfigDBAccessSvc::write(const T& config) {
    // first check whether it is not already there!
    typename T::digest_type id = config.digest();
    boost::optional<T> present = read<T>(id);
    if (present) {
        assert(present.get() == config);
        return id;
    } 

    coral::ITransaction& trans = m_session->transaction();
    trans.start(false);
    coral::ITable& ctable = m_session->nominalSchema().tableHandle( table_traits<T>::tableName() );
    coral::AttributeList row;
    ctable.dataEditor().rowBuffer(row);
    row["id"]            .setValue<std::string>( id.str() );
    table_traits<T>::write(config,row);
    row["InsertionTime"] .setValue<coral::TimeStamp>( coral::TimeStamp::now() ); 
    ctable.dataEditor().insertRow( row );

    trans.commit();
    return id;
}

boost::optional<PropertyConfig>  
ConfigDBAccessSvc::readPropertyConfig(const PropertyConfig::digest_type& ref)
{ return read<PropertyConfig>(ref); }

PropertyConfig::digest_type     
ConfigDBAccessSvc::writePropertyConfig(const PropertyConfig& config)
{ return write<PropertyConfig>(config); }

boost::optional<ConfigTreeNode>  
ConfigDBAccessSvc::readConfigTreeNode(const ConfigTreeNode::digest_type& ref)
{ return read<ConfigTreeNode>(ref); }

ConfigTreeNode::digest_type     
ConfigDBAccessSvc::writeConfigTreeNode(const ConfigTreeNode& config)
{ return write<ConfigTreeNode>(config); }


MsgStream& ConfigDBAccessSvc::msg(MSG::Level level) const {
     if (m_msg.get()==0) m_msg.reset( new MsgStream( msgSvc(), name() ));
     return *m_msg << level;
}

template <typename T>
void ConfigDBAccessSvc::createTable() {
    coral::TableDescription tableDesc( std::string("Table_")+table_traits<T>::tableName() );
    tableDesc.setName( table_traits<T>::tableName() );
    // Define primary key
    tableDesc.insertColumn( "id",
                               coral::AttributeSpecification::typeNameForId( typeid(std::string) ),
                               32 );
    tableDesc.setPrimaryKey( "id" );
    tableDesc.setNotNullConstraint( "id" );

    table_traits<T>::addSchema(tableDesc);

    tableDesc.insertColumn( "InsertionTime",
                               coral::AttributeSpecification::typeNameForId( typeid(coral::TimeStamp) )
                               );

    // Create the actual table
    m_session->nominalSchema().dropIfExistsTable( tableDesc.name() );
    coral::ITable& table = m_session->nominalSchema().createTable( tableDesc );
    table.privilegeManager().grantToPublic( coral::ITablePrivilegeManager::Select );
}

StatusCode ConfigDBAccessSvc::openConnection() {
  if (m_coolConfSvc==0) {
    StatusCode sc = service("COOLConfSvc",m_coolConfSvc);
    if ( !sc.isSuccess() )   return sc;
  }

  m_session = m_coolConfSvc->connectionSvc().connect(m_connection, m_readOnly ? coral::ReadOnly : coral::Update );
//  seal::IHandle<coral::IConnectionService> iHandle =
//     m_coolConfSvc->context()->query<coral::IConnectionService>( "CORAL/Services/ConnectionService" );
//
//   m_session = iHandle->connect(m_connection, m_readOnly ? coral::ReadOnly : coral::Update );
   return StatusCode::SUCCESS;
}

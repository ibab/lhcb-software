#include "ConfigDBAccessSvc.h"
#include <sstream>
#include <map>
#include <memory>
#include <utility>


#include "GaudiKernel/SvcFactory.h"

#define GR_USE_SEAL
#ifdef GR_USE_SEAL
#include "SealKernel/Context.h"
#endif



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
}

template <>
struct ConfigDBAccessSvc::table_traits<PropertyConfig> {
    static const std::string& tableName() { static std::string s("PropertyConfigs"); return s; }
    static const std::string& keyName() { static std::string k("id"); return k; }
    typedef PropertyConfig::digest_type key_type;

    static key_type key(const PropertyConfig& x) { return x.digest(); }

    static PropertyConfig& read(const coral::AttributeList& row, PropertyConfig& x) {
        std::stringstream ss;
        ss << "Name: " << row["Name"].data<std::string>() << '\n';
        ss << "Kind: " << row["Kind"].data<std::string>() << '\n';
        ss << "Type: " << row["Type"].data<std::string>() << '\n';
        ss << "Properties: [\n" << row["Properties"].data<std::string>() << "]\n";
        ss >> x;
        return x;
    }
    static void write(const key_type& key, coral::AttributeList& row)  {
        row["id"]            .setValue<std::string>( key.str() );
    }
    static void write(const PropertyConfig& config, coral::AttributeList& row)  {
        row["Name"]          .setValue<std::string>( config.name());
        row["Kind"]          .setValue<std::string>( config.kind());
        row["Type"]          .setValue<std::string>( config.type());
        const PropertyConfig::Properties&  props = config.properties();
        row["Properties"]    .setValue<std::string>( join( props.begin(),props.end() ) );
    }
    static void addKey(coral::TableDescription& table) {
        // Define primary key
        table.insertColumn( "id",
                           coral::AttributeSpecification::typeNameForId( typeid(std::string) ),
                           32 );
        table.setPrimaryKey( "id" );
        table.setNotNullConstraint( "id" );
    }
    static void addData(coral::TableDescription& table) {
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
struct ConfigDBAccessSvc::table_traits<ConfigTreeNode> {
    static const std::string& tableName() { static std::string s("ConfigTreeNodes"); return s; }
    static const std::string& keyName() { static std::string k("id"); return k; }
    typedef ConfigTreeNode::digest_type key_type;
    static key_type key(const ConfigTreeNode& x) { return x.digest(); }

    static ConfigTreeNode& read(const coral::AttributeList& row, ConfigTreeNode& x) {
        std::stringstream ss;
        ss << "Label: " << row["Label"].data<std::string>() << '\n';
        ss << "Leaf: "  << row["Leaf"].data<std::string>() << '\n';
        ss << "Nodes: [\n" << row["Nodes"].data<std::string>() << "]\n";
        ss >> x;
        return x;
    }
    static void write(const key_type& key, coral::AttributeList& row)  {
        row["id"]            .setValue<std::string>( key.str() );
    }
    static void write(const ConfigTreeNode& config, coral::AttributeList& row) {
        row["Label"]         .setValue<std::string>( config.label() );
        row["Leaf"]          .setValue<std::string>( config.leaf().str() );
        const ConfigTreeNode::NodeRefs&  nodes = config.nodes();
        row["Nodes"]         .setValue<std::string>( join( nodes.begin(),nodes.end(),to_str<ConfigTreeNode::digest_type>() ) );
    }
    static void addKey(coral::TableDescription& table) {
        table.insertColumn( "id",
                           coral::AttributeSpecification::typeNameForId( typeid(std::string) ),
                           32 );
        table.setPrimaryKey( "id" );
        table.setNotNullConstraint( "id" );
    }
    static void addData(coral::TableDescription& table) {
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

template <>
struct ConfigDBAccessSvc::table_traits<ConfigTreeNodeAlias> {
    static const std::string& tableName() { static std::string s("ConfigTreeNodeAliases"); return s; }
    static const std::string& keyName() { static std::string k("Alias"); return k; }
    typedef ConfigTreeNodeAlias::alias_type key_type;
    static key_type key(const ConfigTreeNodeAlias& x) { return x.alias(); }
    static ConfigTreeNodeAlias& read(const coral::AttributeList& row, ConfigTreeNodeAlias& x) {
        std::stringstream ss;
        ss << "Ref: "    << row["Ref"]  .data<std::string>() << '\n';
        ss << "Alias: "  << row["Alias"].data<std::string>() << '\n';
        // major is 'just' there to allow searches on type... eg. 'gimme all TCKs'
        ss >> x;
        return x;
    }
    static void write(const key_type& key, coral::AttributeList& row) {
        row["Alias"]       .setValue<std::string>( key.str() );
        row["Major"]       .setValue<std::string>( key.major() );
    }
    static void write(const ConfigTreeNodeAlias& config, coral::AttributeList& row) {
        row["Ref"]         .setValue<std::string>( config.ref().str() );
    }
    static void addKey(coral::TableDescription& table) {
        table.insertColumn( "Alias",
                           coral::AttributeSpecification::typeNameForId( typeid(std::string) ),
                           32 );
        table.setPrimaryKey( "Alias" );
        table.setNotNullConstraint( "Alias" );
        // Major is derived from Alias: the bit upto the first '/'
        table.insertColumn( "Major",
                           coral::AttributeSpecification::typeNameForId( typeid(std::string) ),
                           32 );
        table.setNotNullConstraint( "Major" );
        //@TODO declare Major to be an index... so we can search for 'TCK', or 'TOPLEVEL', or ...

    }
    static void addData(coral::TableDescription& table) {
        table.insertColumn( "Ref",
                           coral::AttributeSpecification::typeNameForId( typeid(std::string) ),
                           32 );
        table.setNotNullConstraint( "Ref" );
    }
};

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
       createTable<ConfigTreeNodeAlias>();
  }
  return sc;
}

//=============================================================================
// Finalization
//=============================================================================
StatusCode ConfigDBAccessSvc::finalize() {
  info() << "Finalize" << endmsg;
  if (m_coolConfSvc) { m_coolConfSvc->release(); m_coolConfSvc=0; }
  return Service::finalize();
}


template <typename T>
boost::optional<T> 
ConfigDBAccessSvc::read(const typename table_traits<T>::key_type& key) {
    Transaction transaction(*m_session,true);
    coral::AttributeList condData;
    condData.extend<std::string>("key");
    condData["key"].setValue<std::string>(key.str());
    coral::IQuery* q = m_session->nominalSchema().tableHandle( table_traits<T>::tableName() ).newQuery();
    q->setCondition( table_traits<T>::keyName() + " = :key", condData);
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

std::vector<ConfigTreeNodeAlias>
ConfigDBAccessSvc::configTreeNodeAliases(const ConfigTreeNodeAlias::alias_type& key) {
    Transaction transaction(*m_session,true);
    coral::AttributeList condData;
    condData.extend<std::string>("key");
    condData["key"].setValue<std::string>(key.major());
    coral::IQuery* q = m_session->nominalSchema().tableHandle( table_traits<ConfigTreeNodeAlias>::tableName() ).newQuery();
    q->setCondition( "Major = :key", condData);
    coral::ICursor& c = q->execute();
    std::vector<ConfigTreeNodeAlias> vec;
    while ( c.next() ) {
        ConfigTreeNodeAlias x;
        table_traits<ConfigTreeNodeAlias>::read(c.currentRow(),x);
        vec.push_back(x);
    }
    return vec;
}

template <typename T>
typename ConfigDBAccessSvc::table_traits<T>::key_type
ConfigDBAccessSvc::write(const T& value) {

    // first check whether it is not already there!
    typename ConfigDBAccessSvc::table_traits<T>::key_type key = ConfigDBAccessSvc::table_traits<T>::key(value);
    boost::optional<T> present = read<T>(key);
    if (present) {
        // check with table_traits<T> whether overwriting is allowed...
        assert(present.get() == value);
        return key;
    } 

    coral::ITransaction& trans = m_session->transaction();
    trans.start(false);
    coral::ITable& ctable = m_session->nominalSchema().tableHandle( ConfigDBAccessSvc::table_traits<T>::tableName() );
    coral::AttributeList row;
    ctable.dataEditor().rowBuffer(row);
    ConfigDBAccessSvc::table_traits<T>::write(key,row);
    ConfigDBAccessSvc::table_traits<T>::write(value,row);
    row["InsertionTime"] .setValue<coral::TimeStamp>( coral::TimeStamp::now() ); 
    ctable.dataEditor().insertRow( row );
    trans.commit();
    return key;
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

boost::optional<ConfigTreeNode>  
ConfigDBAccessSvc::readConfigTreeNodeAlias(const ConfigTreeNodeAlias::alias_type& alias)
{
  // resolve alias to a ref
  boost::optional<ConfigTreeNodeAlias> a = read<ConfigTreeNodeAlias>(alias);
  // and dispatch
  return a ? read<ConfigTreeNode>(a->ref()) : boost::optional<ConfigTreeNode>() ;
}

ConfigTreeNodeAlias::alias_type 
ConfigDBAccessSvc::writeConfigTreeNodeAlias(const ConfigTreeNodeAlias& alias)
{
  // insure the target already exists; if not, return invalid alias_type...
  if ( !read<ConfigTreeNode>(alias.ref()) ) return ConfigTreeNodeAlias::alias_type();
  // and perfrom the write 
  return write<ConfigTreeNodeAlias>(alias);
  //@TODO: if writing a toplevel alias, add entry in a 'cache' table with all nodes & leafs
  //       used for this toplevel so we can read the entire tree with a single SQL querry..
  //       note: if cache table absent, should still be able to read, albeit with multiple
  //       querries...
}

MsgStream& ConfigDBAccessSvc::msg(MSG::Level level) const {
     if (m_msg.get()==0) m_msg.reset( new MsgStream( msgSvc(), name() ));
     return *m_msg << level;
}

template <typename T>
void ConfigDBAccessSvc::createTable() {
    coral::TableDescription tableDesc( std::string("Table_")+ConfigDBAccessSvc::table_traits<T>::tableName() );
    tableDesc.setName( ConfigDBAccessSvc::table_traits<T>::tableName() );
    ConfigDBAccessSvc::table_traits<T>::addKey(tableDesc);
    ConfigDBAccessSvc::table_traits<T>::addData(tableDesc);

    tableDesc.insertColumn( "InsertionTime",
                            coral::AttributeSpecification::typeNameForId( typeid(coral::TimeStamp) ) );

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

#ifdef GR_USE_SEAL
  m_session = m_coolConfSvc->context()->query<coral::IConnectionService>( "CORAL/Services/ConnectionService" )->
#else
  m_session = m_coolConfSvc->connectionSvc().
#endif
               connect(m_connection, m_readOnly ? coral::ReadOnly : coral::Update );
   info() << "Connected to database \"" << m_connection << "\"" << endmsg;
   return StatusCode::SUCCESS;
}

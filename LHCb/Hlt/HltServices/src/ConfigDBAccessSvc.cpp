#include "ConfigDBAccessSvc.h"
#include <sstream>
#include <map>
#include <memory>
#include <utility>



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

    class Mutex {
        public:
            Mutex() { if (mutex()) std::cout << "cannot acquire" << std::endl;
                      else mutex()=true;
                    }
            ~Mutex() { if (!mutex()) std::cout << "cannot release" << std::endl;
                       else mutex()=false;
                    }
        private:
            static bool& mutex() { static bool m = false; return m; }
    };

    class Transaction {
        public:
            //TODO: deal with nested transactions... upgrading read to write...
            Transaction(coral::ISessionProxy& session,bool readonly=true) : m_trans(session.transaction()), m_readonly(readonly) { 
               if (!m_trans.isActive()) m_trans.start(readonly); }
            //void abort() { m_trans.rollback(); }
            //void commit() { m_trans.commit();  }
            ~Transaction() { if (m_trans.isActive())  m_trans.commit();  }
        private:
            Mutex m_mutex; // forbid nested transactions;
            coral::ITransaction& m_trans;
            bool m_readonly;
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
DECLARE_COMPONENT(ConfigDBAccessSvc)


//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
ConfigDBAccessSvc::ConfigDBAccessSvc( const std::string& name, ISvcLocator* pSvcLocator)
  : base_class ( name , pSvcLocator )
{
  declareProperty("Connection", m_connection = "" );
  declareProperty("ReadOnly", m_readOnly = true );
  declareProperty("CreateSchema", m_createSchema = false);
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
       createCacheTables();
  }
  return sc;
}

//=============================================================================
// Finalization
//=============================================================================
StatusCode ConfigDBAccessSvc::finalize() {
  info() << "Finalize" << endmsg;
  if (m_coolConfSvc) { m_coolConfSvc->release(); m_coolConfSvc=nullptr; }
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

    Transaction transaction(*m_session,false);
    coral::ITable& ctable = m_session->nominalSchema().tableHandle( ConfigDBAccessSvc::table_traits<T>::tableName() );
    coral::AttributeList row;
    ctable.dataEditor().rowBuffer(row);
    ConfigDBAccessSvc::table_traits<T>::write(key,row);
    ConfigDBAccessSvc::table_traits<T>::write(value,row);
    row["InsertionTime"] .setValue<coral::TimeStamp>( coral::TimeStamp::now() ); 
    ctable.dataEditor().insertRow( row );
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
  boost::optional<ConfigTreeNode> target = read<ConfigTreeNode>(alias.ref());
  if ( !target ) return ConfigTreeNodeAlias::alias_type();
  // and perfrom the write 
  ConfigTreeNodeAlias::alias_type ret = write<ConfigTreeNodeAlias>(alias);
  //@TODO: if writing a toplevel alias, add entry in a 'cache' table with all nodes & leafs
  //       used for this toplevel so we can read the entire tree with a single SQL querry..
  //       note: if cache table absent, should still be able to read, albeit with multiple
  //       querries...

  // see if target is already in cache table
  // if not, create entries in cache table
  if (!hasCacheEntries( "ConfigTreeNode2ConfigTreeNode",  target->digest().str())) {
     generateCacheTableEntries(*target);
  }
  return ret;
}


template <typename iterator>
void ConfigDBAccessSvc::writeCacheEntries(const std::string& tableName, const std::string& parent, 
                                          iterator i, iterator end ) {
    debug() << " requested to write " << std::distance(i,end) 
            << " cache entries for " << parent 
            << " in table " << tableName << endmsg;

    Transaction trans(*m_session,false) ;
    coral::ITable& table = m_session->nominalSchema().tableHandle(tableName);
    coral::AttributeList data;
    table.dataEditor().rowBuffer(data);
    data["parent"].data<std::string>() = parent; 
    coral::IBulkOperation* op = table.dataEditor().bulkInsert(data,std::distance(i,end));
    //TODO: use binding...
    while ( i!=end ) {
        data["daughter"].data<std::string>() = i->str();
        op->processNextIteration();
        ++i;
    }
    op->flush();
}

bool ConfigDBAccessSvc::hasCacheEntries(const std::string& tableName, const std::string& parent) {
    Transaction transaction(*m_session,true);
    coral::AttributeList condData;
    condData.extend<std::string>("key");
    condData["key"].setValue<std::string>(parent);
    coral::IQuery* q = m_session->nominalSchema().tableHandle( tableName ).newQuery();
    q->setCondition(  " parent = :key", condData);
    q->limitReturnedRows(1); // one match is enough...
    coral::ICursor& c = q->execute();
    bool x = false;
    while ( c.next() ) { x=true; } // make sure we finish the statement...
    return x;
}

void ConfigDBAccessSvc::generateCacheTableEntries(const ConfigTreeNode& target) {
    // first check cache...

    std::list<ConfigTreeNode::digest_type> nodeRefs; // note: we use list, as appending to a list
                                             // does not invalidate iterators!!!!
    nodeRefs.push_back(target.digest()); //TODO: remove this one at the end...
    std::vector<PropertyConfig::digest_type> leafRefs;
    auto i = nodeRefs.begin();
    while(i!=nodeRefs.end()) {
        boost::optional<ConfigTreeNode> node = read<ConfigTreeNode>(*i);
        if (!node) {
            throw GaudiException("failed to resolve node ", name() + "::generateCacheTableEntries",StatusCode::FAILURE);
        }
        PropertyConfig::digest_type leafRef = node->leaf();
        //TODO: use equal_range instead of find...
        if (leafRef.valid() && std::find(leafRefs.begin(),leafRefs.end(),leafRef)==leafRefs.end()) { 
            leafRefs.push_back(leafRef);
        }

        const ConfigTreeNode::NodeRefs& nodes = node->nodes();
        for (const auto & node : nodes) {
            //FIXME: this is not going to be very fast, as it going to 
            // make this operation quadratic...  should keep list sorted... but then
            // we have to rescan the entire range, as right now we keep going forward...
            // std::pair< > r = std::equal_range(nodeRefs.begin(),nodeRefs.end(),*j);
            // if (r.first!=r.second) nodeRefs.insert(r.first,*j);
            if (std::find(nodeRefs.begin(),nodeRefs.end(),node)==nodeRefs.end()) {
                nodeRefs.push_back(node);
            }
        }
        ++i;
    }
    nodeRefs.pop_front(); // remove initial dependency on self...

    // now we have a list of digests referenced by this node. 
    // fill table with parent/daughter entries for each daughter
    // as one single bulk operation
    writeCacheEntries( "ConfigTreeNode2ConfigTreeNode",  target.digest().str(),  nodeRefs.begin(),nodeRefs.end() );
    writeCacheEntries( "ConfigTreeNode2PropertyConfig",  target.digest().str(),  leafRefs.begin(),leafRefs.end() );
}

void ConfigDBAccessSvc::createCacheTables() {
    const char *names[] = { "ConfigTreeNode2ConfigTreeNode", "ConfigTreeNode2PropertyConfig" };
    for (auto & name : names) {
        coral::TableDescription descr( std::string("Table_")+name );
        descr.setName( std::string(name) );
        descr.insertColumn( "parent",
                           coral::AttributeSpecification::typeNameForId( typeid(std::string) ),
                           32 );
        descr.setNotNullConstraint( "parent" );
        descr.insertColumn( "daughter",
                           coral::AttributeSpecification::typeNameForId( typeid(std::string) ),
                           32 );
        descr.setNotNullConstraint( "daughter" );
        m_session->nominalSchema().dropIfExistsTable( descr.name() );
        coral::ITable& table = m_session->nominalSchema().createTable( descr );
        table.privilegeManager().grantToPublic( coral::ITablePrivilegeManager::Select );
    }
}

MsgStream& ConfigDBAccessSvc::msg(MSG::Level level) const {
     if (!m_msg) m_msg.reset( new MsgStream( msgSvc(), name() ));
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
    if (m_coolConfSvc==nullptr) {
        StatusCode sc = service("COOLConfSvc",m_coolConfSvc);
        if ( !sc.isSuccess() )   return sc;
    }

    if ( m_connection.empty() ) {
       std::string def( System::getEnv( "HLTTCKROOT" ) );
       if (!def.empty()) {
          def = std::string("sqlite_file:") + def + "/db/config.db";
       } else {
          throw GaudiException("Environment variable HLTTCKROOT not specified and no explicit "
                               "filename given; cannot obtain location of config.db.",
                               name(), StatusCode::FAILURE);
       }
       m_connection = def;
    }
    m_session = m_coolConfSvc->connectionSvc()
              . connect(m_connection, m_readOnly ? coral::ReadOnly : coral::Update );
    info() << "Connected to database \"" << m_connection << "\"" << endmsg;
    return StatusCode::SUCCESS;
}

#include "ConfigDBAccessSvc.h"
// #include "md5.h"
#include <sstream>
#include <map>
#include <memory>
#include <utility>


#include "GaudiKernel/SvcFactory.h"


#include "PluginManager/PluginManager.h"
#include "SealKernel/Context.h"
#include "SealKernel/ComponentLoader.h"
#include "SealKernel/MessageStream.h"
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
}

// Factory implementation
DECLARE_SERVICE_FACTORY(ConfigDBAccessSvc)


//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
ConfigDBAccessSvc::ConfigDBAccessSvc( const std::string& name, ISvcLocator* pSvcLocator)
  : Service ( name , pSvcLocator )
{
  declareProperty("Connection", m_connection = "sqlite_file:/tmp/config.db");
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
  if ( m_createSchema )  sc = createSchema();
  return sc;
}

//=============================================================================
// Finalization
//=============================================================================
StatusCode ConfigDBAccessSvc::finalize() {
  info() << "Finalize" << endmsg;
  // TODO close connection if still open...
  //TODO: add idle-time thread, and see how long has passed since we
  //      last committed a transaction... (and close the connection if
  //      too long)
  return Service::finalize();
}

namespace {
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
};

// read a single config
boost::optional<PropertyConfig> 
ConfigDBAccessSvc::readPropertyConfig(const PropertyConfig::digest_type& ref) {
    Transaction transaction(*m_session,true);
    coral::AttributeList condData;
    condData.extend<std::string>("ref");
    condData["ref"].setValue<std::string>(ref.str());
    coral::IQuery* q = m_session->nominalSchema().tableHandle( "PropertyConfigs" ).newQuery();
    q->setCondition( std::string(" id = :ref "), condData);
    q->limitReturnedRows(1); // we know we only get zero or one matches
    coral::ICursor& c = q->execute();
    PropertyConfig x;
    size_t  nrow(0);
    while ( c.next() ) {
        std::stringstream ss;
        ss << "Name: " << c.currentRow()["Name"].data<std::string>() << '\n';
        ss << "Kind: " << c.currentRow()["Kind"].data<std::string>() << '\n';
        ss << "Type: " << c.currentRow()["Type"].data<std::string>() << '\n';
        ss << "Properties: [\n" << c.currentRow()["Properties"].data<std::string>() << "]\n";
        ss >> x;
        ++nrow;
    }
    return nrow==1 ? x : boost::optional<PropertyConfig>();
}

// read a single node
boost::optional<ConfigTreeNode> 
ConfigDBAccessSvc::readConfigTreeNode(const ConfigTreeNode::digest_type& ref) {
    Transaction transaction(*m_session,true);
    coral::AttributeList condData;
    condData.extend<std::string>("ref");
    condData["ref"].setValue<std::string>(ref.str());
    coral::IQuery* q = m_session->nominalSchema().tableHandle( "ConfigTreeNodes" ).newQuery();
    q->setCondition( std::string(" id = :ref "), condData);
    q->limitReturnedRows(1); // we know we only get zero or one matches
    coral::ICursor& c = q->execute();
    ConfigTreeNode x;
    size_t  nrow(0);
    while ( c.next() ) {
        std::stringstream ss;
        ss << "Label: " << c.currentRow()["Label"].data<std::string>() << '\n';
        ss << "Leaf: " << c.currentRow()["Leaf"].data<std::string>() << '\n';
        ss << "Nodes: [\n" << c.currentRow()["Nodes"].data<std::string>() << "]\n";
        ss >> x;
        ++nrow;
    }
    return nrow==1 ? x : boost::optional<ConfigTreeNode>();
}



PropertyConfig::digest_type
ConfigDBAccessSvc::writePropertyConfig(const PropertyConfig& config) {
    // first check whether it is not already there!
    PropertyConfig::digest_type id = config.digest();
    boost::optional<PropertyConfig> present = readPropertyConfig(id);
    if (present) {
        assert(present.get() == config);
        return id;
    } 

    coral::ITransaction& trans = m_session->transaction();
    trans.start(false);
    coral::ITable& ctable = m_session->nominalSchema().tableHandle( "PropertyConfigs" );
    coral::AttributeList row;
    ctable.dataEditor().rowBuffer(row);
    row["id"]            .setValue<std::string>( id.str() );
    row["Name"]          .setValue<std::string>( config.name());
    row["Kind"]          .setValue<std::string>( config.kind());
    row["Type"]          .setValue<std::string>( config.type());
    const PropertyConfig::Properties&  props = config.properties();
    row["Properties"]    .setValue<std::string>( join( props.begin(),props.end() ) );
    row["InsertionTime"] .setValue<coral::TimeStamp>( coral::TimeStamp::now() ); 
    ctable.dataEditor().insertRow( row );

    trans.commit();
    return id;
}

ConfigTreeNode::digest_type
ConfigDBAccessSvc::writeConfigTreeNode(const ConfigTreeNode& config) {
    // first check whether it is not already there!
    ConfigTreeNode::digest_type id = config.digest();
    boost::optional<ConfigTreeNode> present = readConfigTreeNode(id);
    if (present) {
        assert(present.get() == config);
        return id;
    } 

    coral::ITransaction& trans = m_session->transaction();
    trans.start(false);
    coral::ITable& ctable = m_session->nominalSchema().tableHandle( "ConfigTreeNodes" );
    coral::AttributeList row;
    ctable.dataEditor().rowBuffer(row);
    row["id"]            .setValue<std::string>( id.str() );
    row["Label"]         .setValue<std::string>( config.label() );
    row["Leaf"]          .setValue<std::string>( config.leaf().str() );
    const ConfigTreeNode::NodeRefs&  nodes = config.nodes();
    row["Nodes"]         .setValue<std::string>( join( nodes.begin(),nodes.end(),to_str<ConfigTreeNode::digest_type>() ) );
    row["InsertionTime"] .setValue<coral::TimeStamp>( coral::TimeStamp::now() ); 
    ctable.dataEditor().insertRow( row );

    trans.commit();
    return id;
}

MsgStream& ConfigDBAccessSvc::msg(MSG::Level level) const {
     if (m_msg.get()==0) m_msg.reset( new MsgStream( msgSvc(), name() ));
     return *m_msg << level;
}


StatusCode ConfigDBAccessSvc::createSchema() {
    Transaction transaction(*m_session,false);

    coral::TableDescription propertyConfigTable( "Table_PropertyConfig" );
    propertyConfigTable.setName( "PropertyConfigs" );
    // Define primary key
    propertyConfigTable.insertColumn( "id",
                               coral::AttributeSpecification::typeNameForId( typeid(std::string) ),
                               32 );
    propertyConfigTable.setPrimaryKey( "id" );
    propertyConfigTable.setNotNullConstraint( "id" );

    propertyConfigTable.insertColumn( "Name",
                               coral::AttributeSpecification::typeNameForId( typeid(std::string) ),
                               128,
                               false );
    propertyConfigTable.setNotNullConstraint( "Name" );

    propertyConfigTable.insertColumn( "Type",
                               coral::AttributeSpecification::typeNameForId( typeid(std::string) ),
                               128,
                               false );
    propertyConfigTable.setNotNullConstraint( "Type" );

    propertyConfigTable.insertColumn( "Kind",
                               coral::AttributeSpecification::typeNameForId( typeid(std::string) ),
                               32,
                               false );
    propertyConfigTable.setNotNullConstraint( "Kind" );

    propertyConfigTable.insertColumn( "Properties",
                               coral::AttributeSpecification::typeNameForId( typeid(std::string) ),
                               1024,
                               false );

    propertyConfigTable.insertColumn( "InsertionTime",
                               coral::AttributeSpecification::typeNameForId( typeid(coral::TimeStamp) )
                               );

    // Create the actual table
    m_session->nominalSchema().dropIfExistsTable( propertyConfigTable.name() );
    coral::ITable& table = m_session->nominalSchema().createTable( propertyConfigTable );
    table.privilegeManager().grantToPublic( coral::ITablePrivilegeManager::Select );



    coral::TableDescription configTreeNodeTable( "Table_ConfigTreeNode" );
    configTreeNodeTable.setName( "ConfigTreeNodes" );
    // Define primary key
    configTreeNodeTable.insertColumn( "id",
                               coral::AttributeSpecification::typeNameForId( typeid(std::string) ),
                               32 );
    configTreeNodeTable.setPrimaryKey( "id" );
    configTreeNodeTable.setNotNullConstraint( "id" );

    configTreeNodeTable.insertColumn( "Label",
                               coral::AttributeSpecification::typeNameForId( typeid(std::string) ),
                               128 );

    configTreeNodeTable.insertColumn( "Leaf",
                               coral::AttributeSpecification::typeNameForId( typeid(std::string) ),
                               32 );
    configTreeNodeTable.setNotNullConstraint( "Leaf" );

    configTreeNodeTable.insertColumn( "Nodes",
                               coral::AttributeSpecification::typeNameForId( typeid(std::string) ),
                               1024,
                               false );
    configTreeNodeTable.insertColumn( "InsertionTime",
                               coral::AttributeSpecification::typeNameForId( typeid(coral::TimeStamp) )
                               );


    // Create the actual table
    m_session->nominalSchema().dropIfExistsTable( configTreeNodeTable.name() );
    table = m_session->nominalSchema().createTable( configTreeNodeTable );
    table.privilegeManager().grantToPublic( coral::ITablePrivilegeManager::Select );


    return StatusCode::SUCCESS;
}

StatusCode ConfigDBAccessSvc::openConnection() {
   always() << " opening connection!!! " << endmsg;
   seal::Handle<seal::Context> m_context = new seal::Context() ;
   seal::PluginManager* pm = seal::PluginManager::get();
   pm->initialise();
   seal::Handle<seal::ComponentLoader> loader = new seal::ComponentLoader( m_context.get() );
   loader->load("SEAL/Services/MessageService");
   std::vector< seal::Handle<seal::IMessageService> > v_msgSvc;
   m_context->query( v_msgSvc );
   if ( ! v_msgSvc.empty() ) {
        seal::Handle<seal::IMessageService>& msgSvc = v_msgSvc.front();
        msgSvc->setOutputLevel( seal::Msg::Debug ); //FIXME: reduce outputlevel...
   }
   loader->load("CORAL/Services/ConnectionService");
   seal::IHandle<coral::IConnectionService> iHandle =
      m_context->query<coral::IConnectionService>( "CORAL/Services/ConnectionService" );

   m_session = iHandle->connect(m_connection, coral::Update );
   return StatusCode::SUCCESS;
}

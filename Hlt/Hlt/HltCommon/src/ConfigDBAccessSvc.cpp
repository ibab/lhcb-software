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
  declareProperty("Prefetch", m_prefetch = true);
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

boost::optional<PropertyConfig> 
ConfigDBAccessSvc::read(const PropertyConfig::digest_type& ref) {
    if (m_prefetch) {
        cache_t::iterator i = m_cache.find(ref);
        // we prefetch all dependencies using a single
        // database transaction in order to minimize DB 
        // access overhead
        if ( i==m_cache.end() ) { 
           // the cache doesn't contain the right branch...
           // clear, and start again.
           m_cache.clear();
           prefetch(ref);
           i = m_cache.find(ref);
        }
        boost::optional<PropertyConfig> c;
        if (i!=m_cache.end()) { 
           c = i->second ;
            // given that the layer above us caches random accesses,
            // we can remove it from our prefetch cache once it has
            // been read...
           m_cache.erase(i);
        }
        return c;
    } else {
        return get(ref);
    }
}

// read ref, and everything it uses, into the cache
// with only a single DB transaction.
size_t
ConfigDBAccessSvc::prefetch(const PropertyConfig::digest_type& ref) {
//TODO: compare performance of subquery vs. join
        info() << "prefetch for " << ref << endmsg;
        Transaction transaction(*m_session,true);
        coral::IQuery *q = m_session->nominalSchema().newQuery();
        q->addToTableList("ConfigurationData");
        q->addToOutputList("id");
        q->addToOutputList("Name");
        q->addToOutputList("Type");
        q->addToOutputList("Properties");
        q->addToOutputList("Dependencies");

        coral::IQueryDefinition& sub = q->defineSubQuery( "P" );
        q->addToTableList("P");
        sub.addToTableList("used_by_data");
        sub.addToOutputList("daughter_id","d");
        coral::AttributeList subCondData;
        subCondData.extend<std::string>("ref");
        subCondData["ref"].setValue<std::string>(ref.str());
        sub.setCondition(" parent_id = :ref ",subCondData);
        q->setCondition(" id = P.d ",coral::AttributeList());

        // now add the ref itself as UNION... 
        // (doing this as an 'OR' in the condition of the query itself doesn't work
        //  if the subquery gives an empty set... weird; At least SQLite manager
        //  has the same behaviour as CORAL, so it is probably something in SQL(ite))
        coral::IQueryDefinition& u = q->applySetOperation( coral::IQueryDefinition::Union );
        u.addToTableList("ConfigurationData");
        u.addToOutputList("id");
        u.addToOutputList("Name");
        u.addToOutputList("Type");
        u.addToOutputList("Properties");
        u.addToOutputList("Dependencies");
        coral::AttributeList condData;
        condData.extend<std::string>("req");
        condData["req"].setValue<std::string>(ref.str());
        u.setCondition(" id = :req",condData);

        coral::ICursor& c = q->execute();
        size_t  nrow(0);
        while ( c.next() ) { //TODO: bind variables to currentRow
           PropertyConfig::digest_type id = MD5::convertString2Digest( c.currentRow()["id"].data<std::string>() );
           std::pair<cache_t::iterator,cache_t::iterator> iter = m_cache.equal_range(id);
           if (iter.first!=iter.second) { 
                info() << " prefetch: entry already in cache !!???? " << id  << endmsg;
                continue;
           }
           std::stringstream ss;
           ss << "Name: " << c.currentRow()["Name"].data<std::string>() << '\n';
           ss << "Type: " << c.currentRow()["Type"].data<std::string>() << '\n';
           ss << "Properties: [\n" << c.currentRow()["Properties"].data<std::string>() << "]\n";
           ss << "Dependencies: [\n" << c.currentRow()["Dependencies"].data<std::string>() << "]\n";
           PropertyConfig x;
           ss >> x;

           m_cache.insert( iter.first, std::make_pair(  id, x ) );
           ++nrow;
        }
        info() << " prefetch: cache now contains " << m_cache.size() << " entries " << endmsg;
        return nrow;
}

// read a single config
boost::optional<PropertyConfig> 
ConfigDBAccessSvc::get(const PropertyConfig::digest_type& ref) {
    Transaction transaction(*m_session,true);
    coral::AttributeList condData;
    condData.extend<std::string>("ref");
    condData["ref"].setValue(ref);
    coral::IQuery* q = m_session->nominalSchema().tableHandle( "ConfigurationData" ).newQuery();
    q->setCondition( std::string(" id = :ref "), condData);
    q->limitReturnedRows(1); // we know we only get zero or one matches
    coral::ICursor& c = q->execute();
    PropertyConfig x;
    size_t  nrow(0);
    while ( c.next() ) {
        std::stringstream ss;
        ss << "Name: " << c.currentRow()["Name"].data<std::string>() << '\n';
        ss << "Type: " << c.currentRow()["Type"].data<std::string>() << '\n';
        ss << "Properties: [\n" << c.currentRow()["Properties"].data<std::string>() << "]\n";
        ss << "Dependencies: [\n" << c.currentRow()["Dependencies"].data<std::string>() << "]\n";
        ss >> x;
        ++nrow;
    }
    return nrow==1 ? x : boost::optional<PropertyConfig>();
}


namespace {
    template <typename T> struct id { T& operator()(T& t) { return t; } };
    template <typename T> struct to_str { std::string operator()(const T& t) { return t.str(); } };
    template <typename T, typename OP> std::string join(T i, const T& end, OP op) {
       static const std::string x("'");
       std::string out;
       while (i != end ) {
         out += x + i->first + ':' + op(i->second) + '\n';
         ++i;
       }
       return out;
    }
    template <typename T> std::string join(T i, const T& end) {
       static const std::string x("'");
       std::string out;
       while (i != end ) {
         out += x + i->first + ':' + i->second + '\n';
         ++i;
       }
       return out;
    }
}

PropertyConfig::digest_type
ConfigDBAccessSvc::write(const PropertyConfig& config) {
    // first check whether it is not already there!
    PropertyConfig::digest_type id = config.digest();
    boost::optional<PropertyConfig> present = read(id);
    if (present) {
        assert(present.get() == config);
        return id;
    } 

    const PropertyConfig::Properties&  props = config.properties();
    const PropertyConfig::Dependencies& deps = config.dependants();

    coral::ITransaction& trans = m_session->transaction();
    trans.start(false);
    coral::ITable& ctable = m_session->nominalSchema().tableHandle( "ConfigurationData" );
    coral::AttributeList row;
    ctable.dataEditor().rowBuffer(row);
    row["id"]            .setValue<std::string>( id.str() );
    row["Name"]          .setValue<std::string>( config.name());
    row["Type"]          .setValue<std::string>( config.type());
    //FIXME: move conversion of props,deps -> string into PropertyConfig to 'canonicalize' it...
    row["Properties"]    .setValue<std::string>( join( props.begin(),props.end() ) );
    row["Dependencies"]  .setValue<std::string>( join(deps.begin(),deps.end(),to_str<PropertyConfig::digest_type>()) );
    row["InsertionTime"] .setValue<coral::TimeStamp>( coral::TimeStamp::now() ); 
    ctable.dataEditor().insertRow( row );

    trans.commit();
    
    if (!deps.empty()) { 
        // record the fact that this config uses its direct dependants
        trans.start(false);
        coral::ITable& rtable = m_session->nominalSchema().tableHandle( "used_by_data" );
        coral::AttributeList prow;
        rtable.dataEditor().rowBuffer(prow);
        prow["parent_id"] .setValue<std::string>( id.str() );
        prow["depth"] .setValue<unsigned int>( 1 );
        std::auto_ptr<coral::IBulkOperation> op( rtable.dataEditor().bulkInsert(prow,deps.size()) );
        for (PropertyConfig::Dependencies::const_iterator i=deps.begin(); i!=deps.end(); ++i ) { 
            prow["daughter_id"]     .setValue<std::string>( i->second.str() );
            op->processNextIteration();
        }
        op->flush();
        op.reset();
        trans.commit();

        // subquery:   select daugther_id where parent_id = me
        // query:      select daughter_id, depth where parent_id = ( sub query )
        //   next: insert daughter_id, depth+1, me
        trans.start(false);
        coral::IQuery *q = m_session->nominalSchema().newQuery();
        q->addToTableList("used_by_data");

        coral::IQueryDefinition& sub = q->defineSubQuery( "P" );
        sub.addToTableList("used_by_data");
        sub.addToOutputList("daughter_id","d");
        coral::AttributeList subCondData;
        subCondData.extend<std::string>("ref");
        subCondData["ref"].setValue<std::string>(id.str());
        sub.setCondition(" parent_id = :ref ",subCondData);

        q->addToTableList("P");
        q->addToOutputList("daughter_id");
        q->addToOutputList("depth");
        q->setCondition(" parent_id = P.d ",coral::AttributeList());
        coral::ICursor& c = q->execute();
        std::vector<std::pair<std::string,unsigned int> > uses; // sibling, distance
        while ( c.next() ) {
               uses.push_back( std::make_pair(  c.currentRow()["daughter_id"].data<std::string>(),
                                                c.currentRow()["depth"].data<unsigned int>()+1     ) );
        }

        trans.commit();
        if (!uses.empty()) {
            trans.start(false);
            coral::AttributeList irow;
            rtable.dataEditor().rowBuffer(irow);
            irow["parent_id"].setValue<std::string>( id.str() );
            op.reset( rtable.dataEditor().bulkInsert(irow,uses.size()) );
            for (std::vector<std::pair<std::string,unsigned int> >::const_iterator i = uses.begin(); i!=uses.end(); ++i) {
              irow["daughter_id"].setValue<std::string>( i->first );
              irow["depth"].setValue<unsigned int>(  i->second );
              op->processNextIteration();
            }
            op->flush();
            trans.commit();
        }
    }
    return id;
}

MsgStream& ConfigDBAccessSvc::msg(MSG::Level level) const {
     if (m_msg.get()==0) m_msg.reset( new MsgStream( msgSvc(), name() ));
     return *m_msg << level;
}


StatusCode ConfigDBAccessSvc::createSchema() {
    Transaction transaction(*m_session,false);

    coral::TableDescription configData( "Table_config_data" );
    configData.setName( "ConfigurationData" );
    // Define primary key
    configData.insertColumn( "id",
                               coral::AttributeSpecification::typeNameForId( typeid(std::string) ),
                               32 );
    configData.setPrimaryKey( "id" );
    configData.setNotNullConstraint( "id" );

    configData.insertColumn( "Name",
                               coral::AttributeSpecification::typeNameForId( typeid(std::string) ),
                               128,
                               false );
    configData.setNotNullConstraint( "Name" );

    configData.insertColumn( "Type",
                               coral::AttributeSpecification::typeNameForId( typeid(std::string) ),
                               128,
                               false );
    configData.setNotNullConstraint( "Type" );

    configData.insertColumn( "Properties",
                               coral::AttributeSpecification::typeNameForId( typeid(std::string) ),
                               1024,
                               false );

    configData.insertColumn( "Dependencies",
                               coral::AttributeSpecification::typeNameForId( typeid(std::string) ),
                               256,
                               false );
    //TODO add insertion time
    configData.insertColumn( "InsertionTime",
                               coral::AttributeSpecification::typeNameForId( typeid(coral::TimeStamp) )
                               );


    // Create the actual table
    m_session->nominalSchema().dropIfExistsTable( configData.name() );
    coral::ITable& table = m_session->nominalSchema().createTable( configData );
    table.privilegeManager().grantToPublic( coral::ITablePrivilegeManager::Select );


    coral::TableDescription depList( "Table_used_by" );
    depList.setName( "used_by_data" );
    depList.insertColumn( "daughter_id",
                          coral::AttributeSpecification::typeNameForId( typeid(std::string) ),
                          32 );
    depList.setNotNullConstraint("daughter_id");
    depList.createForeignKey( "daughter", "daughter_id", "ConfigurationData", "id");
    depList.insertColumn( "parent_id",
                          coral::AttributeSpecification::typeNameForId( typeid(std::string) ),
                          32 );
    depList.createForeignKey( "parent", "parent_id", "ConfigurationData", "id");
    depList.insertColumn( "depth",
                          coral::AttributeSpecification::typeNameForId( typeid(unsigned int) ));

    m_session->nominalSchema().dropIfExistsTable( depList.name() );
    table = m_session->nominalSchema().createTable( depList );
    table.privilegeManager().grantToPublic( coral::ITablePrivilegeManager::Select );

    return StatusCode::SUCCESS;
}

StatusCode ConfigDBAccessSvc::openConnection() {
   always() << " opening connection!!! " << endmsg;
   seal::Handle<seal::Context> m_context = new seal::Context() ;
   seal::PluginManager* pm = seal::PluginManager::get();
   pm->initialise();
   seal::Handle<seal::ComponentLoader> loader = new seal::ComponentLoader( m_context.get() );
   loader->load("CORAL/Services/ConnectionService");
   seal::IHandle<coral::IConnectionService> iHandle =
      m_context->query<coral::IConnectionService>( "CORAL/Services/ConnectionService" );

   m_session = iHandle->connect(m_connection, coral::Update );
   return StatusCode::SUCCESS;
}

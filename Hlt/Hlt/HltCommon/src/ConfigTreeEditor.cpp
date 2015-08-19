// Include files
// local
#include "ConfigTreeEditor.h"

// std
#include <memory>


// from Gaudi
#include "GaudiKernel/ToolFactory.h"

// from HltBase

using namespace std;

namespace
{

    constexpr struct select2nd_t {
        template<typename U, typename V>
        const V& operator()(const std::pair<U,V>& p) const { return p.second; }
        template<typename U, typename V>
        V& operator()(std::pair<U,V>& p) const { return p.second; }
    } select2nd {};
}

/////////Utility class for easy manipulation of
///configurations/////////////////////////////
class ConfigTree
{
  public:
    class Visitor
    {
      public:
        virtual ~Visitor() = default;
        virtual void pre( const ConfigTree& )
        {
            ;
        }
        virtual void post( const ConfigTree& )
        {
            ;
        }
        virtual bool descend( const ConfigTree& )
        {
            return true;
        }
    };

    class Lookup
    { // contains all object for this tree...
      public:
        Lookup( IPropertyConfigSvc& r ) : m_r( r ) {};
        const ConfigTreeNode* node( const ConfigTreeNode::digest_type& id )
        {
            auto i = m_nodes.find( id );
            if ( i == m_nodes.end() ) {
                auto cn = m_r.resolveConfigTreeNode( id );
                auto r = m_nodes.insert( make_pair( id, cn ) );
                assert( r.second );
                i = r.first;
            }
            return i->second;
        }

        const PropertyConfig* leaf( const PropertyConfig::digest_type& id )
        {
            auto i = m_config.find( id );
            if ( i == m_config.end() ) {
                auto r = m_config.insert( {id, m_r.resolvePropertyConfig( id )} );
                assert( r.second );
                i = r.first;
            }
            return i->second;
        }
        ConfigTree* tree( const ConfigTreeNode::digest_type& id, ConfigTree* parent )
        {
            auto i = m_trees.find( id );
            if ( i == m_trees.end() ) {
                auto r = m_trees.insert( {id, new ConfigTree{id, parent}} );
                assert( r.second );
                i = r.first;
            } else {
                i->second->addParent( parent );
            }
            return i->second;
        }

      private:
        typedef map<ConfigTreeNode::digest_type, ConfigTree*> TreeMap_t;
        typedef map<PropertyConfig::digest_type, const PropertyConfig*> ConfigMap_t;
        typedef map<ConfigTreeNode::digest_type, const ConfigTreeNode*> NodeMap_t;
        TreeMap_t m_trees;
        ConfigMap_t m_config;
        NodeMap_t m_nodes;
        IPropertyConfigSvc& m_r;
    };

    // constructor for the nodes inside the tree
    ConfigTree( const ConfigTreeNode::digest_type& in, ConfigTree* parent )
        : m_lookup{nullptr}
        , m_ownedLeaf{nullptr}
        , m_root{nullptr}
        , m_leaf{nullptr}
        , m_origDigest{in}
    {
        assert( parent != 0 );
        addParent( parent );
        addLeaf();
        addDeps();
    }

    // constructor for the top of the tree
    // root node is special:
    // given that the graph might contain cycles, we need one
    // lookup table which is 'global' to the tree -- so we put
    // it into the 'root' node.
    ConfigTree( const ConfigTreeNode::digest_type& in, IPropertyConfigSvc& r,
                std::string label = {} )
        : m_lookup( new ConfigTree::Lookup( r ) )
        , m_ownedLeaf{nullptr}
        , m_root( this )
        , m_leaf{nullptr}
        , m_origDigest( in )
    {
        addParent( nullptr );
        addLeaf(); // unconditionally copies label from 'in' into m_label...
        m_label = std::move(label);
        if ( m_label.empty() ) m_label = " ( mutation of " + in.str() + " )";
        addDeps();
    }

    void addParent( ConfigTree* parent )
    {
        if ( parent ) {
            assert( !root() || root() == parent->root() ); // must be same tree...
            if ( find( m_parents.begin(), m_parents.end(), parent ) !=
                 m_parents.end() ) {
                // std::cout << " configTree("<<m_origDigest<<
                // ")::addParent("<<parent->m_origDigest<<") already present.."  <<
                // std::endl;
            } else {
                m_parents.push_back( parent );
                assert( m_lookup.get() == 0 );
                m_root = parent->root();
            }
        } else {
            assert( m_parents.empty() );
        }
    }

    void addLeaf()
    {
        auto c = lookupConfigTreeNode( m_origDigest );
        assert( c );
        m_label = c->label();
        if ( !c->leaf().invalid() ) {
            m_leaf = lookupPropertyConfig( c->leaf() );
            assert( m_leaf );
        }
    }

    void addDeps()
    {
        // lookup returns a pointer to either a 'new' ConfigTree instance,
        // or to an existing one, adding 'this' to the list of parents of it.
        // It is done this way because the graph may contain cycles...
        auto c = lookupConfigTreeNode( m_origDigest );
        std::transform( std::begin( c->nodes() ), std::end( c->nodes() ),
                        std::back_inserter( m_deps ),
                        [&]( const ConfigTreeNode::digest_type& i ) {
            return lookupConfigTree( i, this );
        } );
    }

    ~ConfigTree()
    {
    }

    void visit( ConfigTree::Visitor& visitor ) const
    {
        visitor.pre( *this );
        if ( visitor.descend( *this ) ) {
            for_each( std::begin(m_deps), std::end(m_deps),
                      [&]( const ConfigTree* tree ) { tree->visit( visitor ); } );
        }
        visitor.post( *this );
    }

    ConfigTreeNode::digest_type write( IConfigAccessSvc& w )
    {
        ConfigTreeNode::LeafRef lr =
            ( m_leaf  ?  w.writePropertyConfig( *m_leaf ) 
                      : ConfigTreeNode::digest_type::createInvalid());
        assert( !m_leaf || !lr.invalid() );
        ConfigTreeNode::NodeRefs nr;
        std::transform( std::begin( m_deps ), std::end( m_deps ),
                        std::back_inserter( nr ), [&]( ConfigTree* tree ) {
            auto d = tree->write( w );
            assert( !d.invalid() );
            return d;
        } );
        const ConfigTreeNode* c = lookupConfigTreeNode( m_origDigest );
        assert( c );
        unique_ptr<ConfigTreeNode> nc{ c->clone( lr, nr, m_label ) };
        return w.writeConfigTreeNode( *nc );
    }

    const ConfigTreeNode*
    lookupConfigTreeNode( const ConfigTreeNode::digest_type& id )
    {
        return lookup().node( id );
    }
    const PropertyConfig*
    lookupPropertyConfig( const PropertyConfig::digest_type& id )
    {
        return lookup().leaf( id );
    }
    ConfigTree* lookupConfigTree( const ConfigTreeNode::digest_type& id,
                                  ConfigTree* parent )
    {
        return lookup().tree( id, parent );
    }

    ConfigTree* findNodeWithLeaf( const string& name )
    {
        if ( m_leaf && m_leaf->name() == name ) return this;
        for ( auto& i : m_deps ) {
            ConfigTree* f = i->findNodeWithLeaf( name );
            if ( f ) return f;
        }
        return nullptr;
    }

    const PropertyConfig* leaf() const
    {
        return m_leaf;
    }

    template <typename T>
    bool updateLeaf( const T& key2value )
    { // T::value_type must be pair<string,string>, representing (key,value)
        PropertyConfig update =
            m_leaf->copyAndModify( std::begin(key2value), std::end(key2value) );
        if ( !update.digest().valid() ) return false;
        m_ownedLeaf.reset( new PropertyConfig{update} );
        m_leaf = m_ownedLeaf.get();
        return true;
    }

  private:
    ConfigTree* root()
    {
        return m_root;
    }

    Lookup& lookup()
    {
        assert( ( m_root == this && m_lookup.get() != 0 ) ||
                ( m_root != this && m_lookup.get() == 0 ) );
        return m_lookup.get() ? *m_lookup : root()->lookup();
    }

    unique_ptr<Lookup> m_lookup;
    unique_ptr<PropertyConfig> m_ownedLeaf;
    vector<ConfigTree*> m_parents;
    vector<ConfigTree*> m_deps;
    vector<ConfigTree*> m_ownedDeps;
    ConfigTree* m_root;
    const PropertyConfig* m_leaf;
    ConfigTreeNode::digest_type m_origDigest;
    string m_label;
};
///////////////////////////////////////////////////////////////////////////////////////////

// Declaration of the Tool Factory
DECLARE_TOOL_FACTORY( ConfigTreeEditor )

//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
ConfigTreeEditor::ConfigTreeEditor( string type, string name,
                                    const IInterface* parent )
    : base_class( std::move( type ), std::move( name ), parent )
    , m_propertyConfigSvc{nullptr}
    , m_configAccessSvc{nullptr}
{
    declareInterface<IConfigTreeEditor>( this );
    declareProperty( "ConfigAccessSvc",
                     s_configAccessSvc = "ConfigTarFileAccessSvc" );
    declareProperty( "PropertyConfigSvc",
                     s_propertyConfigSvc = "PropertyConfigSvc" );
}

//=============================================================================
// Initialization
//=============================================================================
StatusCode ConfigTreeEditor::initialize()
{
    info() << "Initialize" << endmsg;
    StatusCode status = GaudiTool::initialize();
    if ( !status.isSuccess() ) return status;
    m_configAccessSvc = svc<IConfigAccessSvc>( s_configAccessSvc );
    m_propertyConfigSvc = svc<IPropertyConfigSvc>( s_propertyConfigSvc );
    return StatusCode::SUCCESS;
}

//=============================================================================
ConfigTreeNode::digest_type ConfigTreeEditor::updateAndWrite(
    const ConfigTreeNode::digest_type& in,
    const multimap<string, pair<string, string>>& updates,
    const string& label ) const
{
    ConfigTree tree( in, *m_propertyConfigSvc, label );
    auto i = std::begin(updates);
    while ( i != std::end(updates) ) {
        ConfigTree* node = tree.findNodeWithLeaf( i->first );
        if ( !node ) {
            error() << " could not locate requested leaf with name " << i->first
                    << endmsg;
            return ConfigTreeNode::digest_type::createInvalid();
        }
        // grab entire range matching this one
        auto j = updates.upper_bound( i->first );
        vector<pair<string, string>> mods;
        std::transform( i, j, std::back_inserter( mods ), select2nd );
        i = j;
        node->updateLeaf( std::move(mods) );
    }
    return tree.write( *m_configAccessSvc );
}

# $Id: DetCondExample.gdb,v 1.4 2001-12-16 16:12:05 andreav Exp $
#-----------------------------------------------------------------------------
# HINT: use the Tab Key to see possible completions of a breakpoint
delete breakpoints

# Breakpoints during algorithm initialization
#break 'PopulateDB::initialize(void)'
#break 'EvtClock::initialize(void)'

# Breakpoints during algorithm execution
#break 'EvtClock::execute(void)'
break 'CondDBTestAlgorithm::execute(void)'
break 'XmlTestAlgorithm::execute(void)'

# Breakpoints during algorithm finalization
#break 'EvtClock::finalize(void)'

# The breakpoints in the catalog below refer to components from shared 
# libraries and can only be set after the shared libraries have been loaded
# => Set some of the breakpoints above, and run the program
run

#-----------------------------------------------------------------------------
# A catalog of useful breakpoints
#-----------------------------------------------------------------------------
#break 'ConditionsDBCnvSvc::updateObj(IOpaqueAddress *, DataObject *)' 
#break 'ConditionsDBCnvSvc::updateConditionData(DataObject *, basic_string<char, string_char_traits<char>, __default_alloc_template<true, 0> > const &, basic_string<char, string_char_traits<char>, __default_alloc_template<true, 0> > const &, basic_string<char, string_char_traits<char>, __default_alloc_template<true, 0> > const &, ITime const &, unsigned long const &, IRegistry *)' 
#break 'XmlGenericCnv::internalCreateObj(DOM_Element, DataObject *&, IOpaqueAddress *)' 
#break 'XmlBaseConditionCnv::initialize(void)'
#break 'XmlBaseConditionCnv::i_createObj(DOM_Element, DataObject *&)'
#break 'XmlBaseConditionCnv::i_fillObj(DOM_Element, DataObject *)'
#break 'ConditionInfo::condition(void)' 
#break 'SmartDataStorePtr<IUserParameterSet, SmartDataObjectPtr::ObjectLoader>::operator!(void)'
#break 'DataSvc::retrieveEntry(DataSvcHelpers::RegistryEntry *, basic_string<char, string_char_traits<char>, __default_alloc_template<true, 0> > const &, DataSvcHelpers::RegistryEntry *&)' 
#break 'Converter::addressCreator(void) const' 
#break 'XmlGenericCnv::i_fillObj(DOM_Element, DataObject *)'
#break 'XmlCatalogCnv::i_fillObj(DOM_Element, DataObject *)'
#break 'DataSvc::registerAddress(IRegistry *, basic_string<char, string_char_traits<char>, __default_alloc_template<true, 0> > const &, IOpaqueAddress *)'
#break 'PersistencySvc::createObj(IOpaqueAddress *, DataObject *&)'
#break 'PersistencySvc::fillObjRefs(IOpaqueAddress *, DataObject *)'
#break 'PersistencySvc::updateObj(IOpaqueAddress *, DataObject *)'
#break 'PersistencySvc::updateObjRefs(IOpaqueAddress *, DataObject *)'
#break 'PersistencySvc::makeCall(int, IOpaqueAddress *&, DataObject *&)'
#break 'ConversionSvc::createObj(IOpaqueAddress *, DataObject *&)'
#break 'ConversionSvc::updateObjRefs(IOpaqueAddress *, DataObject *)'
#break 'ConversionSvc::fillObjRefs(IOpaqueAddress *, DataObject *)'
#break 'ConversionSvc::updateObj(IOpaqueAddress *, DataObject *)'
#break 'ConversionSvc::makeCall(int, bool, bool, bool, IOpaqueAddress *&, DataObject *&)'




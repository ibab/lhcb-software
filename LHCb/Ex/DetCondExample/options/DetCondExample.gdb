# $Id: DetCondExample.gdb,v 1.3 2001-11-23 18:00:44 andreav Exp $
delete breakpoints

# Breakpoints during algorithm initialization
#break 'PopulateDB::initialize(void)'
#break 'EvtClock::initialize(void)'
#break 'TestConditionsDBDataSvc::initialize(void)'
#break 'TestSmartDataPtr::initialize(void)'

# Breakpoints during algorithm execution
break 'EvtClock::execute(void)'
break 'TestSmartDataPtr::execute(void)'
break 'TestConditionsDBDataSvc::execute(void)'

# Breakpoints during algorithm finalization
break 'EvtClock::finalize(void)'

# The following breakpoints refer to components from shared libraries
# and can only be set after the shared libraries have been loaded
# => Set some of the breakpoints above, and run the program
run
break 'PersistencySvc::createObj(IOpaqueAddress *, DataObject *&)'
break 'PersistencySvc::fillObjRefs(IOpaqueAddress *, DataObject *)'
break 'PersistencySvc::updateObj(IOpaqueAddress *, DataObject *)'
break 'PersistencySvc::updateObjRefs(IOpaqueAddress *, DataObject *)'
break 'PersistencySvc::makeCall(int, IOpaqueAddress *&, DataObject *&)'
break 'ConversionSvc::createObj(IOpaqueAddress *, DataObject *&)'
break 'ConversionSvc::fillObjRefs(IOpaqueAddress *, DataObject *)'
break 'ConversionSvc::updateObj(IOpaqueAddress *, DataObject *)'
break 'ConversionSvc::updateObjRefs(IOpaqueAddress *, DataObject *)'
break 'ConversionSvc::makeCall(int, bool, bool, bool, IOpaqueAddress *&, DataObject *&)'




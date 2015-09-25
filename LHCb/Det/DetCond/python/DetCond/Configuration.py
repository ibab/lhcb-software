"""
High level configuration tools for Conditions Database.
"""
__author__  = "Marco Clemencic <Marco.Clemencic@cern.ch>"

from Gaudi.Configuration import allConfigurables, ConfigurableUser, importOptions, getConfigurable, log
from Configurables import ( CondDBAccessSvc,
                            CondDBDispatcherSvc,
                            CondDBLayeringSvc,
                            CondDBTimeSwitchSvc,
                            CondDBCnvSvc,
                            CondDBSQLiteCopyAccSvc,
                            CondDBLogger,
                            COOLConfSvc,
                            ApplicationMgr )

import os, re
from os.path import exists, join

class CondDB(ConfigurableUser):
    """
    Configurable user to allow high-level configuration of the access to the
    conditions database.
    """
    __slots__ = { "Tags"        : {},
                  "Simulation"  : False,
                  "Upgrade"     : False,
                  "UseOracle"   : False,
                  "LocalTags"   : {},
                  "LogFile"     : "",
                  "Overrides"   : [],
                  "PartitionConnectionString": {},
                  "SQLiteLocalCopiesDir": "",
                  "OverwriteSQLiteLocalCopy": False,
                  "DisableLFC"  : False,
                  "Online"      : False,
                  "IgnoreHeartBeat": False,
                  "HeartBeatCondition" : "/Conditions/Online/LHCb/Tick",
                  "EnableRunStampCheck": False,
                  "RunStampCondition": "",
                  "LatestGlobalTagByDataType" : "",
                  "LatestGlobalTagByDataTypes" : [],
                  "LatestLocalTagsByDataType": [],
                  "AllLocalTagsByDataType": [],
                  "UseLatestTags" : [],
                  "QueryGranularity" : 0,
                  "UseDBSnapshot"     : False ,
                  "DBSnapshotDirectory" : "/group/online/hlt/conditions" ,
                  'EnableRunChangeHandler' : False,
                  'RunChangeHandlerConditions' : { 'online_%d.xml' :  [ "Conditions/Online/LHCb/Magnet/Set"
                                                                      , "Conditions/Online/Velo/MotionSystem"
                                                                      , "Conditions/Online/LHCb/Lumi/LumiSettings"
                                                                      , "Conditions/Online/LHCb/LHCFillingScheme"
                                                                      , "Conditions/Online/LHCb/RunParameters"
                                                                      , "Conditions/Online/Rich1/R1HltGasParameters"
                                                                      , "Conditions/Online/Rich2/R2HltGasParameters" ] },
                  'LoadCALIBDB' : "OFFLINE"
                  }
    _propertyDocDct = {
                       'Tags' : """ Dictionary of tags (partition:tag) to use for the COOL databases """,
                       'Simulation' : """ Boolean flag to select the simulation or real-data configuration """,
                       'Upgrade' : """ Boolean flag to select the Upgrade simulation configuration """,
                       'UseOracle' : """ Boolean flag to enable the usage of the CondDB from Oracle servers. NB: Obsolete as Oracle support is now dropped""",
                       'LocalTags' : """ Dictionary with local tags to use to override the global ones (partition: list of local tags) """,
                       'LogFile' : """ Record the requests to the database the specified file """,
                       'Overrides' : """ Internal property used to add layers or alternatives """,
                       'PartitionConnectionString' : """ Dictionary with alternative connection strings for the CondDB partitions """,
                       'SQLiteLocalCopiesDir' : """ The directory where to copy the SQLite files before accessing them """,
                       'OverwriteSQLiteLocalCopy' : """ When using SQLite local copies, overwrite existing files """,
                       'DisableLFC' : """ Do not use LFC lookup even if we are connecting to Oracle """,
                       'Online' : """ Flag to activate configuration options specific for the Online environment """,
                       'IgnoreHeartBeat' : """ Do not set the HeartBeatCondition for the Online partition """,
                       'HeartBeatCondition' : """ Location of the heart-beat condition in the database """,
                       'EnableRunStampCheck': """ Enable the check for the special RunStamp condition """,
                       'RunStampCondition': """ Path (in the CondDB) to the special RunsTamp condition. """,
                       'LatestGlobalTagByDataType' : """ Use latest CondDB global tag marked with the data type""",
                       'LatestGlobalTagByDataTypes' : """ Use latest CondDB global tag marked with the data type, will override LatestGlobalTagByDataType if set""",
                       'LatestLocalTagsByDataType' : """ Use all latest CondDB local tags marked with the data type """,
                       'AllLocalTagsByDataType' : """ Use all CondDB local tags marked with the data type """,
                       'UseLatestTags' : """ List of the form [DataType, OnlyGlobalTags = False] to turn on the usage of the latest tags """,
                       'QueryGranularity': """Granularity of the query in the database (in time units)""",
                       'LoadCALIBDB': """ Load CALIB*.db file as additional layers on top of ONLINE-*.db file, could be either "HLT1" (w/o layering, ONLINE only), or "OFFLINE" (Layer CALIBOFF above CALIB & ONLINE ), Do not set it for the Online environment""",
                       }
    LAYER = 0
    ALTERNATIVE = 1
    # List of known implementations of ICondDBReader (str is used for backward compatibility)
    __CondDBReaders__ = [ CondDBAccessSvc,
                          CondDBDispatcherSvc,
                          CondDBLayeringSvc,
                          CondDBTimeSwitchSvc,
                          CondDBSQLiteCopyAccSvc,
                          str ]

    def _checkOverrideArgs(self, accessSvc, connStr, dbFile, dbName):
        """
        Check if the accessSvc is a valid CondDBReader or build one using the
        other arguments.
        """
        kwargs = { "accessSvc": accessSvc, "connStr": connStr, "dbFile": dbFile, "dbName": dbName }
        if accessSvc is None:
            if not connStr:
                if dbFile:
                    if not dbName:
                        dbName = os.path.basename(dbFile)
                        m = re.match(r'([A-Z][A-Z0-9_]{0,7})(_\w+)?.db', dbName)
                        if m:
                            dbName = m.group(1)
                        else:
                            raise ValueError('invalid arguments %r' % kwargs)
                    connStr = "sqlite_file:%s/%s" % (dbFile, dbName)
                else:
                    raise ValueError('invalid arguments %r' % kwargs)
                name = dbName
            else:
                name = connStr.rsplit('/')[-1]
                if not re.match(r'[A-Z][A-Z0-9_]{0,7}', name):
                    name = 'CondDBAccessSvc'
            # make a unique name for the configurable
            name = "automatic_" + name
            name_format = name + '_%d'
            i = 0
            while name in allConfigurables:
                i += 1
                name = name_format % i
            accessSvc = CondDBAccessSvc(name, ConnectionString = connStr)
        elif type(accessSvc) not in __CondDBReaders__: # Check for supported types
            raise TypeError("'%s' not supported as CondDBReader"%accessSvc.__class__.__name__)
        return accessSvc

    def addLayer(self, accessSvc = None, connStr = None, dbFile = None, dbName = None):
        """
        Add the given CondDBReader as a layer on top of the existing configuration.

        Example:
        CondDB().addLayer(myDB)
        """
        # Check the arguments and/or prepare a valid access svc
        accessSvc = self._checkOverrideArgs(accessSvc, connStr, dbFile, dbName)
        self.Overrides.append((self.LAYER, accessSvc))
    def _addLayer(self, accessSvc):
        cnvSvc = allConfigurables["CondDBCnvSvc"]

        originalReader = cnvSvc.CondDBReader
        if type(originalReader) == CondDBLayeringSvc:
            # If the original reader is already a layering svc, we can extend the
            # configuration.
            originalReader.Layers.insert(0,accessSvc)
        else:
            # We have to create a new layering svc.
            name = "CondDBLayeringSvc"
            i = 0
            while name in allConfigurables:
                i += 1
                name = "CondDBLayeringSvc_%d"%i
            cnvSvc.CondDBReader = CondDBLayeringSvc(name,
                                                    Layers = [accessSvc,
                                                              originalReader])

    def addAlternative(self, accessSvc = None, path = None, connStr = None, dbFile = None, dbName = None):
        """
        Add the given CondDBReader as an alternative to the existing configuration
        for the specified path.

        Example:
        CondDB().addAlternative(myDB,"/Conditions/MyDetector/Alignment")
        """
        if path is None:
            raise ValueError("'path' must be specified")
        # Check the arguments and/or prepare a valid access svc
        accessSvc = self._checkOverrideArgs(accessSvc, connStr, dbFile, dbName)
        self.Overrides.append((self.ALTERNATIVE, accessSvc, path))

    def _addAlternative(self, accessSvc, path):
        cnvSvc = allConfigurables["CondDBCnvSvc"]

        originalReader = cnvSvc.CondDBReader
        if type(originalReader) == CondDBDispatcherSvc:
            # If the original reader is already a dispatcher, we can extend the
            # configuration:
            originalReader.Alternatives[path] = accessSvc
        else:
            # We have to create a new dispatcher
            name = "CondDBDispatcherSvc"
            i = 0
            while name in allConfigurables:
                i += 1
                name = "CondDBDispatcherSvc_%d"%i
            cnvSvc.CondDBReader = CondDBDispatcherSvc(name,
                                                      MainAccessSvc = originalReader,
                                                      Alternatives = { path: accessSvc }
                                                      )

    def useLatestTags(self, DataType, OnlyGlobalTags = False):
        self.UseLatestTags = [DataType, OnlyGlobalTags]

    def _useLatestTags(self, DataTypes, OnlyGlobalTags = False, OnlyLocalTags = False):
        """
        Configure the conditions database to use the latest local tags on top of the latest global tag for a given data type.
        """
        # Check arguments
        if type(DataTypes) is not list:
            DataTypes = [DataTypes]

        # Check if the latest tags should be set for simulation or not
        if not self.getProp("Simulation"):
            partitions = ["DDDB", "LHCBCOND", "DQFLAGS"]
            if os.environ['LoadCALIBDB'] is "OFFLINE":
                partitions += ["CALIBOFF"]
        else:
            partitions = ["DDDB", "SIMCOND"]

        # Set the latest tags
        from CondDBUI.Admin.TagsFilter import last_gt_lts
        rel_notes = None
        if self.getProp('Upgrade'):
            rel_notes = os.path.join(os.environ['SQLITEUPGRADEDBPATH'], '..', 'doc', 'release_notes.xml')

        for partition in partitions:
            gt, lts = None, []
            for dt in DataTypes:
                tags = last_gt_lts(partition, dt, rel_notes)
                if not tags:
                    # Allowing absence of valid tags for DQFLAGS
                    if partition == 'DQFLAGS': continue
                    # Allowing absence of valid tags for CALIBOFF before 2015 
                    elif partition == 'CALIBOFF' and dt.isdigit() and int(dt)<2015: continue
                    else:
                        raise RuntimeError("Cannot find tags for partition '%s',"
                                       " data type '%s'" % (partition, dt))
                if not gt:
                    gt = tags[0]
                lts += tags[1]

            if not OnlyLocalTags and gt: # Only for partitions with available global tags
                self.Tags[partition] = gt
            if not OnlyGlobalTags and lts:
                if partition in self.LocalTags: self.LocalTags[partition] += lts
                else: self.LocalTags[partition] = lts

    def _useAllLocalTags(self, DataTypes):
        """
        Configure the conditions database to use all local tags for a given data type.
        """

        # Check arguments
        if type(DataTypes) is not list:
            DataTypes = [DataTypes]

        # Check if the latest tags should be set for simulation or not
        if not self.getProp("Simulation"):
            partitions = ["DDDB", "LHCBCOND"]
            if os.environ['LoadCALIBDB'] is "OFFLINE":
                partitions += ["CALIBOFF"]
        else:
            partitions = ["DDDB", "SIMCOND"]

        # Set the latest tags
        from CondDBUI.Admin.TagsFilter import all_lts
        rel_notes = None
        if self.getProp('Upgrade'):
            rel_notes = os.path.join(os.environ['SQLITEUPGRADEDBPATH'], '..', 'doc', 'release_notes.xml')

        for partition in partitions:
            local_tags = []
            for dt in DataTypes:
                lts = all_lts(partition, dt, rel_notes)
                if lts: local_tags += lts
            if partition in self.LocalTags: self.LocalTags[partition] += local_tags
            else: self.LocalTags[partition] = local_tags

    def __make_sqlite_local_copy__(self, accsvc, local_dir = None, force_copy = None):
        if isinstance(accsvc, str):
            # convert the string in an actual configurable instance
            # This is both for backward compatibility and CondDBTimeSwitchSvc
            if "/" in accsvc:
                tp, name = accsvc.split("/",1)
            else:
                tp = name = accsvc
            accsvc = getConfigurable(name, tp)
        if local_dir is None:
            local_dir = self.getProp("SQLiteLocalCopiesDir")
        if force_copy is None:
            force_copy = self.getProp("OverwriteSQLiteLocalCopy")
        # If the directory for the local copies is not specified, we do nothing
        if not local_dir:
            return accsvc
        # Check if we are using Oracle or SQLite
#        if self.getProp("UseOracle"):
#            log.warning("Conflicting properties in CondDB Configurable: "
#                        "ignoring SQLiteLocalCopiesDir because UseOracle is set to True")
#            return accsvc
        # Modify partitions to use local copies of the DBs
        newaccsvc = accsvc # fallback return value (no change)
        if isinstance(accsvc, CondDBAccessSvc):
            # replace the reader with another
            m = re.match(r"^sqlite_file:(.*)/([_0-9A-Z]{1,8})$",
                         accsvc.getProp("ConnectionString"))
            if not m: # not SQLite connection string
                return accsvc
            newaccsvc = CondDBSQLiteCopyAccSvc(accsvc.name() + "_local")
            newaccsvc.OriginalFile = m.group(1)
            newaccsvc.DestinationFile = os.path.join(local_dir,
                                                     os.path.basename(m.group(1)))
            newaccsvc.DBName = m.group(2)
            newaccsvc.ForceCopy = force_copy
            newaccsvc.IgnoreCopyError = not force_copy # ignore copy errors if we do not overwrite (needed for local tags)
            if hasattr(accsvc, "CacheHighLevel"):
                newaccsvc.CacheHighLevel = accsvc.CacheHighLevel
        elif isinstance(accsvc, CondDBDispatcherSvc):
            # use the same dispatcher replacing its content
            mainAccSvc = accsvc.getProp("MainAccessSvc")
            accsvc.MainAccessSvc = self.__make_sqlite_local_copy__(mainAccSvc, local_dir)
            alternatives = accsvc.getProp("Alternatives")
            for alt in alternatives.keys():
                accsvc.Alternatives[alt] = \
                   self.__make_sqlite_local_copy__(alternatives[alt], local_dir)
        elif isinstance(accsvc, CondDBLayeringSvc):
            # use the same layering service replacing its content
            new_layers = []
            for layer in accsvc.getProp("Layers"):
                new_layers.append(self.__make_sqlite_local_copy__(layer, local_dir))
            accsvc.Layers = new_layers
        elif isinstance(accsvc, CondDBTimeSwitchSvc):
            # use the same time switcher replacing its content,
            # but we need to parse its options (in format "'%s':(%d,%d)")
            readers_list = accsvc.getProp("Readers")
            new_readers = []
            for line in readers_list:
                # Parse the line for the reader (it looks like "'name':(0,1)")
                r, iov = map(eval, line.rsplit(":"))
                new_reader = self.__make_sqlite_local_copy__(r, local_dir)
                new_readers.append("'%s':(%d,%d)" %
                                   (new_reader.getFullName(), iov[0], iov[1]))
            accsvc.Readers = new_readers
        elif isinstance(accsvc, CondDBLogger):
            # use the same logger replacing its content
            logged = accsvc.getProp("LoggedReader")
            accsvc.LoggedReader = self.__make_sqlite_local_copy__(logged, local_dir)
        elif isinstance(accsvc, CondDBCnvSvc):
            # use the same conversion service replacing its content
            reader = accsvc.getProp("CondDBReader")
            accsvc.CondDBReader = self.__make_sqlite_local_copy__(reader, local_dir)
        return newaccsvc

    def _configureDBSnapshot(self):

        baseloc = self.getProp( "DBSnapshotDirectory" )
        self.DisableLFC = True

        # Set alternative connection strings and tags
        # if simulation is False, we use DDDB, LHCBCOND and ONLINE
        #                  True          DDDB, SIMCOND
        # (see Det/DetCond's configurable... )
        dbPartitions = { False : [ "DDDB", "LHCBCOND", "ONLINE" ]
                       , True :  [ "DDDB", "SIMCOND" ]
                       }

        tag = self.getProp("Tags")
        for part in dbPartitions[ self.getProp('Simulation') ] :
            if tag[part] is 'default' : raise KeyError('must specify an explicit %s tag'%part)
            self.PartitionConnectionString[part] = "sqlite_file:%(dir)s/%(part)s_%(tag)s.db/%(part)s" % {"dir":  baseloc,
                                                                                                         "part": part,
                                                                                                         "tag":  tag[part]}
            self.Tags[part] = tag[part]

        # Set the location of the Online run-by-run conditions
        if self.getProp('EnableRunChangeHandler') :
            from Configurables import RunChangeHandlerSvc
            rch = RunChangeHandlerSvc()
            rch.Conditions = dict( (c,'/'.join([baseloc,f])) for f,cs in self.getProp("RunChangeHandlerConditions").iteritems() for c in cs )
            ApplicationMgr().ExtSvc.append(rch)

    def __apply_configuration__(self):
        """
        Converts the high-level information passed as properties into low-level configuration.
        """
        # special case for online
        if self.getProp('UseDBSnapshot') : self._configureDBSnapshot()

        # In the Online/Upgrade/Simulation environment, LoadCALIBDB should be defaulted to HLT1
        if self.getProp("Online") or self.getProp('Upgrade') or self.getProp('Simulation'):
            self._properties["LoadCALIBDB"].setDefault("HLT1")
        # Set up environment variables for loading CALIBOFF layers, must be before loading any tags
        LoadCALIBDB = self.getProp('LoadCALIBDB')
        loadcaliboptions = ["HLT1", "OFFLINE"]
        if LoadCALIBDB not in loadcaliboptions:
            raise ValueError("'%s' is not a valid LoadCALIBDB value. Allowed: %s" %(LoadCALIBDB, loadcaliboptions))
        if LoadCALIBDB is "OFFLINE" and not exists(join(os.environ["SQLITEDBPATH"], "CALIBOFF.db")):
            LoadCALIBDB = "HLT1" # When CALIBOFF.db is not there, reset the option
        os.environ['LoadCALIBDB'] = LoadCALIBDB

        # Set the usage of the latest global/local tags
        old_latest_Tags_prop = self.getProp("UseLatestTags") # it is deprecated
        latest_GTags_prop = self.getProp("LatestGlobalTagByDataTypes")
        if not latest_GTags_prop: # if property not set
            latest_GTags_prop = self.getProp("LatestGlobalTagByDataType")
        latest_LTags_prop = self.getProp("LatestLocalTagsByDataType")
        all_LTags_prop = self.getProp("AllLocalTagsByDataType")

        if old_latest_Tags_prop:
            if latest_GTags_prop or latest_LTags_prop:
                log.warning("The property 'UseLatestTags' is deprecated:"
                            "'LatestGlobalTagByDataType(s)' and 'LatestLocalTagsByDataType'"
                            " will be used instead.")
            else:
                latest_GTags_prop = old_latest_Tags_prop[0]
                if type(old_latest_Tags_prop[-1]) != bool or \
                   (type(old_latest_Tags_prop[-1]) == bool and not old_latest_Tags_prop[1]):
                    latest_LTags_prop = old_latest_Tags_prop[0]

        if latest_GTags_prop:
            datatype = latest_GTags_prop
            if self.getProp("Tags"):
                self.Tags = {}
            self._useLatestTags(datatype, OnlyGlobalTags = True)
            log.warning("Default global tags will be overridden with the latest ones"
                        " available for '%s' data type: %s"%(datatype, self.getProp("Tags")) )

        if latest_LTags_prop:
            datatypes = latest_LTags_prop
            #if self.getProp("LocalTags"):
            #    self.LocalTags = {}
            self._useLatestTags(datatypes, OnlyLocalTags = True)
            log.warning("Latest unbound local tags on top of the latest global tags"
                        " of %s data type(s) are added: %s"
                        %(datatypes, self.getProp("LocalTags")))

        if all_LTags_prop:
            datatypes = all_LTags_prop
            self._useAllLocalTags(datatypes)
            log.warning("ALL local tags of %s data type(s) are added: %s"
                        %(datatypes, self.getProp("LocalTags")))

        # Import SQLDDDB specific info
        if self.getProp("UseOracle") or self.getProp("UseDBSnapshot"):
#            importOptions("$SQLDDDBROOT/options/SQLDDDB-Oracle.py")
            CondDBAccessSvc("ONLINE", ConnectionString = "CondDBOnline/ONLINE")
            if self.getProp("DisableLFC"):
                COOLConfSvc(UseLFCReplicaSvc = False)
            if self.getProp("Online"):
                COOLConfSvc(UseLFCReplicaSvc = False)
                CORAL_XML_DIR = "/group/online/condb_viewer"
                ApplicationMgr().Environment["CORAL_AUTH_PATH"] = CORAL_XML_DIR
                ApplicationMgr().Environment["CORAL_DBLOOKUP_PATH"] = CORAL_XML_DIR
                
        else:
            configureOnlineSnapshots()
#            importOptions("$SQLDDDBROOT/options/SQLDDDB.py")

        #########################################################################
        # Access to ConditionsDB
        ##########################################################################
        conns = self.getProp("PartitionConnectionString")
        tags = self.getProp("Tags")
        # DB partitions
        partition = {}
        parttypes = [ ("DDDB",     CondDBAccessSvc),
                      ("LHCBCOND", CondDBAccessSvc),
                      ("ONLINE",   CondDBTimeSwitchSvc),
                      ("SIMCOND",  CondDBAccessSvc),
                      ("DQFLAGS",  CondDBAccessSvc)]
        if LoadCALIBDB is "OFFLINE":
        # CALIBOFF not needed for the upgrade
            parttypes += [("CALIBOFF", CondDBAccessSvc)]

        for (p ,t) in parttypes:
            partition[p] = getAnyDBReader(p, t)
            # Override connection strings:
            if p in conns:
                if type(partition[p]) is CondDBAccessSvc:
                    partition[p].ConnectionString = conns[p]
                    del conns[p]

            # Override connection strings for Upgrade case
            if self.getProp('Simulation') and self.getProp('Upgrade') and type(partition[p]) is CondDBAccessSvc:
                partition[p].ConnectionString = os.path.join('sqlite_file:$SQLITEUPGRADEDBPATH', p + '.db', p)
            # Override tags
            if p in tags and p != "ONLINE":
                partition[p].DefaultTAG = tags[p]
                del tags[p]
            # Set the query granularity
            if p != "CALIBOFF": self.propagateProperty("QueryGranularity", partition[p])
            if type(partition[p]) is CondDBTimeSwitchSvc: # also online
                for r in partition[p].Readers:
                    config = allConfigurables[eval(r.split(':')[0]).split("/")[1]]
                    if isinstance(config, CondDBAccessSvc):
                        self.propagateProperty("QueryGranularity", config)
                    # Pass along the configuration for the layered DBs
                    elif isinstance(config, CondDBLayeringSvc):
                        for ly in config.Layers:
                            if isinstance(ly, CondDBAccessSvc):
                                self.propagateProperty("QueryGranularity", ly)

        if conns:
            log.warning("Cannot override the connection strings of the partitions %r", conns.keys())
        if tags and tags.keys()!=['ONLINE']:
            log.warning("Cannot set the tag for partitions %r", tags.keys())

        # In the Online environment, IgnoreHeartBeat should be defaulted to True
        if self.getProp("Online"):
            self._properties["IgnoreHeartBeat"].setDefault(True)
        if not self.getProp("IgnoreHeartBeat"):
            if isinstance(partition["ONLINE"], CondDBAccessSvc):
                self.propagateProperty("HeartBeatCondition", partition["ONLINE"])
            elif isinstance(partition["ONLINE"], CondDBTimeSwitchSvc):
                # Add the heart beat conditions to the latest snapshot only since the
                # others are limited but valid by construction.
                if partition["ONLINE"].Readers:
                    latest = partition["ONLINE"].Readers[-1]
                    config = allConfigurables[eval(latest.split(':')[0]).split("/")[1]]
                    if isinstance(config, CondDBAccessSvc):
                        self.propagateProperty("HeartBeatCondition", config)
                    # Pass along the configuration for the layered DBs
                    elif isinstance(config, CondDBLayeringSvc):
                        for ly in config.Layers:
                            #Only apply HeartBeatCondition for ONLINE
                            if isinstance(ly, CondDBAccessSvc) and ly.getName().startswith("ONLINE_"):
                                self.propagateProperty("HeartBeatCondition", ly)

        if not self.getProp("Simulation"):
            # Standard configurations
            #  - Reconstruction / analysis
            disp = CondDBDispatcherSvc("MainCondDBReader",
                                       MainAccessSvc = partition["DDDB"],
                                       Alternatives = {
                                         "/Conditions": partition["LHCBCOND"],
                                         "/Conditions/Online": partition["ONLINE"],
                                         "/Conditions/DQ": partition["DQFLAGS"]
                                       })
        else:
            #  - Simulation
            disp = CondDBDispatcherSvc("SimulationCondDBReader",
                                       MainAccessSvc = partition["DDDB"],
                                       Alternatives = {
                                         "/Conditions": partition["SIMCOND"]
                                       })
        CondDBCnvSvc( CondDBReader = disp )

        if not (self.getProp("Online") or self.getProp("Simulation")):
            self._properties["EnableRunStampCheck"].setDefault(True)
        if self.getProp("EnableRunStampCheck"):
            from Configurables import RunStampCheck
            rsc = RunStampCheck()
            self.propagateProperty("RunStampCondition", rsc)
            ApplicationMgr().ExtSvc.append(rsc)

        # Load the CALIBOFF layer above everything if it exists
#        if len([x for x in parttypes if x[0] == 'CALIBOFF']):
#            self._addLayer(getAnyDBReader('CALIBOFF'))

        localTags = self.getProp("LocalTags")
        not_applied = []
        for p in localTags:
            if p in partition:
                taglist = list(localTags[p])
                taglist.reverse() # we need to stack the in reverse order to use the first as on top of the others
                i = 0 # counter
                if p is "CALIBOFF":
                    if LoadCALIBDB is not "OFFLINE":
                        raise ValueError("invalid argument LoadCALIBDB set at '%s' instead of 'OFFLINE' for accessing local tags for CALIBOFF.db" % LoadCALIBDB)
                    pcolayers = []
                    for t in taglist:
                        pcolayers.append(partition[p].clone("CALIBOFF_%d" %i, DefaultTAG = t))
                        i += 1
                        for r in partition["ONLINE"].Readers:
                            config = allConfigurables[eval(r.split(':')[0]).split("/")[1]]
                            if isinstance(config, CondDBLayeringSvc):
                                config.Layers = pcolayers + config.Layers
                elif type(partition[p]) is not CondDBTimeSwitchSvc:
                    for t in taglist:
                        self._addLayer(partition[p].clone("%s_%d" % (p, i),
                            DefaultTAG = t))
                        i += 1
                else:
                    not_applied.append(p)
            else:
                not_applied.append(p)
        if not_applied:
            log.warning("Cannot set the local tags for partitions %r", not_applied)

        # Modify partitions to use local copies of the DBs
        # before adding user layers and alternatives, which should be already local.
        # This is a no-operation if the property is not set
        self.__make_sqlite_local_copy__(CondDBCnvSvc())

        # Add layers and alternatives
        call = { self.LAYER : self._addLayer,
                 self.ALTERNATIVE : self._addAlternative }
        for override in self.getProp("Overrides"):
            apply(call[override[0]], override[1:])

        # Add the logger
        filename = self.getProp("LogFile")
        if filename:
            cnvSvc = allConfigurables["CondDBCnvSvc"]
            cnvSvc.CondDBReader = CondDBLogger(LoggedReader = cnvSvc.CondDBReader,
                                               LogFile = filename)

        # Suppress pointless warning from COOL_2_5_0
        msgSvc = getConfigurable("MessageSvc")
        msgSvc.setError.append("RelationalDatabase")

        # Set up Virtual File System service, can be used by ParticlePropertySvc
        from Gaudi.Configuration import VFSSvc
        from Configurables       import CondDBEntityResolver
        VFSSvc().FileAccessTools.append(CondDBEntityResolver())


# Exported symbols
__all__ = [ "addCondDBLayer", "addCondDBAlternative", "useCondDBLogger",
            "configureOnlineSnapshots" ]
# Configurables provided by the package
__all__ += [ "CondDBAccessSvc",
             "CondDBDispatcherSvc", "CondDBLayeringSvc", "CondDBTimeSwitchSvc",
             "CondDBSQLiteCopyAccSvc", "CondDBLogger",
             "CondDBCnvSvc",
             "CondDB" ]

# List of known implementations of ICondDBReader (str is used for backward compatibility)
__CondDBReaders__ = [ CondDBAccessSvc,
                      CondDBDispatcherSvc,
                      CondDBLayeringSvc,
                      CondDBTimeSwitchSvc,
                      CondDBSQLiteCopyAccSvc,
                      str ]

def _assertConfig(funcname):
    """
    Check if the default configuration has been loaded.
    """
    if "CondDBCnvSvc" not in allConfigurables:
        raise RuntimeError("You cannot call '%s' before the standard CondDB configuration"%funcname)

def addCondDBLayer(accessSvc):
    """
    Add the given CondDBReader as a layer on top of the existing configuration.

    Example:
    addCondDBLayer(myDB)
    """
    DetCond().addLayer(accessSvc)

def addCondDBAlternative(accessSvc, path):
    """
    Add the given CondDBReader as an alternative to the existing configuration
    for the specified path.

    Example:
    addCondDBAlternative(myDB,"/Conditions/MyDetector/Alignment")
    """
    DetCond().addAlternative(accessSvc, path)

def useCondDBLogger(filename = None, logger = None):
    """
    Add the CondDBLogger to the chain of CondDBReaders.

    The simplest usage is to call the function without options (use defaults), or
    pass a file name.
    """
    _assertConfig('useCondDBLogger')
    cnvSvc = allConfigurables["CondDBCnvSvc"]
    if logger is None:
        # use the default configuration
        cnvSvc.CondDBReader = CondDBLogger(LoggedReader = cnvSvc.CondDBReader)
    elif type(logger) is CondDBLogger:
        # use the user-provided configurable
        logger.LoggedReader = cnvSvc.CondDBReader
        cnvSvc.CondDBReader = logger
    else:
        raise TypeError("useCondDBLogger does not support '%s'"%logger.__class__.__name__)
    # Use the user specified filename, if any
    if filename:
        cnvSvc.CondDBReader.LogFile = filename

def _timegm(t):
    """Inverse of time.gmtime. Implementation from Gaudi::Time."""
    import time
    if t[8] != 0: # ensure that dst is not set
        t = tuple(list(t[0:8]) + [0])
    t1 = time.mktime(t)
    gt = time.gmtime(t1)
    t2 = time.mktime(gt)
    return t1 + (t1 - t2)

def defConnStrFunc(ym_tuple):
    return self.connStrOnline(ym_tuple)

def connStrOnline(ym_tuple):
    dbpath = os.environ["SQLITEDBPATH"]
    if exists(join(dbpath, "ONLINE-%04d%02d.db" % ym_tuple)):
        return "sqlite_file:$SQLITEDBPATH/ONLINE-%04d%02d.db/ONLINE" % ym_tuple
    return "sqlite_file:$SQLITEDBPATH/ONLINE-%04d.db/ONLINE" % ym_tuple[0]

def getAnyDBReader(layer = 'CALIBOFF', svc = CondDBAccessSvc):
    CacheHighLevel = 200
    if layer == 'DDDB' : CacheHighLevel = 1700
    # Put the discovered layer on top
    cfg = getConfigurable(layer, svc)
    if svc is not CondDBAccessSvc: return cfg
    try: cfg.ConnectionString
    except AttributeError: # Set up connection for the 1st time
        connstr = "sqlite_file:$SQLITEDBPATH/%s.db/%s" % (layer, layer)
        if layer == 'DQFLAGS' :
            cfg = CondDBAccessSvc(layer, ConnectionString = connstr,
                    CacheLowLevel = 5, CacheHighLevel = 10)
        else:
            cfg = CondDBAccessSvc(layer, ConnectionString = connstr,
                CacheHighLevel = CacheHighLevel)
    return cfg

def getOnlineDBReader(ym_tuple, granularity = 'YEARLY', connStrFunc = None):
    cnstr = ''
    ymstr = ''
    if granularity == 'YEARLY':
        ymstr = "%04d" %ym_tuple[0]
        cnstr = connStrFunc((ym_tuple[0],13))
    else:
        ymstr = "%04d%02d" %ym_tuple
        cnstr = connStrFunc(ym_tuple)

    ptnm = "ONLINE_" + ymstr
    accSvc = CondDBAccessSvc(ptnm, ConnectionString = cnstr)
    dblayers = [ accSvc ]
    LoadCALIBDB = os.environ.get('LoadCALIBDB')
    if ym_tuple[0] < 2015 or LoadCALIBDB is not "OFFLINE": # For datatype before 2015, no CALIBOFF layer is needed
        return accSvc
    dbpath = os.environ["SQLITEDBPATH"]
    layer = 'CALIBOFF'
    if exists(join(dbpath, layer + '.db')):
        # Put the discovered layer on top
        cfg = getConfigurable(layer, CondDBAccessSvc)
        try: cfg.ConnectionString
        except AttributeError: # Set up connection for the 1st time
            cfg = CondDBAccessSvc("CALIBOFF", ConnectionString =
                    cnstr.replace('ONLINE-%s.db/ONLINE' %ymstr, "%s.db/%s" % (layer, layer)), CacheHighLevel = 200)
        dblayers.insert(0, cfg)

    if (len(dblayers) == 1): return accSvc # In case no CALIBOFF.db is found
    return CondDBLayeringSvc("ONLINELAYER_"+ymstr, Layers = dblayers )

def configureOnlineSnapshots(start = None, end = None, connStrFunc = None):
    if connStrFunc is None:
        connStrFunc = connStrOnline

    # prepare the configurable instance
    ONLINE = CondDBTimeSwitchSvc("ONLINE")

    # Default snapshots granularity
    granularity = 'YEARLY'

    # Set the first available snapshot pair: per-year by default
    if start is None:
        first_snapshot = (2008, None)
    else:
        first_snapshot = start

    # Set the last available snapshot pair: current year by default
    if end is None:
        import time
        last_snapshot = (time.gmtime()[0], None)
    else:
        last_snapshot = end

    # If last snapshot is per-month switch the first one to be also per-month
    if last_snapshot[1]:
        granularity = 'MONTHLY'
        first_snapshot = (2008, 6)

    # reset the list of readers, for safety
    ONLINE.Readers = []
    # loop from first to last-1
    i = first_snapshot
    until = 0 # this makes the first service used from times starting from 0
    while i < last_snapshot:
        accSvc = getOnlineDBReader(i, granularity, connStrFunc)
        since = until
        # increment
        if granularity == 'YEARLY':
            i = (i[0]+1, None)
            until = int(_timegm(tuple([i[0], 1, 1, 0, 0, 0, 0, 0, 0]))) * 1000000000
        else:
            if i[1] == 12: i = (i[0]+1,1)
            else: i = (i[0],i[1]+1)
            until = int(_timegm(tuple([i[0], i[1], 1, 0, 0, 0, 0, 0, 0]))) * 1000000000
        descr = "'%s':(%d,%d)" % ( accSvc.getFullName(), since, until )
        ONLINE.Readers.append(descr)

    # append the last database with validity extended to the maximum validity
    accSvc = getOnlineDBReader(i, granularity, connStrFunc)
    since = until
    until = 0x7fffffffffffffffL # Defined in PyCool.cool as ValidityKeyMax
    descr = "'%s':(%d,%d)" % ( accSvc.getFullName(), since, until )
    ONLINE.Readers.append(descr)



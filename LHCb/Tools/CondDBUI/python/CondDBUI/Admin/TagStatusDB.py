#!/usr/bin/env python

__author__ = "Illya Shapoval <illya.shapoval@cern.ch>, Marco Clemencic <marco.clemencic@cern.ch>"
__version__ = "$Id: TagStatusDB.py,v 1.0 2010-10-25 00:00:00 ishapova Exp $"

import os, sys
from storm.locals import *
from datetime import datetime, timedelta

class Tag(Storm):
    """ Class represents tag in the TagStatus database.
    """
    __storm_table__ = "tags"
    __storm_primary__ = "id"
    id = Int(primary=True)
    name = Unicode()
    partition = Unicode()
    creation_time = DateTime()
    hash_sum = Unicode()
    hash_alg = Unicode()
    status_set = ReferenceSet(id, "TagStatus.tag_id")

    def __init__(self, name, partition, creation_time, hash_sum, hash_alg):
        """
        @param name: tag name
        @param partition: CondDB partition (DDDB, LHCBCOND, SIMCOND)
        @param creation: time when the tag was created
        """

        self.name = name
        self.partition = partition
        self.creation_time = creation_time
        self.hash_sum = hash_sum
        self.hash_alg = hash_alg


class TagStatus(Storm):
    """ Class represents tag status in the TagStatus database.
    """
    __storm_table__ = "tags_status"
    __storm_primary__ = "tag_id", "site"
    tag_id = Int()
    tag = Reference(tag_id, "Tag.id")
    site = Unicode()
    last_ok_time = DateTime()

    def __init__(self, site, last_ok_time = datetime(1970,1,1)):
        """
        @param site: site name (LCG.CERN.ch, LCG.CNAF.it, ...)
        @param last_ok: last time the tag was found to be correct
        """

        self.site = site
        self.last_ok_time = last_ok_time

    def dumpInfo(self):
        return {
                'TagID':self.tag_id,
                'TagName':self.tag.name,
                'Partition':self.tag.partition,
                'Site':self.site,
                'Last_ok_time':self.last_ok_time,
                'Master_hash_sum':self.tag.hash_sum,
                'Hash_alg':self.tag.hash_alg
                }


class TagStatusDB(object):
    """ Class to interact with the TagStatusDB designed to monitor the CondDB global tag availability status."""

    def __init__(self, db_path, create_new_db = False, recheck = timedelta(7), expiration = timedelta(30)):
        """
        @param db_path: database path to connect to or create new one
        @param recheck: time interval after which the tag must be re-checked
        @param expiration: time interval after which a tag doesn't need to be checked again
        """
        if create_new_db and os.path.exists(db_path):
            raise Exception, "Requested path for the new db at '%s' contains already some db."%db_path

        self.openDatabase(db_path,create_new_db)
        if create_new_db:
            self.createTagsTable()
            self.createTagStatusTable()

        self.recheck = recheck
        self.expiration = expiration
        self.start_test_time_boundary = datetime.now()-self.recheck
        self.stop_test_time_boundary = datetime.now()-self.expiration

    def openDatabase(self, db_path, create_new_db = False):
        """Connects to the db at db_path (or creates new one if requested) and opens its store."""

        if os.path.exists(db_path.split(":")[-1]) or create_new_db:
            self.db = create_database(db_path)
        else:
            raise Exception, "No database found at %s" %db_path

        self.store = Store(self.db)

    def createTagsTable(self):
        """Creates empty 'TAGS' table"""

        self.store.execute("CREATE TABLE TAGS (id INTEGER PRIMARY KEY, "
                           "name VARCHAR UNIQUE, partition VARCHAR UNIQUE, "
                           "creation_time DATETIME, hash_sum VARCHAR UNIQUE, "
                           "hash_alg VARCHAR);")

    def createTagStatusTable(self):
        """Creates empty 'TAGS_BULLETINS' table"""

        self.store.execute("CREATE TABLE TAGS_STATUS (tag_id INTEGER, site VARCHAR,"
                           "last_ok_time DATETIME, PRIMARY KEY (tag_id,site),"
                           "FOREIGN KEY(tag_id) REFERENCES tags(id));")

    def addRow(self, row):
        """Adds new row to the in-memory-db.

        @param row: Instance. Instance of a row to be found in the db.
        """

        self.store.add(row)

    def getTag(self, tag, partition):
        """Finds row in the table 'TAGS' and returns Tag instance.

        @param tag: String. Tag name to be found.
        @param partition: String. Partition name to look the tag in.
        """

        return self.store.find(Tag,Tag.name == tag, Tag.partition == partition).one()

    def getTagStatus(self, tag, partition, site):
        """Finds row in the table 'TAG_BULLETINS' and returns TagStatus instance.

        @param tag: String. Tag name to be found.
        @param partition: String. Partition name to look the tag in.
        """
        status_set = self.getTag(tag,partition).status_set
        for status in status_set:
            if status.site == site: return status
            else: return None

    def resolveRow(self, row):
        """Resolves if the row (can be Tag or TagStatus) is in the db or not.

        @param row: Instance. Instance of a row to be found in the db.
        """

        if self.store.of(row) is store: return True
        else: return False

    def getGoodTags(self, site, partition):
        """ Get the list of tags that are believed to be good:
            now < tag.created + self.expiration &&
            now < tag.last_ok + self.recheck

        Returns a list of Tag objects.
        """

        valid_status_set = self.store.find(TagStatus,
                                           TagStatus.site == site,
                                           TagStatus.last_ok_time > self.start_test_time_boundary)

        return [status for status in valid_status_set if status.tag.partition == partition]

    def getGoodSites(self, tag, partition):
        """ Get the list of sites that are believed to contain valid tag:
            now < tag.created + self.expiration &&
            now < tag.last_ok + self.recheck

        Returns a list of sites.
        """

        tagObj = self.getTag(tag,partition)
        if tagObj:
            return [status.site for status in tagObj.status_set
                    if status.last_ok_time > self.start_test_time_boundary]
        else:
            return []

    def getTagsToCheck(self, site, partition):
        """ Get list of tags that requires to be checked:
            now < tag.created + self.expiration &&
            now > tag.last_ok + self.recheck

        Returns a list of TagStatus objects.
        """

        actual_tags = self.store.find(Tag,
                                      Tag.partition == partition,
                                      Tag.creation_time > self.stop_test_time_boundary)
        tag_status_to_check = []
        for tag in actual_tags:
            for status in tag.status_set:
                if status.site == site and status.last_ok_time < self.start_test_time_boundary:
                    tag_status_to_check.append(status)
        return tag_status_to_check

    def setAsGood(self, tags_status):
        """ Flag the tags as successfully checked.
        The last_ok_time field of each of the tags is set to the current time.

        @param tags: list of TagStatus objects
        """

        for status in tags_status:
            status_to_modify = self.store.find(TagStatus,
                                               TagStatus.tag_id == status.tag_id,
                                               TagStatus.last_ok_time == status.last_ok_time).one()
            status_to_modify.last_ok_time = datetime.now()

    def write(self):
        """Commits changes to the db."""

        self.store.commit()
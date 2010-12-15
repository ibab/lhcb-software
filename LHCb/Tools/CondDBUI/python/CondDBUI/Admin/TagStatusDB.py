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
    time = DateTime()
    status = Unicode()

    def __init__(self, site, time = datetime(1970,1,1)):
        """
        @param site: site name (LCG.CERN.ch, LCG.CNAF.it, ...)
        @param time: last time the tag was checked
        """

        self.site = site
        self.time = time
        self.status = u"BAD"

    def dumpInfo(self):
        """Dumps all information about a tag status entry."""

        return {
                'TagID':self.tag_id,
                'TagName':self.tag.name,
                'Partition':self.tag.partition,
                'Site':self.site,
                'Time':self.time,
                'Status':self.status,
                'ReferenceHashSum':self.tag.hash_sum,
                'HashAlg':self.tag.hash_alg
                }


class TagStatusDB(object):
    """ Class to interact with the TagStatusDB designed to monitor the CondDB global tag availability status."""

    def __init__(self, db_path, recheck = timedelta(7), expiration = timedelta(30),
                 verification_latency = timedelta(1)):
        """
        @param db_path: database path to connect to
        @param recheck: time interval after which the tag must be re-checked
        @param expiration: time interval after which a tag doesn't need to be checked again
        """
        if not os.path.exists(db_path.split(":")[-1]):
            raise Exception, "No TSDB found at '%s'."%db_path

        self.openDatabase(db_path)

        self.recheck = recheck
        self.expiration = expiration
        self.start_test_time_boundary = datetime.now()-self.recheck
        self.stop_test_time_boundary = datetime.now()-self.expiration
        self.verification_latency = verification_latency
        self.latency_boundary = self.start_test_time_boundary - self.verification_latency
        self.sites = [
                      u"LCG.IN2P3.fr",
                      u"LCG.PIC.es",
                      u"LCG.CERN.ch",
                      u"LCG.CNAF.it",
                      u"LCG.RAL.uk",
                      u"LCG.GRIDKA.de",
                      u"LCG.SARA.nl"
                      ]

    def openDatabase(self, db_path):
        """Connects to the TSDB at db_path and builds its store."""

        if not os.path.exists(db_path.split(":")[-1]):
            raise Exception, "No TSDB found at '%s'." %db_path

        self.db = create_database(db_path)
        self.store = Store(self.db)

    def addRow(self, row):
        """Adds new row to the in-memory-db.

        @param row: Instance. Instance of a row to be found in the db.
        """

        self.store.add(row)

    def getTag(self, tag, partition):
        """Finds a row in the table 'TAGS' and returns Tag instance.

        @param tag: String. Tag name to be found.
        @param partition: String. Partition name to look the tag in.
        """

        return self.store.find(Tag,Tag.name == tag, Tag.partition == partition).one()

    def getTagStatus(self, tag, partition, site):
        """Finds a row in the table 'TAGS_STATUS' and returns TagStatus instance.

        @param tag: String. Tag name to be found.
        @param partition: String. Partition name to look the tag in.
        """
        status_set = self.getTag(tag,partition).status_set
        for status in status_set:
            if status.site == site: return status
        return None

    def resolveRow(self, row):
        """Resolves if a row (can be Tag or TagStatus) is in the db or not.

        @param row: Instance. Instance of a row to be found in the db.
        """

        if self.store.of(row) is store: return True
        else: return False

    def getGoodTags(self, site, partition):
        """ Get the list of tags that are believed to be good:
            now < tag.created + self.expiration &&
            now < tag.last_ok + self.recheck &&
            status is 'GOOD'

        Returns a list of Tag objects.
        """

        valid_status_set = self.store.find(TagStatus,
                                           TagStatus.site == site,
                                           TagStatus.time > self.start_test_time_boundary,
                                           TagStatus.status == u'GOOD')

        return [status for status in valid_status_set if status.tag.partition == partition]

    def getGoodSites(self, tag, partition):
        """ Get the list of sites that are believed to contain valid tag:
            now < tag.created + self.expiration &&
            now < tag.time + self.recheck &&
            status is 'GOOD'

        Returns a list of sites.
        """

        tagObj = self.getTag(tag,partition)
        if tagObj:
            return [status.site for status in tagObj.status_set
                    if status.time > self.start_test_time_boundary and status.status == u'GOOD']
        else:
            return []

    def getTagsToCheck(self, site, partition, includeTagsBeingChecked = True):
        """ Get list of tags that requires to be checked:
            now < tag.created + self.expiration &&
            now > tag.time + self.recheck &&
            status is 'BAD'

        Returns a list of TagStatus objects.
        """

        actual_tags = self.store.find(Tag,
                                      Tag.partition == partition,
                                      Tag.creation_time > self.stop_test_time_boundary)
        tag_status_to_check = []
        if includeTagsBeingChecked:
            for tag in actual_tags:
                for status in tag.status_set:
                    if status.site == site and (status.time < self.start_test_time_boundary or
                                                status.status == u'BAD'):
                        tag_status_to_check.append(status)
        else:
            for tag in actual_tags:
                for status in tag.status_set:
                    if status.site == site and (status.time < self.latency_boundary or
                                                status.status == u'BAD'):
                        tag_status_to_check.append(status)
        return tag_status_to_check

    def getTagsBeingChecked(self, site, partition):
        """ Get list of tags that are *believed* being checked right now:
            now < tag.created + self.expiration &&
            now > tag.time + self.recheck

        Returns a list of TagStatus objects.
        """

        actual_tags = self.store.find(Tag,
                                      Tag.partition == partition,
                                      Tag.creation_time > self.stop_test_time_boundary)
        half_way_tags = []
        for tag in actual_tags:
            for status in tag.status_set:
                if status.site == site and \
                self.latency_boundary < status.time < self.start_test_time_boundary:
                    half_way_tags.append(status)
        return half_way_tags

    def setAsGood(self, tags_status):
        """ Flag the tags as successfully checked.
        - The 'time' field (last check time) of each tag which has been checked is set to the current time.
        - The 'status' field  of a tag is updated to its new just discovered status.

        @param tags_status: list of TagStatus objects to be set as good.
        """

        for status in tags_status:
            status_to_modify = self.store.find(TagStatus,
                                               TagStatus.tag_id == status.tag_id,
                                               TagStatus.site == status.site).one()
            status_to_modify.time = datetime.now()
            status_to_modify.status = u"GOOD"

    def write(self):
        """Commits changes to the db."""

        self.store.commit()
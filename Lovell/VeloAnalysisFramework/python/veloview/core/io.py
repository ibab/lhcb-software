"""This module implements an interface to the DQ Database"""

from ..utils.utils import flatten
import os
import sqlite3
import time

class DQDB(object):
    """Interface for versioned objects implemented using a SQLite DB."""

    def __init__(self, fname, mode = 'read'):
        if not isinstance(fname, str):
            raise ValueError('File name must be a string')

        self.mode = mode.lower()
        if self.mode == 'new':
            self.mode = 'create' #internally store 'new' as 'create' for easier parsing

        if self.mode not in ('create', 'read', 'update'):
            raise ValueError('File mode {} not allowed, must be one of create, new, read or update'.format(self.mode))

        if self.mode == 'create' and os.path.isfile(fname):
            raise ValueError("Can't create existing file {}".format(fname))

        self.__connect(fname)

        if self.mode == 'create':
            self.__create_db()

    def __connect(self, fname):
        self._connection = sqlite3.connect(fname)

    def __create_db(self):
        c = self.__get_cursor()
        c.execute('CREATE TABLE runs(runnr integer PRIMARY KEY, timestamp integer, runendtimestamp integer, comment text NOT NULL DEFAULT "")')
        c.execute('CREATE TABLE entries(timestamp integer NOT NULL, runnr integer NOT NULL)')
        self.__commit()

    def __get_cursor(self):
        return self._connection.cursor()

    def __commit(self):
        self._connection.commit()

    @classmethod
    def __sanitize_column_name(cls, name):
        """Check there's no backticks in the column name, as those could escape
        the SQL string."""
        assert('`' not in name)

    def fill(self, runnr, dqdict):
        """Flatten and fill dictionary"""
        if self.mode == 'read':
            raise RuntimeError('Cannot fill database in READ mode')

        now = time.time()
        runendtimestamp = dqdict.pop("endtimestamp")
        dqflat = flatten(dqdict)

        c = self.__get_cursor()

        vars = dqflat.keys()
        existing = self.vars()
        for var in vars:
            if not var in existing:
                self.__sanitize_column_name(var)
                # The default DEFAULT is NULL, so all old entries will have NULL in this column
                c.execute('ALTER TABLE entries ADD COLUMN `{}` REAL'.format(var))
                existing.append(var)
        self.__commit()

        c.executemany('INSERT INTO entries VALUES(?, ?{})'.format(',?' * len(existing)), [(now, runnr) + tuple(dqflat.get(value, 'NULL') for value in existing)])
        c.execute('INSERT INTO runs(runnr, timestamp, runendtimestamp) VALUES(?, ?, ?)', (runnr, now, runendtimestamp))
        self.__commit()
    
    def get_runs(self):
        c = self.__get_cursor()
        return list(row[0] for row in c.execute('SELECT runnr FROM runs'))

    def read(self, runnr):
        """Return most recent value with runnr for each distinct var"""
        c = self.__get_cursor()
        result = c.execute('SELECT * FROM entries WHERE runnr = ? ORDER BY timestamp DESC LIMIT 1', (runnr,))
        return dict(zip(self.__cols(), result))

    def get_comment(self, runnr):
        c = self.__get_cursor()
        result = c.execute('SELECT comment FROM runs WHERE runnr = ?', (runnr,))
        return str(result)

    def set_comment(self, runnr, comment):
        c = self.__get_cursor()
        c.execute('UPDATE runs SET comment = ? WHERE runnr = ?', (comment, runnr))
        self.__commit()

    def vars(self):
        cols = self.__cols()
        cols.remove('runnr')
        cols.remove('timestamp')
        return cols

    def __cols(self):
        c = self.__get_cursor()
        table_info = c.execute('PRAGMA table_info(entries)')
        return [str(column[1]) for column in table_info]

    def trend(self, var, runRange):
        self.__sanitize_column_name(var)

        c = self.__get_cursor()
        result = c.execute('SELECT runnr, `{}` FROM entries WHERE runnr >= ? AND runnr <= ? GROUP BY runnr ORDER BY runnr ASC'.format(var), runRange)
        return list(result)

    def trend2d(self, varX, varY, runRange):
        self.__sanitize_column_name(varX)
        self.__sanitize_column_name(varY)

        c = self.__get_cursor()
        result = c.execute('''SELECT X.`{0}`, Y.`{1}` FROM (
                (SELECT runnr, `{0}` FROM entries WHERE runnr >= ? AND runnr <= ? GROUP BY runnr ORDER BY runnr ASC) AS X
                JOIN
                (SELECT runnr, `{1}` FROM entries WHERE runnr >= ? AND runnr <= ? GROUP BY runnr ORDER BY runnr ASC) AS Y
                ON X.runnr = Y.runnr)'''.format(varX, varY),
                2 * runRange)
        return list(result)

    def close(self):
        """Close DB connection"""
        self._connection.close()

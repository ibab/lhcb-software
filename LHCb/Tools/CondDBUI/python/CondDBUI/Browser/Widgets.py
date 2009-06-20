from PyQt4 import QtGui, QtCore
from PyQt4.QtCore import (QObject, SIGNAL, SLOT,
                          QDateTime, QDate, QTime,
                          Qt)
from PyQt4.QtGui import QSizePolicy

import time

from PyCool import cool

__all__ = ["TimePointEdit"]


class TimePointEdit(QtGui.QWidget):
    __pyqtSignals__ = ("timeChanged(QTime)",
                       "dateChanged(QDate)",
                       "dateTimeChanged(QDateTime)",
                       "validityKeyChange(cool::ValidityKey)",
                       "maxChanged(bool)",
                       "maxChecked()"
                       "maxUnchecked()")
    
    def __init__(self, parent = None):
        super(TimePointEdit,self).__init__(parent)
        
        self._layout = QtGui.QHBoxLayout(self)
        self._layout.setObjectName("layout")
        self._layout.setContentsMargins(0,0,0,0)
        
        self._edit = QtGui.QDateTimeEdit(self)
        self._edit.setObjectName("edit")
        self._edit.setTimeSpec(Qt.UTC)
        # Set the time range from cool ValidityKeyMin/Max.
        # Cannot use setTime_t because of the limited range.
        minTimeTuple = time.gmtime(cool.ValidityKeyMin / 1e9)
        minDate = apply(QDate, minTimeTuple[0:3])
        minTime = apply(QTime, minTimeTuple[3:6])
        maxTimeTuple = time.gmtime(cool.ValidityKeyMax / 1e9)
        maxDate = apply(QDate, maxTimeTuple[0:3])
        maxTime = apply(QTime, maxTimeTuple[3:6])
        self._minDateTime = QDateTime(minDate, minTime)
        self._maxDateTime = QDateTime(maxDate, maxTime)
        self._edit.setDateTimeRange(self._minDateTime, self._maxDateTime)
        self._edit.setDisplayFormat("dd-MM-yyyy hh:mm:ss")
        self._edit.setCalendarPopup(True)
        self._edit.setSizePolicy(QtGui.QSizePolicy.MinimumExpanding,
                                 QtGui.QSizePolicy.Minimum)
        self._layout.addWidget(self._edit)
        
        self._utc = QtGui.QCheckBox(self)
        self._utc.setObjectName("utc")
        self._utc.setText("UTC")
        self._utc.setChecked(True)
        self._utc.setSizePolicy(QtGui.QSizePolicy.Minimum,
                                QtGui.QSizePolicy.Minimum)
        self._layout.addWidget(self._utc)
        
        self._max = QtGui.QCheckBox(self)
        self._max.setObjectName("max")
        self._max.setText("Max")
        self._max.setChecked(False)
        self._max.setSizePolicy(QtGui.QSizePolicy.Minimum,
                                QtGui.QSizePolicy.Minimum)
        self._layout.addWidget(self._max)
        
        #self.setFrameShape(self.Box)
        
        QtCore.QMetaObject.connectSlotsByName(self)
        
        # propagate edit field signals:
        for signal in ["timeChanged(QTime)",
                       "dateChanged(QDate)",
                       "dateTimeChanged(QDateTime)"]:
            QObject.connect(self._edit, SIGNAL(signal), self, SIGNAL(signal))
        QObject.connect(self._edit, SIGNAL("dateTimeChanged(QDateTime)"),
                        self.emitValidityKeyChange)
    
    ## Value of the property minDateTime.
    def minimumDateTime(self):
        return self._edit.minimumDateTime()
    
    ## Value of the property maxDateTime.
    def maximumDateTime(self):
        return self._edit.maximumDateTime()
    
    ## Set the property minDateTime.
    def setMinimumDateTime(self, dateTime):
        if dateTime >= self._minDateTime:
            self._edit.setMinimumDateTime(dateTime)
        else:
            self._edit.setMinimumDateTime(self._minDateTime)
    
    ## Set the property maxDateTime.
    def setMaximumDateTime(self, dateTime):
        if dateTime <= self._maxDateTime:
            self._edit.setMaximumDateTime(dateTime)
        else:
            self._edit.setMaximumDateTime(self._maxDateTime)
    
    ## Reset the property minDateTime to its default value.
    def resetMinimumDateTime(self):
        self._edit.setMinimumDateTime(self._minDateTime)
    
    ## Reset the property maxDateTime to its default value.
    def resetMaximumDateTime(self):
        self._edit.setMaximumDateTime(self._maxDateTime)
    
    ## Value of the property maxEnabled.
    def maxEnabled(self):
        return self._max.isEnabled()
    
    ## Set the property maxEnabled.
    def setMaxEnabled(self, enable):
        self._max.setEnabled(enable)
    
    ## Value of the property maxChecked.
    #  Indicates if the "Max" checkbox is checked or not.
    def maxChecked(self):
        return self._max.isChecked()
    
    ## Set the status of the "Max" checkbox..
    def setMaxChecked(self, checked):
        self._max.setChecked(checked)
    
    ## Value of the property dateTime.
    def dateTime(self):
        return self._edit.dateTime()
    
    ## Set the value of the property dateTime.
    def setDateTime(self, dateTime):
        self._edit.setDateTime(dateTime)
    
    ## Slot to update internal state when the "UTC" checkbox is toggled.
    def on_utc_toggled(self, checked):
        if checked:
            spec = Qt.UTC
        else:
            spec = Qt.LocalTime
        self._edit.setTimeSpec(spec)
        # FIXME: This seems to be a bit stupid, but it is the only way I found
        # to trigger a correct re-paint of the displayed date-time.
        self._edit.setDateTime(self._edit.dateTime())
    
    ## Slot to update internal state when the "Max" checkbox is toggled.
    def on_max_toggled(self, checked):
        self._edit.setEnabled(not checked)
        self._utc.setEnabled(not checked)
        self.emit(SIGNAL("maxChanged(bool)"), checked)
        if checked:
            self._edit.setDateTime(self._maxDateTime)
            self.emit(SIGNAL("maxChecked()"))
        else:
            self.emit(SIGNAL("maxUnchecked()"))
            self.emit(SIGNAL("dateTimeChanged(QDateTime)"),self._edit.dateTime())

    ## Convert the internal QDateTime to a cool::ValidityKey.
    def toValidityKey(self):
        # Get the number of seconds since epoch and convert it to ns.
        if self._max.isChecked():
            return cool.ValidityKeyMax
        # FIXME: This is awkward, but I do not have enough resolution otherwise
        d = self._edit.dateTime().toLocalTime()
        timeTuple = (d.date().year(), d.date().month(), d.date().day(),
                     d.time().hour(), d.time().minute(), d.time().second(),
                     0,0,-1)
        return int(time.mktime(timeTuple) * 1e9)

    ## Set the internal QDateTime from a cool::ValidityKey.
    def setValidityKey(self, valKey):
        if valKey == cool.ValidityKeyMax:
            self._edit.setDateTime(self._maxDateTime)
            self._max.setChecked(True)
        else:
            # Cannot use setTime_t because of the limited range.
            timeTuple = time.gmtime(valkey / 1e9)
            d = apply(QDate, timeTuple[0:3])
            t = apply(QTime, timeTuple[3:6])
            self._edit.setDateTime(QDateTime(d, t))
            self._max.setChecked(False)
    
    ## Slot called by a "dateTimeChanged" signal to propagate it as a
    #  cool::ValidityKey.
    def emitValidityKeyChange(self):
        #self.emit(SIGNAL("validityKeyChange(cool::ValidityKey)"),self.toValidityKey())
        self.emit(SIGNAL("validityKeyChange"),self.toValidityKey())

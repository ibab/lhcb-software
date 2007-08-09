<?xml version="1.0" encoding="UTF-8"?>
<xsl:stylesheet version="1.0" xmlns:xsl="http://www.w3.org/1999/XSL/Transform">
  <!-- ==================================================================== -->
  <!-- Main entry point                                                     -->
  <!-- ==================================================================== -->
  <xsl:template match="MBM_SUMMARY">
    <xsl:element name="html">
      <xsl:element name="head"></xsl:element>
        <xsl:element name="body">
          Buffer:<xsl:value-of select="CONTROL/NAME"/>

        <xsl:for-each select="USER">
           Name:<xsl:value-of select="NAME"/>
        </xsl:for-each>
      </xsl:element>
     </xsl:element>
  </xsl:template>
</xsl:stylesheet>

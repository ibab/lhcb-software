<?xml version="1.0" encoding="UTF-8"?>
<xsl:stylesheet version="1.0"
 xmlns:xsl="http://www.w3.org/1999/XSL/Transform"
 xmlns:lhcb="http://cern.ch/lhcbproject/dist/">
  <xsl:output method="html" indent="yes"
   doctype-system="http://www.w3.org/TR/xhtml1/DTD/xhtml1-strict.dtd"
   doctype-public="-//W3C//DTD XHTML 1.0 Strict//EN"/>
  <!-- Variables -->
  <xsl:variable name="lcgcmthome">https://twiki.cern.ch/twiki/bin/view/SPI</xsl:variable>
  <xsl:variable name="gaudihome">http://proj-gaudi.web.cern.ch/proj-gaudi</xsl:variable>
  <xsl:variable name="lhcbreleases">http://lhcb-release-area.web.cern.ch/LHCb-release-area/DOC</xsl:variable>
  
  <!-- Needed to convert from lowercase to uppercase and vice versa -->
  <xsl:variable name="lcletters">abcdefghijklmnopqrstuvwxyz</xsl:variable>
  <xsl:variable name="ucletters">ABCDEFGHIJKLMNOPQRSTUVWXYZ</xsl:variable>
  
  <!-- templates (functions) -->  
  <xsl:template name="hyperlink">
    <xsl:param name="url"/>
    <xsl:param name="text"/>
    <a href="{$url}"><xsl:value-of select="$text"/></a>
  </xsl:template>
  
  <xsl:template name="projecthomeurl">
    <xsl:param name="project"/>
    <xsl:choose>
      <xsl:when test="@name='LCGCMT'">
        <xsl:value-of select="$lcgcmthome"/>
      </xsl:when>
      <xsl:when test="@name='Gaudi'">
        <xsl:value-of select="$gaudihome"/>
      </xsl:when>
      <xsl:otherwise>
        <xsl:value-of select="concat($lhcbreleases,'/',translate(@name,$ucletters,$lcletters))"/>
      </xsl:otherwise>
    </xsl:choose>
  </xsl:template>

  <xsl:template name="projectversionurl">
    <xsl:variable name="url">
      <xsl:call-template name="projecthomeurl">
      </xsl:call-template>
    </xsl:variable>  
    <xsl:choose>
    <xsl:when test="@name='LCGCMT'">
      <xsl:value-of select="concat($url,'/LcgConfiguration',@version)"/>
    </xsl:when>
    <xsl:otherwise>
      <xsl:value-of select="concat($url,'/releases/',@version)"/>
    </xsl:otherwise>
    </xsl:choose>
  </xsl:template>
  
  <xsl:template name="projectlink">
    <td>
      <xsl:call-template name="hyperlink">
        <xsl:with-param name="url">
          <xsl:call-template name="projecthomeurl"/>
        </xsl:with-param>
        <xsl:with-param name="text"><xsl:value-of select="@name"/></xsl:with-param>
      </xsl:call-template>
    </td>
  </xsl:template>
    
  <xsl:template name="projectversionlink">
    <td>
      <xsl:call-template name="hyperlink">
        <xsl:with-param name="url">
          <xsl:call-template name="projectversionurl"/>
        </xsl:with-param>
        <xsl:with-param name="text"><xsl:value-of select="@version"/></xsl:with-param>
      </xsl:call-template>
    </td>
  </xsl:template>

  <xsl:template match="release">
    <tr>
    <th rowspan="2">
    Release &quot;<xsl:value-of select="@name"/>&quot;
    <xsl:if test="@date">
      <br/>date: <xsl:value-of select="@date"/>
    </xsl:if>
    </th>
    <th>Project</th>
    <xsl:for-each select="project">
    <xsl:call-template name="projectlink"/>
    </xsl:for-each>
    </tr>
    <tr>
    <th>Version</th>
    <xsl:for-each select="project">
      <xsl:call-template name="projectversionlink"/>
    </xsl:for-each>
    </tr><tr></tr>
  </xsl:template>

  <!-- main template -->
  <xsl:template match="/">
    <html>
      <head>
        <title>Releases Database</title>
      </head>
      <body>
        <h1>Releases Database</h1>
        <table border="1">
          <xsl:apply-templates select="lhcb:releases_db/release">
            <xsl:sort select="@date" order="descending"/>
          </xsl:apply-templates>
        </table>
      </body>
    </html>
  </xsl:template>
  
</xsl:stylesheet>

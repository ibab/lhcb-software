<?xml version="1.0" encoding="UTF-8"?>
<xsl:stylesheet version="1.0"
 xmlns:xsl="http://www.w3.org/1999/XSL/Transform"
 xmlns:lhcb="http://cern.ch/lhcbproject/dist/">
  <xsl:output method="html" indent="yes"
   doctype-system="http://www.w3.org/TR/xhtml1/DTD/xhtml1-strict.dtd"
   doctype-public="-//W3C//DTD XHTML 1.0 Strict//EN"/>
  <xsl:template match="/">
    <html>
      <head>
        <title>Releases Database</title>
      </head>
      <body>
        <h1>Releases Database</h1>
        <xsl:for-each select="lhcb:releases_db/release">
          <h2>Release &quot;<xsl:value-of select="@name"/>&quot;</h2>
          <table border="1">
          <tr><th>Project</th><th>Version</th></tr>
    	  <xsl:for-each select="project">
            <tr>
              <td>
                <xsl:value-of select="@name"/>
              </td><td>
                <xsl:value-of select="@version"/>
              </td>
            </tr>
          </xsl:for-each>
          </table>
          </xsl:for-each>
      </body>
    </html>
  </xsl:template>
<!--
  <xsl:template match="release">
    <p><xsl:value-of select="@name"/></p>
  </xsl:template>
 -->
</xsl:stylesheet>

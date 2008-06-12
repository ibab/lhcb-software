<?xml version="1.0" encoding="UTF-8" ?>
<xsl:transform version="1.0" xmlns:xsl="http://www.w3.org/1999/XSL/Transform">

  <xsl:include href="datamgmtlib.xsl"/>

  <!-- ==================================================================== -->
  <!--                                                                      -->
  <!-- ==================================================================== -->
	<xsl:template match="WELCOME">
		<xsl:element name="html">
      <xsl:element name="head">
        <xsl:call-template name="xmlns_DataMgmt__style"/>
		  </xsl:element>
 		  <xsl:element name="body">
        <xsl:call-template name="xmlns_DataMgmt__header"/>
        <table>
          <xsl:attribute name="width"><xsl:value-of select="$xmlns_DataMgmt__PgWidth"/></xsl:attribute>
          <TR><TD>
          <xsl:call-template name="output-welcome-text"/>
        </TD></TR></table>
        <xsl:call-template name="xmlns_DataMgmt__trailer"/>
   		</xsl:element>
 		</xsl:element>
	</xsl:template>

  <!-- ==================================================================== -->
  <!--                                                                      -->
  <!-- ==================================================================== -->
	<xsl:template match="DEBUGGER_STARTED">
		<xsl:element name="html">
      <xsl:element name="head">
        <xsl:call-template name="xmlns_DataMgmt__style"/>
		  </xsl:element>
 		  <xsl:element name="body">
        <xsl:call-template name="xmlns_DataMgmt__header"/>
        <table>
          <xsl:attribute name="width"><xsl:value-of select="$xmlns_DataMgmt__PgWidth"/></xsl:attribute>
          <TR>
            <TD>
              <H1>Gaudi Servlet debugging facility:</H1>
              <UL>
                <xsl:for-each select="STATUS_INFO/ROW">
                  <LI><xsl:value-of select="VAL"/></LI>
                </xsl:for-each>
              </UL>
            </TD>
          </TR>
        </table>
        <xsl:call-template name="start_debugger_form"/>
        <xsl:call-template name="xmlns_jcfg__welcome_trailer"/>
        <xsl:call-template name="xmlns_DataMgmt__trailer"/>
   		</xsl:element>
 		</xsl:element>
	</xsl:template>

  <!-- ==================================================================== -->
  <!-- Main entry point                                                     -->
  <!-- ==================================================================== -->
	<xsl:template name="output-welcome-text">
      <h1>Gaudi Servlet Debugging Facility</h1>
      <xsl:call-template name="start_debugger_form"/>
      <xsl:call-template name="xmlns_jcfg__welcome_trailer"/>
	</xsl:template>

  <!-- ==================================================================== -->
  <!-- Main entry point                                                     -->
  <!-- ==================================================================== -->
	<xsl:template name="start_debugger_form">

      <p>
      If you want to start the remote debugger in the servlet server,
      please click the button below!
      </p>

      <form action="Debugger" method="get">
        <input type="hidden" name="-start" value="Debugger"></input>
        <input type="submit" value="Start Debugger"></input>
      </form>
	</xsl:template>

</xsl:transform>

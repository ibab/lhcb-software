<tagfile>
  <compound kind="file">
    <name>DetDescChecks_dll.cpp</name>
    <path>/afs/cern.ch/user/i/ibelyaev/w0/Det/DetDescChecks/v1r0/src/</path>
    <filename>DetDescChecks_dll_cpp.html</filename>
    <member kind="function">
      <name>LOAD_FACTORY_ENTRIES</name>
      <anchor>a0</anchor>
      <arglist>(DetDescChecks)</arglist>
    </member>
  </compound>
  <compound kind="file">
    <name>DetDescChecks_load.cpp</name>
    <path>/afs/cern.ch/user/i/ibelyaev/w0/Det/DetDescChecks/v1r0/src/</path>
    <filename>DetDescChecks_load_cpp.html</filename>
    <member kind="function">
      <name>DECLARE_FACTORY_ENTRIES</name>
      <anchor>a0</anchor>
      <arglist>(DetDescChecks)</arglist>
    </member>
  </compound>
  <compound kind="file">
    <name>DirHbookName.h</name>
    <path>/afs/cern.ch/user/i/ibelyaev/w0/Det/DetDescChecks/v1r0/src/</path>
    <filename>DirHbookName_h.html</filename>
    <namespace>Local</namespace>
    <member kind="define">
      <name>DETDESCCHECKS_DIRHBOOKNAME_H</name>
      <anchor>a0</anchor>
      <arglist></arglist>
    </member>
  </compound>
  <compound kind="file">
    <name>MaterialBudgetAlg.cpp</name>
    <path>/afs/cern.ch/user/i/ibelyaev/w0/Det/DetDescChecks/v1r0/src/</path>
    <filename>MaterialBudgetAlg_cpp.html</filename>
    <member kind="variable">
      <name>MaterialBudgetAlgFactory</name>
      <anchor>a1</anchor>
      <arglist></arglist>
    </member>
  </compound>
  <compound kind="file">
    <name>MaterialBudgetAlg.h</name>
    <path>/afs/cern.ch/user/i/ibelyaev/w0/Det/DetDescChecks/v1r0/src/</path>
    <filename>MaterialBudgetAlg_h.html</filename>
    <class kind="class">MaterialBudgetAlg</class>
    <member kind="define">
      <name>DETDESCCHECKS_MaterialBudgetALG_H</name>
      <anchor>a0</anchor>
      <arglist></arglist>
    </member>
  </compound>
  <compound kind="file">
    <name>VolumeCheckAlg.cpp</name>
    <path>/afs/cern.ch/user/i/ibelyaev/w0/Det/DetDescChecks/v1r0/src/</path>
    <filename>VolumeCheckAlg_cpp.html</filename>
    <member kind="variable">
      <name>VolumeCheckAlgFactory</name>
      <anchor>a1</anchor>
      <arglist></arglist>
    </member>
  </compound>
  <compound kind="file">
    <name>VolumeCheckAlg.h</name>
    <path>/afs/cern.ch/user/i/ibelyaev/w0/Det/DetDescChecks/v1r0/src/</path>
    <filename>VolumeCheckAlg_h.html</filename>
    <class kind="class">VolumeCheckAlg</class>
    <member kind="define">
      <name>DETDESCCHECKS_VOLUMECHECKALG_H</name>
      <anchor>a0</anchor>
      <arglist></arglist>
    </member>
  </compound>
  <compound kind="class">
    <name>Algorithm</name>
    <filename>class_Algorithm.html</filename>
  </compound>
  <compound kind="class">
    <name>MaterialBudgetAlg</name>
    <filename>class_MaterialBudgetAlg.html</filename>
    <base>Algorithm</base>
    <member kind="function" virtualness="virtual">
      <name>initialize</name>
      <anchor>a0</anchor>
      <arglist>()</arglist>
    </member>
    <member kind="function" virtualness="virtual">
      <name>execute</name>
      <anchor>a1</anchor>
      <arglist>()</arglist>
    </member>
    <member kind="function" virtualness="virtual">
      <name>finalize</name>
      <anchor>a2</anchor>
      <arglist>()</arglist>
    </member>
    <member kind="function" protection="public">
      <name>MaterialBudgetAlg</name>
      <anchor>b0</anchor>
      <arglist>(const std::string &amp;name, ISvcLocator *svc)</arglist>
    </member>
    <member kind="function" protection="public" virtualness="virtual">
      <name>~MaterialBudgetAlg</name>
      <anchor>b1</anchor>
      <arglist>()</arglist>
    </member>
  </compound>
  <compound kind="class">
    <name>VolumeCheckAlg</name>
    <filename>class_VolumeCheckAlg.html</filename>
    <base>Algorithm</base>
    <member kind="function" virtualness="virtual">
      <name>initialize</name>
      <anchor>a0</anchor>
      <arglist>()</arglist>
    </member>
    <member kind="function" virtualness="virtual">
      <name>execute</name>
      <anchor>a1</anchor>
      <arglist>()</arglist>
    </member>
    <member kind="function" virtualness="virtual">
      <name>finalize</name>
      <anchor>a2</anchor>
      <arglist>()</arglist>
    </member>
    <member kind="function" protection="public">
      <name>VolumeCheckAlg</name>
      <anchor>b0</anchor>
      <arglist>(const std::string &amp;name, ISvcLocator *svcloc)</arglist>
    </member>
    <member kind="function" protection="public" virtualness="virtual">
      <name>~VolumeCheckAlg</name>
      <anchor>b1</anchor>
      <arglist>()</arglist>
    </member>
  </compound>
  <compound kind="namespace">
    <name>Local</name>
    <filename>namespace_Local.html</filename>
    <member kind="function">
      <name>dirHbookName</name>
      <anchor>a0</anchor>
      <arglist>(const std::string &amp;addr, const int maxLen=8)</arglist>
    </member>
  </compound>
</tagfile>

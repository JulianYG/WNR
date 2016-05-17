<?xml version="1.0" encoding="utf-8"?>
<!DOCTYPE eagle SYSTEM "eagle.dtd">
<eagle version="7.2.0">
<drawing>
<settings>
<setting alwaysvectorfont="no"/>
<setting verticaltext="up"/>
</settings>
<grid distance="0.1" unitdist="inch" unit="inch" style="lines" multiple="1" display="no" altdistance="0.01" altunitdist="inch" altunit="inch"/>
<layers>
<layer number="1" name="Top" color="4" fill="1" visible="no" active="no"/>
<layer number="16" name="Bottom" color="1" fill="1" visible="no" active="no"/>
<layer number="17" name="Pads" color="2" fill="1" visible="no" active="no"/>
<layer number="18" name="Vias" color="2" fill="1" visible="no" active="no"/>
<layer number="19" name="Unrouted" color="6" fill="1" visible="no" active="no"/>
<layer number="20" name="Dimension" color="15" fill="1" visible="no" active="no"/>
<layer number="21" name="tPlace" color="7" fill="1" visible="no" active="no"/>
<layer number="22" name="bPlace" color="7" fill="1" visible="no" active="no"/>
<layer number="23" name="tOrigins" color="15" fill="1" visible="no" active="no"/>
<layer number="24" name="bOrigins" color="15" fill="1" visible="no" active="no"/>
<layer number="25" name="tNames" color="7" fill="1" visible="no" active="no"/>
<layer number="26" name="bNames" color="7" fill="1" visible="no" active="no"/>
<layer number="27" name="tValues" color="7" fill="1" visible="no" active="no"/>
<layer number="28" name="bValues" color="7" fill="1" visible="no" active="no"/>
<layer number="29" name="tStop" color="7" fill="3" visible="no" active="no"/>
<layer number="30" name="bStop" color="7" fill="6" visible="no" active="no"/>
<layer number="31" name="tCream" color="7" fill="4" visible="no" active="no"/>
<layer number="32" name="bCream" color="7" fill="5" visible="no" active="no"/>
<layer number="33" name="tFinish" color="6" fill="3" visible="no" active="no"/>
<layer number="34" name="bFinish" color="6" fill="6" visible="no" active="no"/>
<layer number="35" name="tGlue" color="7" fill="4" visible="no" active="no"/>
<layer number="36" name="bGlue" color="7" fill="5" visible="no" active="no"/>
<layer number="37" name="tTest" color="7" fill="1" visible="no" active="no"/>
<layer number="38" name="bTest" color="7" fill="1" visible="no" active="no"/>
<layer number="39" name="tKeepout" color="4" fill="11" visible="no" active="no"/>
<layer number="40" name="bKeepout" color="1" fill="11" visible="no" active="no"/>
<layer number="41" name="tRestrict" color="4" fill="10" visible="no" active="no"/>
<layer number="42" name="bRestrict" color="1" fill="10" visible="no" active="no"/>
<layer number="43" name="vRestrict" color="2" fill="10" visible="no" active="no"/>
<layer number="44" name="Drills" color="7" fill="1" visible="no" active="no"/>
<layer number="45" name="Holes" color="7" fill="1" visible="no" active="no"/>
<layer number="46" name="Milling" color="3" fill="1" visible="no" active="no"/>
<layer number="47" name="Measures" color="7" fill="1" visible="no" active="no"/>
<layer number="48" name="Document" color="7" fill="1" visible="no" active="no"/>
<layer number="49" name="Reference" color="7" fill="1" visible="no" active="no"/>
<layer number="51" name="tDocu" color="7" fill="1" visible="no" active="no"/>
<layer number="52" name="bDocu" color="7" fill="1" visible="no" active="no"/>
<layer number="90" name="Modules" color="5" fill="1" visible="yes" active="yes"/>
<layer number="91" name="Nets" color="2" fill="1" visible="yes" active="yes"/>
<layer number="92" name="Busses" color="1" fill="1" visible="yes" active="yes"/>
<layer number="93" name="Pins" color="2" fill="1" visible="no" active="yes"/>
<layer number="94" name="Symbols" color="4" fill="1" visible="yes" active="yes"/>
<layer number="95" name="Names" color="7" fill="1" visible="yes" active="yes"/>
<layer number="96" name="Values" color="7" fill="1" visible="yes" active="yes"/>
<layer number="97" name="Info" color="7" fill="1" visible="yes" active="yes"/>
<layer number="98" name="Guide" color="6" fill="1" visible="yes" active="yes"/>
</layers>
<schematic xreflabel="%F%N/%S.%C%R" xrefpart="/%S.%C%R">
<libraries>
<library name="keystone-battery-holder">
<packages>
<package name="2989NOHOLE">
<smd name="NEG" x="0" y="0" dx="3.17" dy="3.17" layer="1"/>
<pad name="POS1" x="-2.765" y="0" drill="1.19"/>
<pad name="P0S2" x="2.765" y="0" drill="1.19"/>
</package>
</packages>
<symbols>
<symbol name="2989">
<wire x1="-2.54" y1="2.54" x2="2.54" y2="2.54" width="0.254" layer="94"/>
<wire x1="2.54" y1="2.54" x2="2.54" y2="-2.54" width="0.254" layer="94"/>
<wire x1="2.54" y1="-2.54" x2="-2.54" y2="-2.54" width="0.254" layer="94"/>
<wire x1="-2.54" y1="-2.54" x2="-2.54" y2="2.54" width="0.254" layer="94"/>
<pin name="NEG" x="0" y="7.62" length="middle" rot="R270"/>
<pin name="POS2" x="7.62" y="0" length="middle" rot="R180"/>
<pin name="POS1" x="-7.62" y="0" length="middle"/>
</symbol>
</symbols>
<devicesets>
<deviceset name="2989NOHOLE">
<gates>
<gate name="G$1" symbol="2989" x="0" y="0"/>
</gates>
<devices>
<device name="" package="2989NOHOLE">
<connects>
<connect gate="G$1" pin="NEG" pad="NEG"/>
<connect gate="G$1" pin="POS1" pad="POS1"/>
<connect gate="G$1" pin="POS2" pad="P0S2"/>
</connects>
<technologies>
<technology name=""/>
</technologies>
</device>
</devices>
</deviceset>
</devicesets>
</library>
<library name="samtec_fts">
<packages>
<package name="SAMTEC_FTS-101-03-S">
<pad name="1" x="0" y="0" drill="0.7112" shape="square"/>
<wire x1="-0.715" y1="1.08" x2="-0.715" y2="-1.08" width="0.127" layer="21"/>
<wire x1="-0.715" y1="-1.08" x2="0.715" y2="-1.08" width="0.127" layer="21"/>
<wire x1="0.715" y1="-1.08" x2="0.715" y2="1.08" width="0.127" layer="21"/>
<wire x1="0.715" y1="1.08" x2="-0.715" y2="1.08" width="0.127" layer="21"/>
</package>
</packages>
<symbols>
<symbol name="FTS-101-03-L-S">
<pin name="1" x="-7.62" y="0" length="middle" direction="pas"/>
<wire x1="-2.54" y1="2.54" x2="-2.54" y2="-2.54" width="0.254" layer="94"/>
<text x="-7.366" y="2.5908" size="2.0828" layer="95" ratio="10" rot="SR0">&gt;NAME</text>
<text x="-8.3312" y="-4.3942" size="2.0828" layer="96" ratio="10" rot="SR0">&gt;VALUE</text>
<wire x1="0" y1="2.54" x2="-2.54" y2="2.54" width="0.254" layer="94"/>
<wire x1="0" y1="2.54" x2="0" y2="-2.54" width="0.254" layer="94"/>
<wire x1="0" y1="-2.54" x2="-2.54" y2="-2.54" width="0.254" layer="94"/>
</symbol>
</symbols>
<devicesets>
<deviceset name="FTS-101-03-L-S">
<gates>
<gate name="G$1" symbol="FTS-101-03-L-S" x="2.54" y="0"/>
</gates>
<devices>
<device name="" package="SAMTEC_FTS-101-03-S">
<connects>
<connect gate="G$1" pin="1" pad="1"/>
</connects>
<technologies>
<technology name=""/>
</technologies>
</device>
</devices>
</deviceset>
</devicesets>
</library>
</libraries>
<attributes>
</attributes>
<variantdefs>
</variantdefs>
<classes>
<class number="0" name="default" width="0" drill="0">
</class>
</classes>
<parts>
<part name="U$1" library="keystone-battery-holder" deviceset="2989NOHOLE" device=""/>
<part name="U$2" library="keystone-battery-holder" deviceset="2989NOHOLE" device=""/>
<part name="GND" library="samtec_fts" deviceset="FTS-101-03-L-S" device=""/>
<part name="VDD" library="samtec_fts" deviceset="FTS-101-03-L-S" device=""/>
</parts>
<sheets>
<sheet>
<plain>
</plain>
<instances>
<instance part="U$1" gate="G$1" x="15.24" y="63.5"/>
<instance part="U$2" gate="G$1" x="48.26" y="63.5"/>
<instance part="GND" gate="G$1" x="55.88" y="78.74"/>
<instance part="VDD" gate="G$1" x="-2.54" y="78.74" rot="R180"/>
</instances>
<busses>
</busses>
<nets>
<net name="N$1" class="0">
<segment>
<pinref part="U$1" gate="G$1" pin="NEG"/>
<pinref part="U$2" gate="G$1" pin="POS1"/>
<wire x1="15.24" y1="71.12" x2="27.94" y2="71.12" width="0.1524" layer="91"/>
<wire x1="27.94" y1="71.12" x2="40.64" y2="71.12" width="0.1524" layer="91"/>
<wire x1="40.64" y1="71.12" x2="40.64" y2="63.5" width="0.1524" layer="91"/>
<pinref part="U$2" gate="G$1" pin="POS2"/>
<wire x1="55.88" y1="63.5" x2="55.88" y2="55.88" width="0.1524" layer="91"/>
<wire x1="55.88" y1="55.88" x2="27.94" y2="55.88" width="0.1524" layer="91"/>
<wire x1="27.94" y1="55.88" x2="27.94" y2="71.12" width="0.1524" layer="91"/>
</segment>
</net>
<net name="N$3" class="0">
<segment>
<pinref part="U$1" gate="G$1" pin="POS1"/>
<wire x1="7.62" y1="63.5" x2="5.08" y2="63.5" width="0.1524" layer="91"/>
<wire x1="5.08" y1="63.5" x2="5.08" y2="78.74" width="0.1524" layer="91"/>
<pinref part="VDD" gate="G$1" pin="1"/>
<pinref part="U$1" gate="G$1" pin="POS2"/>
<wire x1="22.86" y1="63.5" x2="22.86" y2="53.34" width="0.1524" layer="91"/>
<wire x1="22.86" y1="53.34" x2="5.08" y2="53.34" width="0.1524" layer="91"/>
<wire x1="5.08" y1="53.34" x2="5.08" y2="63.5" width="0.1524" layer="91"/>
</segment>
</net>
<net name="N$2" class="0">
<segment>
<pinref part="U$2" gate="G$1" pin="NEG"/>
<pinref part="GND" gate="G$1" pin="1"/>
<wire x1="48.26" y1="71.12" x2="48.26" y2="78.74" width="0.1524" layer="91"/>
</segment>
</net>
</nets>
</sheet>
</sheets>
</schematic>
</drawing>
</eagle>

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
<layer number="50" name="dxf" color="7" fill="1" visible="no" active="no"/>
<layer number="51" name="tDocu" color="6" fill="1" visible="no" active="no"/>
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
<layer number="100" name="help" color="7" fill="1" visible="yes" active="yes"/>
<layer number="101" name="DOC" color="7" fill="1" visible="yes" active="yes"/>
<layer number="102" name="bot_pads" color="7" fill="1" visible="yes" active="yes"/>
<layer number="104" name="S_DOKU" color="7" fill="1" visible="yes" active="yes"/>
<layer number="116" name="Patch_BOT" color="7" fill="1" visible="yes" active="yes"/>
<layer number="199" name="Contour" color="7" fill="1" visible="yes" active="yes"/>
<layer number="200" name="200bmp" color="1" fill="10" visible="yes" active="yes"/>
<layer number="201" name="201bmp" color="2" fill="10" visible="yes" active="yes"/>
<layer number="250" name="Descript" color="3" fill="1" visible="no" active="no"/>
<layer number="251" name="SMDround" color="12" fill="11" visible="no" active="no"/>
<layer number="254" name="OrgLBR" color="7" fill="1" visible="yes" active="yes"/>
</layers>
<schematic xreflabel="%F%N/%S.%C%R" xrefpart="/%S.%C%R">
<libraries>
<library name="RHD2000">
<packages>
<package name="OMNETICS_PZN-12-AA">
<wire x1="-2.2225" y1="3.9878" x2="2.2225" y2="3.9878" width="0.127" layer="21"/>
<wire x1="2.2225" y1="3.9878" x2="2.2225" y2="2.2098" width="0.127" layer="21"/>
<wire x1="2.2225" y1="2.2098" x2="2.2225" y2="-0.3302" width="0.127" layer="21"/>
<wire x1="-2.2225" y1="-0.3302" x2="-2.2225" y2="2.2098" width="0.127" layer="21"/>
<wire x1="-2.2225" y1="2.2098" x2="-2.2225" y2="3.9878" width="0.127" layer="21"/>
<wire x1="-1.5875" y1="-0.3302" x2="-1.5875" y2="-0.4445" width="0.127" layer="21"/>
<wire x1="-0.9525" y1="-0.3302" x2="-0.9525" y2="-0.4445" width="0.127" layer="21"/>
<wire x1="1.5875" y1="-0.3302" x2="1.5875" y2="-0.4445" width="0.127" layer="21"/>
<smd name="B4" x="-0.3175" y="-1.016" dx="0.381" dy="0.762" layer="1" rot="R180"/>
<smd name="B3" x="0.3175" y="-1.016" dx="0.381" dy="0.762" layer="1" rot="R180"/>
<smd name="B5" x="-0.9525" y="-1.016" dx="0.381" dy="0.762" layer="1" rot="R180"/>
<smd name="B2" x="0.9525" y="-1.016" dx="0.381" dy="0.762" layer="1" rot="R180"/>
<smd name="B6" x="-1.5875" y="-1.016" dx="0.381" dy="0.762" layer="1" rot="R180"/>
<smd name="B1" x="1.5875" y="-1.016" dx="0.381" dy="0.762" layer="1" rot="R180"/>
<smd name="T6" x="-1.5875" y="-2.159" dx="0.381" dy="1.016" layer="1" rot="R180"/>
<smd name="T5" x="-0.9525" y="-2.159" dx="0.381" dy="1.016" layer="1" rot="R180"/>
<smd name="T4" x="-0.3175" y="-2.159" dx="0.381" dy="1.016" layer="1" rot="R180"/>
<smd name="T3" x="0.3175" y="-2.159" dx="0.381" dy="1.016" layer="1" rot="R180"/>
<smd name="T2" x="0.9525" y="-2.159" dx="0.381" dy="1.016" layer="1" rot="R180"/>
<smd name="T1" x="1.5875" y="-2.159" dx="0.381" dy="1.016" layer="1" rot="R180"/>
<text x="-2.54" y="-2.3495" size="1.27" layer="25" font="vector" rot="R90">&gt;NAME</text>
<wire x1="-2.2225" y1="-0.3302" x2="-1.5875" y2="-0.3302" width="0.127" layer="21"/>
<wire x1="-1.5875" y1="-0.3302" x2="-0.9525" y2="-0.3302" width="0.127" layer="21"/>
<wire x1="-0.9525" y1="-0.3302" x2="-0.3175" y2="-0.3302" width="0.127" layer="21"/>
<wire x1="-0.3175" y1="-0.3302" x2="0.3175" y2="-0.3302" width="0.127" layer="21"/>
<wire x1="0.3175" y1="-0.3302" x2="0.9525" y2="-0.3302" width="0.127" layer="21"/>
<wire x1="0.9525" y1="-0.3302" x2="1.5875" y2="-0.3302" width="0.127" layer="21"/>
<wire x1="1.5875" y1="-0.3302" x2="2.2225" y2="-0.3302" width="0.127" layer="21"/>
<wire x1="-0.3175" y1="-0.3302" x2="-0.3175" y2="-0.4445" width="0.127" layer="21"/>
<wire x1="0.3175" y1="-0.3302" x2="0.3175" y2="-0.4445" width="0.127" layer="21"/>
<wire x1="0.9525" y1="-0.3302" x2="0.9525" y2="-0.4445" width="0.127" layer="21"/>
<wire x1="-2.2225" y1="2.2098" x2="2.2225" y2="2.2098" width="0.127" layer="21"/>
</package>
</packages>
<symbols>
<symbol name="RHD2000-CABLE">
<wire x1="25.4" y1="5.08" x2="-25.4" y2="5.08" width="0.254" layer="94"/>
<wire x1="-25.4" y1="5.08" x2="-25.4" y2="-7.62" width="0.254" layer="94"/>
<wire x1="-25.4" y1="-7.62" x2="25.4" y2="-7.62" width="0.254" layer="94"/>
<wire x1="25.4" y1="-7.62" x2="25.4" y2="5.08" width="0.254" layer="94"/>
<text x="25.4" y="-8.89" size="1.778" layer="95" font="vector" rot="R180">&gt;NAME</text>
<text x="-15.24" y="-8.89" size="1.778" layer="95" font="vector" rot="R180">&gt;VALUE</text>
<pin name="MISO2-" x="10.16" y="7.62" length="short" rot="R270"/>
<pin name="MISO1+" x="5.08" y="7.62" length="short" rot="R270"/>
<pin name="MISO2+" x="12.7" y="7.62" length="short" rot="R270"/>
<pin name="VDD" x="20.32" y="7.62" length="short" rot="R270"/>
<pin name="GND" x="17.78" y="7.62" length="short" direction="pwr" rot="R270"/>
<pin name="MOSI-" x="-5.08" y="7.62" length="short" rot="R270"/>
<pin name="SCLK+" x="-10.16" y="7.62" length="short" rot="R270"/>
<pin name="MISO1-" x="2.54" y="7.62" length="short" rot="R270"/>
<pin name="MOSI+" x="-2.54" y="7.62" length="short" rot="R270"/>
<pin name="!CS!-" x="-20.32" y="7.62" length="short" rot="R270"/>
<pin name="!CS!+" x="-17.78" y="7.62" length="short" rot="R270"/>
<pin name="SCLK-" x="-12.7" y="7.62" length="short" rot="R270"/>
</symbol>
</symbols>
<devicesets>
<deviceset name="RHD2000-CABLE-DAQ-SIDE" prefix="S">
<description>Omnetics A79623 PZN-12-AA connector for SPI interface (controller side)</description>
<gates>
<gate name="A" symbol="RHD2000-CABLE" x="0" y="2.54"/>
</gates>
<devices>
<device name="" package="OMNETICS_PZN-12-AA">
<connects>
<connect gate="A" pin="!CS!+" pad="B1"/>
<connect gate="A" pin="!CS!-" pad="T1"/>
<connect gate="A" pin="GND" pad="T6"/>
<connect gate="A" pin="MISO1+" pad="B4"/>
<connect gate="A" pin="MISO1-" pad="T4"/>
<connect gate="A" pin="MISO2+" pad="B5"/>
<connect gate="A" pin="MISO2-" pad="T5"/>
<connect gate="A" pin="MOSI+" pad="B3"/>
<connect gate="A" pin="MOSI-" pad="T3"/>
<connect gate="A" pin="SCLK+" pad="B2"/>
<connect gate="A" pin="SCLK-" pad="T2"/>
<connect gate="A" pin="VDD" pad="B6"/>
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
<package name="SAMTEC_FTS-108-03-S">
<pad name="1" x="0" y="0" drill="0.7112" shape="square"/>
<pad name="2" x="1.27" y="0" drill="0.7112"/>
<pad name="3" x="2.54" y="0" drill="0.7112"/>
<pad name="4" x="3.81" y="0" drill="0.7112"/>
<pad name="5" x="5.08" y="0" drill="0.7112"/>
<pad name="6" x="6.35" y="0" drill="0.7112"/>
<pad name="7" x="7.62" y="0" drill="0.7112"/>
<pad name="8" x="8.89" y="0" drill="0.7112"/>
<wire x1="-0.71" y1="1.08" x2="-0.71" y2="-1.08" width="0.127" layer="21"/>
<wire x1="-0.71" y1="-1.08" x2="9.6" y2="-1.08" width="0.127" layer="21"/>
<wire x1="9.6" y1="-1.08" x2="9.6" y2="1.08" width="0.127" layer="21"/>
<wire x1="9.6" y1="1.08" x2="-0.71" y2="1.08" width="0.127" layer="21"/>
</package>
</packages>
<symbols>
<symbol name="FTS-108-03-L-S">
<pin name="1" x="-5.08" y="7.62" length="middle" direction="pas"/>
<pin name="2" x="-5.08" y="5.08" length="middle" direction="pas"/>
<pin name="3" x="-5.08" y="2.54" length="middle" direction="pas"/>
<pin name="4" x="-5.08" y="0" length="middle" direction="pas"/>
<pin name="5" x="-5.08" y="-2.54" length="middle" direction="pas"/>
<pin name="6" x="-5.08" y="-5.08" length="middle" direction="pas"/>
<pin name="7" x="-5.08" y="-7.62" length="middle" direction="pas"/>
<pin name="8" x="-5.08" y="-10.16" length="middle" direction="pas"/>
<wire x1="0" y1="10.16" x2="0" y2="-12.7" width="0.254" layer="94"/>
<text x="-4.826" y="10.2108" size="2.0828" layer="95" ratio="10" rot="SR0">&gt;NAME</text>
<text x="-5.7912" y="-14.5542" size="2.0828" layer="96" ratio="10" rot="SR0">&gt;VALUE</text>
<wire x1="2.54" y1="10.16" x2="0" y2="10.16" width="0.254" layer="94"/>
<wire x1="2.54" y1="10.16" x2="2.54" y2="-12.7" width="0.254" layer="94"/>
<wire x1="2.54" y1="-12.7" x2="0" y2="-12.7" width="0.254" layer="94"/>
</symbol>
</symbols>
<devicesets>
<deviceset name="FTS-108-03-L-S" prefix="J">
<description>CONN HEADER, 1.27MM, VERTICAL THT, 16WAY</description>
<gates>
<gate name="A" symbol="FTS-108-03-L-S" x="0" y="0"/>
</gates>
<devices>
<device name="" package="SAMTEC_FTS-108-03-S">
<connects>
<connect gate="A" pin="1" pad="1"/>
<connect gate="A" pin="2" pad="2"/>
<connect gate="A" pin="3" pad="3"/>
<connect gate="A" pin="4" pad="4"/>
<connect gate="A" pin="5" pad="5"/>
<connect gate="A" pin="6" pad="6"/>
<connect gate="A" pin="7" pad="7"/>
<connect gate="A" pin="8" pad="8"/>
</connects>
<technologies>
<technology name="">
<attribute name="MPN" value="FTS-108-03-L-D" constant="no"/>
<attribute name="OC_FARNELL" value="1929684" constant="no"/>
<attribute name="OC_NEWARK" value="11P4665" constant="no"/>
<attribute name="PACKAGE" value="HEADER, 1.27MM, VERTICAL THT, 16WAY" constant="no"/>
<attribute name="SUPPLIER" value="Samtec" constant="no"/>
</technology>
</technologies>
</device>
</devices>
</deviceset>
</devicesets>
</library>
<library name="LPC81XM">
<packages>
<package name="TSSOP20">
<wire x1="-3.3" y1="2.2" x2="3.3" y2="2.2" width="0.127" layer="21"/>
<wire x1="3.3" y1="2.2" x2="3.3" y2="-2.2" width="0.127" layer="21"/>
<wire x1="3.3" y1="-2.2" x2="-3.3" y2="-2.2" width="0.127" layer="21"/>
<wire x1="-3.3" y1="-2.2" x2="-3.3" y2="2.2" width="0.127" layer="25"/>
<rectangle x1="-3.1" y1="2.2" x2="-2.8" y2="3.2" layer="51"/>
<rectangle x1="-2.4" y1="2.2" x2="-2.1" y2="3.2" layer="51"/>
<rectangle x1="-1.8" y1="2.2" x2="-1.5" y2="3.2" layer="51"/>
<rectangle x1="-1.1" y1="2.2" x2="-0.8" y2="3.2" layer="51"/>
<rectangle x1="-0.5" y1="2.2" x2="-0.2" y2="3.2" layer="51"/>
<rectangle x1="0.2" y1="2.2" x2="0.5" y2="3.2" layer="51"/>
<rectangle x1="0.8" y1="2.2" x2="1.1" y2="3.2" layer="51"/>
<rectangle x1="1.5" y1="2.2" x2="1.8" y2="3.2" layer="51"/>
<rectangle x1="2.1" y1="2.2" x2="2.4" y2="3.2" layer="51"/>
<rectangle x1="2.8" y1="2.2" x2="3.1" y2="3.2" layer="51"/>
<rectangle x1="2.8" y1="-3.2" x2="3.1" y2="-2.2" layer="51"/>
<rectangle x1="2.1" y1="-3.2" x2="2.4" y2="-2.2" layer="51"/>
<rectangle x1="1.5" y1="-3.2" x2="1.8" y2="-2.2" layer="51"/>
<rectangle x1="0.8" y1="-3.2" x2="1.1" y2="-2.2" layer="51"/>
<rectangle x1="0.2" y1="-3.2" x2="0.5" y2="-2.2" layer="51"/>
<rectangle x1="-0.5" y1="-3.2" x2="-0.2" y2="-2.2" layer="51"/>
<rectangle x1="-1.1" y1="-3.2" x2="-0.8" y2="-2.2" layer="51"/>
<rectangle x1="-1.8" y1="-3.2" x2="-1.5" y2="-2.2" layer="51"/>
<rectangle x1="-2.4" y1="-3.2" x2="-2.1" y2="-2.2" layer="51"/>
<rectangle x1="-3.1" y1="-3.2" x2="-2.8" y2="-2.2" layer="51"/>
<text x="-3.5" y="-2.6" size="0.4" layer="51" rot="R90">LPC81XM_TSSOP20</text>
<circle x="-2.8" y="-1.7" radius="0.22360625" width="0" layer="21"/>
<smd name="P$1" x="-3.025" y="-2.925" dx="0.6" dy="1.35" layer="1"/>
<smd name="P$2" x="-2.275" y="-2.925" dx="0.4" dy="1.35" layer="1"/>
<smd name="P$3" x="-1.625" y="-2.925" dx="0.4" dy="1.35" layer="1"/>
<smd name="P$4" x="-0.975" y="-2.925" dx="0.4" dy="1.35" layer="1"/>
<smd name="P$5" x="-0.325" y="-2.925" dx="0.4" dy="1.35" layer="1"/>
<smd name="P$6" x="0.325" y="-2.925" dx="0.4" dy="1.35" layer="1"/>
<smd name="P$7" x="0.975" y="-2.925" dx="0.4" dy="1.35" layer="1"/>
<smd name="P$8" x="1.625" y="-2.925" dx="0.4" dy="1.35" layer="1"/>
<smd name="P$9" x="2.275" y="-2.925" dx="0.4" dy="1.35" layer="1"/>
<smd name="P$10" x="3.025" y="-2.925" dx="0.6" dy="1.35" layer="1"/>
<smd name="P$11" x="3.025" y="2.925" dx="0.6" dy="1.35" layer="1"/>
<smd name="P$12" x="2.275" y="2.925" dx="0.4" dy="1.35" layer="1"/>
<smd name="P$13" x="1.625" y="2.925" dx="0.4" dy="1.35" layer="1"/>
<smd name="P$14" x="0.975" y="2.925" dx="0.4" dy="1.35" layer="1"/>
<smd name="P$15" x="0.325" y="2.925" dx="0.4" dy="1.35" layer="1"/>
<smd name="P$16" x="-0.325" y="2.925" dx="0.4" dy="1.35" layer="1"/>
<smd name="P$17" x="-0.975" y="2.925" dx="0.4" dy="1.35" layer="1"/>
<smd name="P$18" x="-1.625" y="2.925" dx="0.4" dy="1.35" layer="1"/>
<smd name="P$19" x="-2.275" y="2.925" dx="0.4" dy="1.35" layer="1"/>
<smd name="P$20" x="-3.025" y="2.925" dx="0.6" dy="1.35" layer="1"/>
</package>
</packages>
<symbols>
<symbol name="LPC81XM">
<wire x1="-12.7" y1="15.24" x2="-12.7" y2="-12.7" width="0.254" layer="94"/>
<wire x1="-12.7" y1="-12.7" x2="12.7" y2="-12.7" width="0.254" layer="94"/>
<wire x1="12.7" y1="-12.7" x2="12.7" y2="15.24" width="0.254" layer="94"/>
<wire x1="12.7" y1="15.24" x2="-12.7" y2="15.24" width="0.254" layer="94"/>
<pin name="PIO0_17" x="-17.78" y="12.7" length="middle"/>
<pin name="PIO0_13" x="-17.78" y="10.16" length="middle"/>
<pin name="PIO0_12" x="-17.78" y="7.62" length="middle"/>
<pin name="PIO0_5" x="-17.78" y="5.08" length="middle"/>
<pin name="PIO0_4" x="-17.78" y="2.54" length="middle"/>
<pin name="PIO0_3" x="-17.78" y="0" length="middle"/>
<pin name="PIO0_2" x="-17.78" y="-2.54" length="middle"/>
<pin name="PIO0_11" x="-17.78" y="-5.08" length="middle"/>
<pin name="PIO0_10" x="-17.78" y="-7.62" length="middle"/>
<pin name="PIO0_16" x="-17.78" y="-10.16" length="middle"/>
<pin name="PIO0_15" x="17.78" y="-10.16" length="middle" rot="R180"/>
<pin name="PIO0_1" x="17.78" y="-7.62" length="middle" rot="R180"/>
<pin name="PIO0_9" x="17.78" y="-5.08" length="middle" rot="R180"/>
<pin name="PIO0_8" x="17.78" y="-2.54" length="middle" rot="R180"/>
<pin name="VDD" x="17.78" y="0" length="middle" rot="R180"/>
<pin name="VSS" x="17.78" y="2.54" length="middle" rot="R180"/>
<pin name="PIO0_7" x="17.78" y="5.08" length="middle" rot="R180"/>
<pin name="PIO0_6" x="17.78" y="7.62" length="middle" rot="R180"/>
<pin name="PIO0_0" x="17.78" y="10.16" length="middle" rot="R180"/>
<pin name="PIO0_14" x="17.78" y="12.7" length="middle" rot="R180"/>
<text x="-15.24" y="15.24" size="2.54" layer="95">LPC81XM-TSSOP20</text>
</symbol>
</symbols>
<devicesets>
<deviceset name="LPC81XM_TSSOP20" prefix="IC">
<gates>
<gate name="G$1" symbol="LPC81XM" x="0" y="0"/>
</gates>
<devices>
<device name="" package="TSSOP20">
<connects>
<connect gate="G$1" pin="PIO0_0" pad="P$19"/>
<connect gate="G$1" pin="PIO0_1" pad="P$12"/>
<connect gate="G$1" pin="PIO0_10" pad="P$9"/>
<connect gate="G$1" pin="PIO0_11" pad="P$8"/>
<connect gate="G$1" pin="PIO0_12" pad="P$3"/>
<connect gate="G$1" pin="PIO0_13" pad="P$2"/>
<connect gate="G$1" pin="PIO0_14" pad="P$20"/>
<connect gate="G$1" pin="PIO0_15" pad="P$11"/>
<connect gate="G$1" pin="PIO0_16" pad="P$10"/>
<connect gate="G$1" pin="PIO0_17" pad="P$1"/>
<connect gate="G$1" pin="PIO0_2" pad="P$7"/>
<connect gate="G$1" pin="PIO0_3" pad="P$6"/>
<connect gate="G$1" pin="PIO0_4" pad="P$5"/>
<connect gate="G$1" pin="PIO0_5" pad="P$4"/>
<connect gate="G$1" pin="PIO0_6" pad="P$18"/>
<connect gate="G$1" pin="PIO0_7" pad="P$17"/>
<connect gate="G$1" pin="PIO0_8" pad="P$14"/>
<connect gate="G$1" pin="PIO0_9" pad="P$13"/>
<connect gate="G$1" pin="VDD" pad="P$15"/>
<connect gate="G$1" pin="VSS" pad="P$16"/>
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
<part name="S1" library="RHD2000" deviceset="RHD2000-CABLE-DAQ-SIDE" device=""/>
<part name="J1" library="samtec_fts" deviceset="FTS-108-03-L-S" device=""/>
<part name="LVDS" library="LPC81XM" deviceset="LPC81XM_TSSOP20" device=""/>
</parts>
<sheets>
<sheet>
<plain>
</plain>
<instances>
<instance part="S1" gate="A" x="83.82" y="50.8" rot="R90"/>
<instance part="J1" gate="A" x="-35.56" y="53.34" rot="R180"/>
<instance part="LVDS" gate="G$1" x="15.24" y="50.8"/>
</instances>
<busses>
</busses>
<nets>
<net name="GND" class="0">
<segment>
<wire x1="-20.32" y1="63.5" x2="-20.32" y2="73.66" width="0.1524" layer="91"/>
<wire x1="-20.32" y1="73.66" x2="-17.78" y2="73.66" width="0.1524" layer="91"/>
<wire x1="-17.78" y1="73.66" x2="53.34" y2="73.66" width="0.1524" layer="91"/>
<wire x1="53.34" y1="73.66" x2="53.34" y2="68.58" width="0.1524" layer="91"/>
<pinref part="S1" gate="A" pin="GND"/>
<wire x1="53.34" y1="68.58" x2="76.2" y2="68.58" width="0.1524" layer="91"/>
<pinref part="LVDS" gate="G$1" pin="PIO0_13"/>
<wire x1="-2.54" y1="60.96" x2="-17.78" y2="60.96" width="0.1524" layer="91"/>
<wire x1="-17.78" y1="60.96" x2="-17.78" y2="73.66" width="0.1524" layer="91"/>
<junction x="-17.78" y="73.66"/>
<pinref part="J1" gate="A" pin="8"/>
<wire x1="-30.48" y1="63.5" x2="-20.32" y2="63.5" width="0.1524" layer="91"/>
<label x="-12.7" y="60.96" size="1.778" layer="95"/>
<label x="-30.48" y="63.5" size="1.778" layer="95"/>
</segment>
</net>
<net name="VDD" class="0">
<segment>
<pinref part="S1" gate="A" pin="VDD"/>
<wire x1="76.2" y1="71.12" x2="55.88" y2="71.12" width="0.1524" layer="91"/>
<wire x1="55.88" y1="71.12" x2="55.88" y2="78.74" width="0.1524" layer="91"/>
<wire x1="55.88" y1="78.74" x2="-22.86" y2="78.74" width="0.1524" layer="91"/>
<wire x1="-22.86" y1="78.74" x2="-22.86" y2="60.96" width="0.1524" layer="91"/>
<pinref part="J1" gate="A" pin="7"/>
<wire x1="-22.86" y1="60.96" x2="-30.48" y2="60.96" width="0.1524" layer="91"/>
<label x="-30.48" y="60.96" size="1.778" layer="95"/>
</segment>
<segment>
<pinref part="LVDS" gate="G$1" pin="PIO0_5"/>
<wire x1="-2.54" y1="55.88" x2="-7.62" y2="55.88" width="0.1524" layer="91"/>
<label x="-10.16" y="55.88" size="1.778" layer="95"/>
</segment>
</net>
<net name="CS" class="0">
<segment>
<pinref part="LVDS" gate="G$1" pin="PIO0_17"/>
<wire x1="-2.54" y1="63.5" x2="-15.24" y2="63.5" width="0.1524" layer="91"/>
<label x="-12.7" y="63.5" size="1.778" layer="95"/>
<pinref part="J1" gate="A" pin="6"/>
<wire x1="-30.48" y1="58.42" x2="-15.24" y2="58.42" width="0.1524" layer="91"/>
<wire x1="-15.24" y1="58.42" x2="-15.24" y2="63.5" width="0.1524" layer="91"/>
<label x="-30.48" y="58.42" size="1.778" layer="95"/>
</segment>
</net>
<net name="MISO" class="0">
<segment>
<pinref part="LVDS" gate="G$1" pin="PIO0_10"/>
<wire x1="-2.54" y1="43.18" x2="-12.7" y2="43.18" width="0.1524" layer="91"/>
<label x="-12.7" y="43.18" size="1.778" layer="95"/>
<wire x1="-12.7" y1="43.18" x2="-12.7" y2="50.8" width="0.1524" layer="91"/>
<pinref part="J1" gate="A" pin="3"/>
<wire x1="-12.7" y1="50.8" x2="-30.48" y2="50.8" width="0.1524" layer="91"/>
<label x="-30.48" y="50.8" size="1.778" layer="95"/>
</segment>
</net>
<net name="SCLK" class="0">
<segment>
<pinref part="LVDS" gate="G$1" pin="PIO0_12"/>
<wire x1="-2.54" y1="58.42" x2="-12.7" y2="58.42" width="0.1524" layer="91"/>
<label x="-12.7" y="58.42" size="1.778" layer="95"/>
<wire x1="-12.7" y1="58.42" x2="-12.7" y2="55.88" width="0.1524" layer="91"/>
<pinref part="J1" gate="A" pin="5"/>
<wire x1="-12.7" y1="55.88" x2="-30.48" y2="55.88" width="0.1524" layer="91"/>
<label x="-30.48" y="55.88" size="1.778" layer="95"/>
</segment>
</net>
<net name="CS+" class="0">
<segment>
<pinref part="LVDS" gate="G$1" pin="PIO0_14"/>
<wire x1="33.02" y1="63.5" x2="53.34" y2="63.5" width="0.1524" layer="91"/>
<wire x1="53.34" y1="63.5" x2="53.34" y2="33.02" width="0.1524" layer="91"/>
<pinref part="S1" gate="A" pin="!CS!+"/>
<wire x1="53.34" y1="33.02" x2="76.2" y2="33.02" width="0.1524" layer="91"/>
<label x="35.56" y="63.5" size="1.778" layer="95"/>
</segment>
</net>
<net name="CS-" class="0">
<segment>
<pinref part="LVDS" gate="G$1" pin="PIO0_0"/>
<wire x1="33.02" y1="60.96" x2="50.8" y2="60.96" width="0.1524" layer="91"/>
<wire x1="50.8" y1="60.96" x2="50.8" y2="30.48" width="0.1524" layer="91"/>
<pinref part="S1" gate="A" pin="!CS!-"/>
<wire x1="50.8" y1="30.48" x2="76.2" y2="30.48" width="0.1524" layer="91"/>
<label x="35.56" y="60.96" size="1.778" layer="95"/>
</segment>
</net>
<net name="SCLK+" class="0">
<segment>
<pinref part="LVDS" gate="G$1" pin="PIO0_6"/>
<wire x1="33.02" y1="58.42" x2="48.26" y2="58.42" width="0.1524" layer="91"/>
<wire x1="48.26" y1="58.42" x2="48.26" y2="40.64" width="0.1524" layer="91"/>
<pinref part="S1" gate="A" pin="SCLK+"/>
<wire x1="48.26" y1="40.64" x2="76.2" y2="40.64" width="0.1524" layer="91"/>
<label x="35.56" y="58.42" size="1.778" layer="95"/>
</segment>
</net>
<net name="SCLK-" class="0">
<segment>
<pinref part="S1" gate="A" pin="SCLK-"/>
<wire x1="76.2" y1="38.1" x2="45.72" y2="38.1" width="0.1524" layer="91"/>
<wire x1="45.72" y1="38.1" x2="45.72" y2="55.88" width="0.1524" layer="91"/>
<pinref part="LVDS" gate="G$1" pin="PIO0_7"/>
<wire x1="45.72" y1="55.88" x2="33.02" y2="55.88" width="0.1524" layer="91"/>
<label x="35.56" y="55.88" size="1.778" layer="95"/>
</segment>
</net>
<net name="MOSI+" class="0">
<segment>
<pinref part="LVDS" gate="G$1" pin="VSS"/>
<wire x1="33.02" y1="53.34" x2="43.18" y2="53.34" width="0.1524" layer="91"/>
<wire x1="43.18" y1="53.34" x2="43.18" y2="48.26" width="0.1524" layer="91"/>
<pinref part="S1" gate="A" pin="MOSI+"/>
<wire x1="43.18" y1="48.26" x2="76.2" y2="48.26" width="0.1524" layer="91"/>
<label x="35.56" y="53.34" size="1.778" layer="95"/>
</segment>
</net>
<net name="MOSI-" class="0">
<segment>
<pinref part="S1" gate="A" pin="MOSI-"/>
<wire x1="76.2" y1="45.72" x2="40.64" y2="45.72" width="0.1524" layer="91"/>
<wire x1="40.64" y1="45.72" x2="40.64" y2="50.8" width="0.1524" layer="91"/>
<pinref part="LVDS" gate="G$1" pin="VDD"/>
<wire x1="40.64" y1="50.8" x2="33.02" y2="50.8" width="0.1524" layer="91"/>
<label x="35.56" y="50.8" size="1.778" layer="95"/>
</segment>
</net>
<net name="MISO+" class="0">
<segment>
<pinref part="LVDS" gate="G$1" pin="PIO0_1"/>
<wire x1="33.02" y1="43.18" x2="55.88" y2="43.18" width="0.1524" layer="91"/>
<wire x1="55.88" y1="43.18" x2="55.88" y2="55.88" width="0.1524" layer="91"/>
<pinref part="S1" gate="A" pin="MISO1+"/>
<wire x1="55.88" y1="55.88" x2="76.2" y2="55.88" width="0.1524" layer="91"/>
<label x="35.56" y="43.18" size="1.778" layer="95"/>
</segment>
</net>
<net name="MISO-" class="0">
<segment>
<wire x1="60.96" y1="53.34" x2="60.96" y2="35.56" width="0.1524" layer="91"/>
<wire x1="60.96" y1="35.56" x2="40.64" y2="35.56" width="0.1524" layer="91"/>
<wire x1="40.64" y1="35.56" x2="40.64" y2="40.64" width="0.1524" layer="91"/>
<pinref part="LVDS" gate="G$1" pin="PIO0_15"/>
<wire x1="40.64" y1="40.64" x2="33.02" y2="40.64" width="0.1524" layer="91"/>
<label x="35.56" y="40.64" size="1.778" layer="95"/>
<pinref part="S1" gate="A" pin="MISO1-"/>
<wire x1="60.96" y1="53.34" x2="76.2" y2="53.34" width="0.1524" layer="91"/>
</segment>
</net>
<net name="MOSI" class="0">
<segment>
<pinref part="LVDS" gate="G$1" pin="PIO0_4"/>
<pinref part="J1" gate="A" pin="4"/>
<wire x1="-2.54" y1="53.34" x2="-30.48" y2="53.34" width="0.1524" layer="91"/>
<label x="-30.48" y="53.34" size="1.778" layer="95"/>
<label x="-10.16" y="53.34" size="1.778" layer="95"/>
</segment>
</net>
</nets>
</sheet>
</sheets>
</schematic>
</drawing>
</eagle>

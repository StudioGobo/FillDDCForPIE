# Fill DDC For PIE Plugin

Working without a Shared DDC (as is the case for most work from home scenarios) can have a significant impact on productivity. One major pain point is loading maps and launching Play In Editor (PIE) for the first time after an update/build.

This plugin contains a basic Automation Test which opens a list of Maps which are specified in a config file, then runs PIE.  This means that the local DDC has the requisite files in it and the next launch will be faster.

It is intended to be run as part of a larger (generally nightly) automation of:

* Sync from VCS
* (optionally) Build Editor
* Use this plugin to preload the local DDC

This means that in the morning we have significantly reduced load times when we launch for the first time.

The plugin has a more general use than just the DDC pre-cache.  It also allows you to run additional commands once you have loaded the map in PIE.  See Extra Steps below for examples of this.

# Compatibility
The plugin has been tested in Unreal Engine 4.23, 4.24 and 4.25 on Windows 10.  It is expected (but not tested) to run on Unreal Engine 4.22, and on all platforms that can run Unreal Editor.  Currently we are not publishing a binary version of the plugin as it is assumed that the main audience will be teams who are building their game in a C++ project.

# Quick start

## How do I run the command manually?

The test is run as part of the unreal automation framework. The test name is: `FillDDCForPIETest`
The simplest way to run this is via the console command: `Automation RunTests FillDDCForPIETest`

Console commands can be run via the [Output Log](https://docs.unrealengine.com/en-US/Engine/Tools/DevTools/index.html) window.

## How do I automate this?

To make best use of this test, it is recommended to run the test after a scheduled sync. If you use Unreal Game Sync you could add a batch file which runs something like this:
```
UE4Editor.exe "T:/Path/To/Your/Project/Game.uproject" -execcmds="Automation RunTests FillDDCForPIETest" -unattended
```


## What does it do?

For each map defined in the configuration, the test will:

1. Open the map in Editor
    * Wait for load to complete
    * Wait for shaders to compile
2. Run Play In Editor (PIE)
    * Wait for PIE to run
    * Wait for load to complete
    * Wait for shaders to compile
3. Run extra steps (These are defined in the configuration. See below.)
    * Optional Wait for load to complete
    * Optional Wait for shaders to compile
4. Close Play In Editor
    * Wait for load to complete
    * Wait for shaders to compile

Once all maps are run, if `-unattended` is specified on the commandline, quit the editor.


## Setup

The test is configured via the UFillDDCForPIESettings class default object. This object is configured in the *engine.ini files.

```
[/Script/AutomateDDC.FillDDCForPIESettings]
Maps="/Game/Levels/SampleMap1"
+Maps="/Game/Levels/SampleMap2"
```

## Extra steps

As part of this, we have extended support to run custom commands while the map is open and Play In Editor is running.  The steps will be run in order, and each step will wait for a specific message in the Log or a timeout before completing.

#### Example of a custom extra step
When a level is open in PIE, you may want to open the pause menu. To do so, you could implement the console commands:
`openpausemenu` and `closepausemenu`
(actually implementing these is outside fo the scope of this document)

To add these to the automated test, you would do the following:

```
[/Script/AutomateDDC.FillDDCForPIESettings]
Maps="/Game/Levels/SampleMap1"
+Maps="/Game/Levels/SampleMap2"
ExtraSteps=(StepId="Open Pause Menu",ExecCmd="openpausemenu",CompleteCondition=(LogStringForAdvance="Opened pause menu",TimeoutSeconds=600.0), bWaitForSettleAfterStep=True)
+ExtraSteps=(StepId="Close Pause Menu",ExecCmd="closepausemenu",CompleteCondition=(LogStringForAdvance="Pause menu closed",TimeoutSeconds=20.0), bWaitForSettleAfterStep=True)
```

### Complete Conditions

The complete conditions which are defined above are used to progress the test. I.e. if you implement the above console commands, it would be advised to log when the pause menu has loaded, "Opened pause menu", so that the test can continue when you are satisfied it should.
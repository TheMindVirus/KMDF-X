# KMDF-X
Windows Diagnostic Drivers for Raspberry Pi 4 in Low Peripheral Mode (no operation, text file output only)

## Please use with caution, this repository is indended for educational purposes and testing. ##

It may be used as a template for production drivers to start with.

Requires Visual Studio 2019 Community Edition with ARM64 WDK Installed (and C++ ARM64 Build Tools with Spectre Mitigated Libs)

Test Signing On with Windows 10 ARM Insider Build 21277 in Test Mode

Inf2Cat On with /uselocaltime

Output Directory and Intermediate Directory both set to the same location: $(SolutionDir)$(Platform)\\$(ConfigurationName)\

Wpp Tracing Off in favour of Text File Debug

Optional Sections in comments for renaming output file (usually found in C:\Windows\KMDFDriver.log once the driver runs)
and also for reporting a memory resource to Windows for detection. Writing is supported but as yet untested (for production drivers to build upon)

## Installation ##

Open Device Manager on the Raspberry Pi, click anywhere and then click "Action->Add Legacy Hardware"

Click Next, Select "Install the hardware that I manually select from a list (Advanced)" and Click Next again

Make sure "Show All Devices" is selected and then Click Next

Click "Have Disk...", Navigate to the folder that contains the ".inf" file of the driver you want to test and
continue with the rest of the steps in the Add Hardware Wizard.

To Uninstall, right click the device under the "Sample" section and select "Uninstall device".

### You may be prompted with a security warning if driver signature enforcement is enabled. Only follow these instructions if you are testing Windows and you know what you are doing. ###

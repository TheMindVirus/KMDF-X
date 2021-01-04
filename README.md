# KMDF-X
Windows Diagnostic Drivers for Raspberry Pi 4 in Low Peripheral Mode (no operation, text file output only)


Requires Visual Studio 2019 Community Edition with ARM64 WDK Installed (and C++ ARM64 Build Tools with Spectre Mitigated Libs)

Test Signing On with Windows 10 ARM Insider Build 21277 in Test Mode

Inf2Cat On with /uselocaltime

Output Directory and Intermediate Directory both set to the same location: $(SolutionDir)$(Platform)\$(ConfigurationName)\

Wpp Tracing Off in favour of Text File Debug

Optional Sections in comments for renaming output file (usually found in C:\Windows\KMDFDriver.log once the driver runs)
and also for reporting a memory resource to Windows for detection. Writing is supported but as yet untested (for production drivers to build upon)

## Please use with caution, this repository is indended for educational purposes and testing. ##

It may be used as a template for production drivers to start with.


# Download and install Code Composer Studio
	
Download CCS for your platform on the following [link](http://www.ti.com/tool/ccstudio) then install it. 

# Building the project

To be able to build the project some additional resources must be installed. On Code Composer Studio go to (View > CCS App Center) and install the following packages:

* TI-RTOS for MSP43x
* ARM GCC

After that, download the project tarball ([UrineAnalyzerProject.tar](https://github.com/rodolfo-picoreti/UrineAnalyzerMSP/blob/master/UrineAnalyzerProject.tar?raw=true)). On Code Composer Studio go to (File > Import). A new screen will pop up. Select **Code Composer Studio** then **CCS Projects** and click next. Choose **Select archive file** then navigate to the folder where you downloaded the project tarball. Click **Finish** and a new project will appear on your Workspace. You can now build it by right clicking the project and selecting **Build**
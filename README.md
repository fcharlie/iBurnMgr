USB Device Boot Burn Manager [![Build Status](https://travis-ci.org/pocoproject/poco.png?branch=develop)](https://github.com/forcegroup/iBurnMgr)
========
iBurnMgr is a 'Metro Style' USB Drives Burn Boot Manager.

iBurnMgr is developed using C + +, runs on Windows, a production Windows 7+ USB installation disk tools

iBurnMgr used WTL framework, using Direct2D rendering Push Button, close and minimize buttons, and backgrounds, functional areas. Text is output using DirectWrite,MessageBox is TaskDialog replaced.

Format the USB flash drive comes format.com tools rely on Windows, unzip the ISO image using 7z, repair USB flash drive using the Windows ADK's bootsect.exe

In the second edition or third edition, iBurnMgr will eliminate reliance on third-party tools.


iBurnMgr now Support Multilanguage.

###Build
run WDExpress [VisualStudio 2012(2013) for Desktop] 
or Type:
> msbuild iBurnMgr.sln /t:Rebuild /p:Configuration=Release

###Run
last double click iBurnMgr.exe ,or Right-to run with administrator privileges.

Good Luck!



####Screenshot
**Format Disk Warning**
![WARNING](./ImageShow/Image.JPG)

**Create BootUSB Success**
![SUCCRSS](./ImageShow/101.JPG)

**Create Faild**
![FAILD](./ImageShow/Image2.JPG)

**NewUI**
![NewUI](./ImageShow/NewUI.PNG)


**Chinese UI**
![ChineseUI](./ImageShow/zh-CN.PNG)

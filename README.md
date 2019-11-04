# WebLogic-jms-c-c-interface
WebLogic JMS is widely used, and it provides the c/c++, .NET and java interfaces. However, there is little samples for c/c++ interface. Here is one.
This WLS JMS sample is to run on Windows. If you want to run it on Linux, you should change the charactor set convertion functions to Linux platform.
Because this demo default use the UTF-8, if you use GBK or Unicode, you should convert it accordingly. See GBKtoUnicode,UnicodeToGBK,GBKToUTF8,UTF8ToGBK,UTF8ToUnicode,UnicodeToUTF8 functions for detail.


To run this sample, the Client shoud install JDK/JRE(Same as the WLS's java version),
For example:
JRE is located at E:\Oracle\jre\
WLS is located at D:\OraHome_3\wlserver\server\lib
This workspace(Executable file) is located at D:\workspace\JmsCodeMesh\x64\Debug
set path=E:\Oracle\jre\bin\server;D:\workspace\JmsCodeMesh\x64\Debug;%path%
set CLASSPATH=%CLASSPATH%;D:\OraHome_3\wlserver\server\lib\wlthint3client.jar;E:\Oracle\jre\bin\server

Then you can run this demo now.
The default values are listed below:
1.WLS's URL is t3://localhost:7013
#define WEBLOGIC_URL "t3://localhost:7013"
2.WLS's username and password.
#define WLS_USRNAME "weblogic"
#define WLS_PASSWD "welcome1"
3.WLS's JMS connection factory JNDI
#define CONNECTION_FACTORY "QCF"
4.TOPIC_NAME_R should be a valid JMS Queue JNDI, which is used for JMS Message recieve.
#define TOPIC_NAME_R "Queue0"

There is also 2 Listeners for JMS.
1. static void ConnectionExceptionListener(JmsException* pjmsException, void* argument, JMS64I flags)
This callback is called while working if the jms session encounters an Exception.
2. static void MessageListener(JmsMessage* message, void* argument, JMS64I flags)
This callback is called new message arrival.

The Class wrap is under development.

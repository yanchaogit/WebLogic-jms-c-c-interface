// JmsCodeMesh.cpp : 此文件包含 "main" 函数。程序执行将在此处开始并结束。
//
//https://shrikworld.blogspot.com/2014/05/communicate-between-2-weblogic-domains.html
//
//

#include <iostream>
#include <windows.h>
#include <Jms.h>
#include <string>

#define ALLOCATED_OUTPUT 10000
#define STRINGSIZE 10000
#define BYTESSIZE 10
#define CLIENTID "OMS.EC1.BSE1"
#define SUBSCRIPTIONID "MySubsriptionID"


#define WEBLOGIC_URL "t3://localhost:7013"
#define WLS_USRNAME "weblogic"
#define WLS_PASSWD "welcome1"
#define CONNECTION_FACTORY "QCF"
#define TOPIC_NAME "Topic-0"
#define TOPIC_NAME_R "Queue0"
#define OUTPUT_SIZE 10000


//字符集转换
//UNICODE和GBK互转
//GBK-->UniCODE
std::wstring GBKtoUnicode(const char* lpcszString)
{
	int len = strlen(lpcszString);
	int unicodeLen = ::MultiByteToWideChar(CP_ACP, 0, lpcszString, -1, NULL, 0);
	wchar_t* pUnicode = new wchar_t[unicodeLen + 1];
	memset(pUnicode, 0, (unicodeLen + 1) * sizeof(wchar_t));
	::MultiByteToWideChar(CP_ACP, 0, lpcszString, -1, (LPWSTR)pUnicode, unicodeLen);
	std::wstring wString = (wchar_t*)pUnicode;
	delete[] pUnicode;
	return wString;
}

std::string UnicodeToGBK(const wchar_t* lpwcszWString)
{
	char* pElementText;
	int iTextLen;
	// wide char to multi char
	iTextLen = ::WideCharToMultiByte(CP_ACP, 0, lpwcszWString, -1, NULL, 0, NULL, NULL);
	pElementText = new char[iTextLen + 1];
	memset((void*)pElementText, 0, (iTextLen + 1) * sizeof(char));
	::WideCharToMultiByte(CP_ACP, 0, lpwcszWString, 0, pElementText, iTextLen, NULL, NULL);
	std::string strReturn(pElementText);
	delete[] pElementText;
	return strReturn;
}

std::string GBKToUTF8(const std::string& strGBK)
{
	std::string strOutUTF8 = "";
	WCHAR* str1;
	int n = MultiByteToWideChar(CP_ACP, 0, strGBK.c_str(), -1, NULL, 0);
	str1 = new WCHAR[n];
	MultiByteToWideChar(CP_ACP, 0, strGBK.c_str(), -1, str1, n);
	n = WideCharToMultiByte(CP_UTF8, 0, str1, -1, NULL, 0, NULL, NULL);
	char* str2 = new char[n];
	WideCharToMultiByte(CP_UTF8, 0, str1, -1, str2, n, NULL, NULL);
	strOutUTF8 = str2;
	delete[] str1;
	str1 = NULL;
	delete[] str2;
	str2 = NULL;
	return strOutUTF8;
}

std::string UTF8ToGBK(const std::string& strUTF8)
{
	int len = MultiByteToWideChar(CP_UTF8, 0, strUTF8.c_str(), -1, NULL, 0);
	WCHAR* wszGBK = new WCHAR[len + 1];
	memset(wszGBK, 0, (len + 1) * sizeof(WCHAR));
	MultiByteToWideChar(CP_UTF8, 0, (LPCSTR)strUTF8.c_str(), -1, wszGBK, len);
	len = WideCharToMultiByte(CP_ACP, 0, wszGBK, -1, NULL, 0, NULL, NULL);
	char* szGBK = new char[len + 1];
	memset(szGBK, 0, len + 1);
	WideCharToMultiByte(CP_ACP, 0, wszGBK, -1, szGBK, len, NULL, NULL);
	//strUTF8 = szGBK;
	std::string strTemp(szGBK);
	delete[] szGBK;
	szGBK = NULL;
	delete[] wszGBK;
	wszGBK = NULL;
	return strTemp;
}

std::wstring UTF8ToUnicode(const char* lpcszString)
{
	int len = strlen(lpcszString);
	int unicodeLen = ::MultiByteToWideChar(CP_UTF8, 0, lpcszString, -1, NULL, 0);
	wchar_t* pUnicode;
	pUnicode = new wchar_t[unicodeLen + 1];
	memset((void*)pUnicode, 0, (unicodeLen + 1) * sizeof(wchar_t));
	::MultiByteToWideChar(CP_UTF8, 0, lpcszString, -1, (LPWSTR)pUnicode, unicodeLen);
	std::wstring wstrReturn(pUnicode);
	delete[] pUnicode;
	return wstrReturn;
}

std::string UnicodeToUTF8(const wchar_t* lpwcszWString)
{
	char* pElementText;
	int iTextLen = ::WideCharToMultiByte(CP_UTF8, 0, (LPWSTR)lpwcszWString, -1, NULL, 0, NULL, NULL);
	pElementText = new char[iTextLen + 1];
	memset((void*)pElementText, 0, (iTextLen + 1) * sizeof(char));
	::WideCharToMultiByte(CP_UTF8, 0, (LPWSTR)lpwcszWString, -1, pElementText, iTextLen, NULL, NULL);
	std::string strReturn(pElementText);
	delete[] pElementText;
	return strReturn;
}

// Set up a simple configuration that logs on the console.
//

int gotMessage = 0;

extern "C" void signal_handler(int signum)
{
	printf("\nctrl-C has been activated");
	gotMessage = 1;
	printf("\nctrl-C has been activated");
}

#ifdef _WIN32
bool ctrlhandler(DWORD fdwctrltype)
{
	switch (fdwctrltype)
	{
		// handle the ctrl-c signal.
	case CTRL_C_EVENT:
		signal_handler(fdwctrltype);
		return(false);
		// ctrl-close: confirm that the user wants to exit.
		//case CTRL_CLOSE_EVENT:
		//	printf("ctrl-close event\n\n");
		//	return(true);
		//	// pass other signals to the next handler.
		//case CTRL_BREAK_EVENT:
		//	printf("ctrl-break event\n\n");
		//	return false;
		//case CTRL_LOGOFF_EVENT:
		//	printf("ctrl-logoff event\n\n");
		//	return false;
		//case CTRL_SHUTDOWN_EVENT:
		//	printf("ctrl-shutdown event\n\n");
		//	return false;
	default:
		return false;
	}
}
#endif

//异常监听(JmsException*, void*, JMS64I)
static void
ConnectionExceptionListener(JmsException* pjmsException, void* argument, JMS64I flags)
{
	JmsString oneString;
	char oneStringData[STRINGSIZE];
	char errorString[STRINGSIZE];
	int retCode = JMS_NO_ERROR;

	oneStringData[STRINGSIZE - 1] = '\0';

	oneString.stringType = CSTRING;
	oneString.uniOrC.string = oneStringData;
	oneString.allocatedSize = STRINGSIZE;
	if ((retCode = JmsThrowableGetMessage(pjmsException, &oneString, 0)) != JMS_NO_ERROR) {
		sprintf_s(errorString, "%s %d", __FILE__, __LINE__);
		return;
	}
	fprintf(stderr, "Connection Exception occured, message: %s\n", oneStringData);
/*	extern int JMSENTRY JmsExceptionGetErrorCode(
		JmsException * exception,
		JmsString * errorCode,
		JMS64I              flags
	);*/
	return;
}

//消息监听
static void
MessageListener(JmsMessage* message, void* argument, JMS64I flags)
{

	int retCode = 500;
	//	unsigned char rByte;
	char errorString[STRINGSIZE];
	JmsString oneString;
	JmsString tagString;
	char oneStringData[STRINGSIZE];

	//	float price;

	//	JMS32I rInt;

	char retValData[STRINGSIZE];
	JmsString retVal;

	JmsEnumeration* rEnum = NULL;

	int numTags = 0;

	retVal.stringType = CSTRING;
	retVal.uniOrC.string = retValData;
	retVal.allocatedSize = STRINGSIZE;


	oneStringData[STRINGSIZE - 1] = '\0';

	oneString.stringType = CSTRING;
	oneString.uniOrC.string = oneStringData;
	oneString.allocatedSize = STRINGSIZE;

	tagString.stringType = CSTRING;

	if ((retCode = JmsTextMessageGetText(message, &oneString, 0)) != JMS_NO_ERROR) {
		sprintf_s(errorString, "%s %d", __FILE__, __LINE__);
		return;
	}
	oneStringData[STRINGSIZE - 1] = '\0';

	fprintf(stderr, "Received string message: %s\n", oneStringData);

	if ((retCode = JmsMessageAcknowledge(message, 0))
		!= JMS_NO_ERROR) {
sprintf_s(errorString, "%s %d", __FILE__, __LINE__);

return;
	}

	return;
}

class WLSJMSSession
{
	//Members
private: JmsContext* jmsContext = NULL;
private: JmsConnectionFactory* jmsFactory = NULL;
private: JmsConnection* jmsConnection = NULL;
private: JmsSession* jmsReaderSession = NULL;
private: JmsSession* jmsWriterSession = NULL;
private: JmsTextMessage* jmsMessage = NULL;
private: JmsDestination* jmsDestination = NULL;
private: JmsProducer* jmsProducer = NULL;

	   //
private: JmsString jstring;
private: char retValData[STRINGSIZE];

	   //initial
public: WLSJMSSession()
{
	jstring.stringType = CSTRING;
	jstring.uniOrC.string = (char*)retValData;
	jstring.allocatedSize = STRINGSIZE;
	return;
}
public: WLSJMSSession* init(JmsString url, JmsString connectionfactory, JmsString username, JmsString password, JmsString qname)
{
	int retCode = JMS_NO_ERROR;
	//获取上下文环境
	if ((retCode = JmsContextCreate(&url, NULL, &username, &password, &jmsContext, 0))
		!= JMS_NO_ERROR) {
		(void)fprintf(stdout, "ERROR: Could not create a context: %d,%s\n", retCode, jstring.uniOrC.string);
		return NULL;
	}
	//获取连接工厂信息
	jstring.uniOrC.string = (char*)CONNECTION_FACTORY;
	if ((retCode = JmsContextCreateConnectionFactory(jmsContext, &connectionfactory,
		&jmsFactory, 0)) != JMS_NO_ERROR) {
		(void)fprintf(stdout, "ERROR: Could not create a connection factory: %d\n", retCode);
		return NULL;
	}
	//创建连接工厂
	if ((retCode = JmsConnectionFactoryCreateConnection(jmsFactory,
		&jmsConnection, 0)) != JMS_NO_ERROR) {
		(void)fprintf(stdout, "ERROR: Could not create a connection: %d\n", retCode);
		return NULL;
	}

	/*		if ((retCode = JmsConnectionSetClientId(jmsConnection, &clientID, 0)) != JMS_NO_ERROR) {
				(void)fprintf(stdout, "ERROR: Could not create the consumer1: %d\n", retCode);
				return NULL;
			}*/

			//创建JMS Session,transacted or not
	if ((retCode = JmsConnectionCreateSession(jmsConnection, 0,
		AUTO_ACKNOWLEDGE, &jmsReaderSession, 0)) != JMS_NO_ERROR) {
		(void)fprintf(stdout, "ERROR: Could not create reader connection: %d\n", retCode);
		return NULL;
	}
	//创建消费者，生产者，或者主题订阅&发布者
	jstring.uniOrC.string = (char*)TOPIC_NAME;
	if ((retCode = JmsDestinationCreate(jmsContext, &qname, &jmsDestination, 0))
		!= JMS_NO_ERROR) {
		(void)fprintf(stdout, "ERROR: Could not Create the Destination: %d\n", retCode);
		return NULL;
	}
	if ((retCode = JmsSessionCreateProducer(jmsReaderSession, jmsDestination, &jmsProducer, 0))
		!= JMS_NO_ERROR) {
		(void)fprintf(stdout, "ERROR: Could not Create the Producer: %d\n", retCode);
		return NULL;
	}

	//启动JMS连接
	if ((retCode = JmsConnectionStart(jmsConnection, (JMS64I)0))
		!= JMS_NO_ERROR) {
		(void)fprintf(stdout, "ERROR: Could not start the connection: %d\n", retCode);
		return NULL;
	}
	//关闭Destination
	if ((retCode = JmsDestinationDestroy(jmsDestination, 0))
		!= JMS_NO_ERROR) {
		(void)fprintf(stdout, "ERROR: Could not destroy the destination: %d\n", retCode);
		return NULL;
	}


	return this;
}
public: JmsTextMessage* creteJMSTextMessage(char* message)
{
	JmsString jmstext;
	JmsTextMessage* jmsTextMessage = NULL;
	std::string utf8strmessage;
	utf8strmessage = ::GBKToUTF8(message);
	jmstext.stringType = CSTRING;
	jmstext.uniOrC.string = (char*)utf8strmessage.c_str();
	jmstext.allocatedSize = utf8strmessage.length();

	int retCode = JMS_NO_ERROR;
	if (retCode = JmsSessionTextMessageCreate(jmsReaderSession, &jmstext, &jmsTextMessage, 0)
		!= JMS_NO_ERROR)
	{
		(void)fprintf(stdout, "ERROR: Could not JMS Text Message: %d,%s\n", retCode, message);
		return NULL;
	}
	return jmsTextMessage;
}
public: int sendTextMessage(char* messagetext)
{
	int retCode = JMS_NO_ERROR;
	JmsTextMessage* jmsTextMessage = jmsTextMessage = creteJMSTextMessage(messagetext);
	if(NULL == jmsTextMessage)
	{
		return 1;
	}
	if (retCode = JmsProducerSend(jmsProducer, NULL, jmsTextMessage, 0)
		!= JMS_NO_ERROR)
	{
		(void)fprintf(stdout, "ERROR: Could not Send Text Message: %d,%s\n", retCode, messagetext);
		return 1;
	}
	return 0;
}
	  public: JmsTextMessage* creteJMSTextMessageW(char* message)
	  {
		  JmsString jmstext;
		  JmsTextMessage* jmsTextMessage = NULL;
		  std::wstring unicodestrmessage;
		  unicodestrmessage = ::GBKtoUnicode(message);
		  jmstext.stringType = UNISTRING;
		  jmstext.uniOrC.uniString.data = (void*)unicodestrmessage.c_str();
		  jmstext.uniOrC.uniString.length = unicodestrmessage.length()*2;
//		  jmstext.allocatedSize = unicodestrmessage.length();

		  int retCode = JMS_NO_ERROR;
		  if (retCode = JmsSessionTextMessageCreate(jmsReaderSession, &jmstext, &jmsTextMessage, 0)
			  != JMS_NO_ERROR)
		  {
			  (void)fprintf(stdout, "ERROR: Could not Create UnicodeText Message: %d,%s\n", retCode, message);
			  return NULL;
		  }
		  return jmsTextMessage;
	  }
public: int sendTextMessageW(char* messagetext)
{
	int retCode = JMS_NO_ERROR;
	JmsTextMessage* jmsTextMessage = jmsTextMessage = creteJMSTextMessageW(messagetext);
	if (NULL == jmsTextMessage)
	{
		return 1;
	}
	if (retCode = JmsProducerSend(jmsProducer, NULL, jmsTextMessage, 0)
		!= JMS_NO_ERROR) 
	{
		(void)fprintf(stdout, "ERROR: Could not Send UnicodeText Message: %d,%s\n", retCode, messagetext);
		return 1;
	}
	return 0;
}
public: int sendBytesMessage(UCHAR* messagetext, int len)
{

	return 0;
}

public: int sendStreamMessage(UCHAR* messagetext, int len)
{

	return 0;
}
public: int sendMapMessageW(UCHAR* messagetext, int len)
{

	return 0;
}

public: void close()
{
	int retCode = JMS_NO_ERROR;
	if ((retCode = JmsProducerClose(jmsProducer, 0))
		!= JMS_NO_ERROR) {
		(void)fprintf(stdout, "ERROR: Could not close jmsProducer: %d\n", retCode);
		return ;
	}
	if (NULL != jmsReaderSession && (retCode = JmsSessionClose(jmsReaderSession, 0))
		!= JMS_NO_ERROR) {
		(void)fprintf(stdout, "ERROR: Could not close reader session: %d\n", retCode);
		return;
	}
	jmsReaderSession = NULL;

	if (NULL != jmsConnection && (retCode = JmsConnectionClose(jmsConnection, 0))
		!= JMS_NO_ERROR) {
		(void)fprintf(stdout, "ERROR: Could not close connection: %d\n", retCode);
		return;
	}
	jmsConnection = NULL;
	if (NULL != jmsFactory && (retCode = JmsConnectionFactoryDestroy(jmsFactory, 0))
		!= JMS_NO_ERROR) {
		(void)fprintf(stdout, "ERROR: Could not close connection factory: %d\n", retCode);
		return;
	}
	jmsFactory = NULL;

	if (NULL != jmsContext && (retCode = JmsContextDestroy(jmsContext, 0))
		!= JMS_NO_ERROR) {
		(void)fprintf(stdout, "ERROR: Could not destroy context: %d\n", retCode);
		return ;
	}
	jmsContext = NULL;
	(void)fprintf(stdout, "INFO: Completed normally!\n");
}


};

int
main(int argc, char* argv[])
{
//	JmsString url;
//	JmsString connectionfactory;
//	JmsString username;
//	JmsString password;
//	JmsString qname;
//	WLSJMSSession ss;
//	char szurl[STRINGSIZE];
//	char szconnectionfactory[STRINGSIZE];
//	char szusername[STRINGSIZE];
//	char szpassword[STRINGSIZE];
//	char szqname[STRINGSIZE];
//	char szmessage[STRINGSIZE];
//
//	strcpy_s(szurl, "t3://localhost:7001");
//	strcpy_s(szconnectionfactory, "QCF");
//	strcpy_s(szusername, "weblogic");
//	strcpy_s(szpassword, "welcome1");
//	strcpy_s(szqname, "SAF.MyTestQueue");
//	strcpy_s(szmessage, "ABCD这是要测试的串。");
//
//	url.stringType = CSTRING;
//	url.uniOrC.string = szurl;
//	url.allocatedSize = STRINGSIZE;
//
//	connectionfactory.stringType = CSTRING;
//	connectionfactory.uniOrC.string = szconnectionfactory;
//	connectionfactory.allocatedSize = STRINGSIZE;
//
//	username.stringType = CSTRING;
//	username.uniOrC.string = szusername;
//	username.allocatedSize = STRINGSIZE;
//
//	password.stringType = CSTRING;
//	password.uniOrC.string = szpassword;
//	password.allocatedSize = STRINGSIZE;
//
//	qname.stringType = CSTRING;
//	qname.uniOrC.string = szqname;
//	qname.allocatedSize = STRINGSIZE;
//
//	if (NULL != ss.init(url, connectionfactory, username, password, qname))
//	{
////		while (true)
////		{
////			std::cout << "请输入要发送的消息，输入“quit”退出: ";
////			std::string mytestmessage;
////			std::getline(std::cin, mytestmessage);
////			if (mytestmessage.compare("quit")==0)
////			{
////				break;
////			}
////			mytestmessage+="====>UTF-8";
////			ss.sendTextMessage((char*)mytestmessage.c_str());
////			mytestmessage += "====>Unicode";
////			ss.sendTextMessageW((char*)mytestmessage.c_str());
////		}
//		for (int j = 0; j < 100; j++)
//		{
//			sprintf_s(szmessage, "测试消息#%d -- UTF-8", j);
//			ss.sendTextMessage((char*)szmessage);
//			sprintf_s(szmessage, "测试消息#%d -- Unicode", j);
//			ss.sendTextMessageW((char*)szmessage);
//		}
//		ss.close();
//		return 0;
//	}
//	else
//	{
//		return 1;
//	}


#ifdef _WIN32
	SetConsoleCtrlHandler((PHANDLER_ROUTINE)ctrlhandler, true);
#else

	signal(SIGINT, signal_handler);
	signal(SIGTERM, signal_handler);
	signal(SIGILL, signal_handler);
	signal(SIGFPE, signal_handler);
	signal(SIGSEGV, signal_handler);
	signal(SIGBREAK, signal_handler);
	signal(SIGABRT, signal_handler);
#endif

	JmsContext* jmsContext = NULL;
	JmsConnectionFactory* jmsFactory = NULL;
	JmsConnection* jmsConnection = NULL;
	JmsSession* jmsReaderSession = NULL;
	JmsSession* jmsWriterSession = NULL;
	JmsTextMessage* jmsMessage = NULL;
	JmsDestination* jmsDestination = NULL;
	JmsProducer* jmsProducer = NULL;
	JmsConsumer* jmsConsumer = NULL;
	JmsTopic* topic = NULL;
	int retCode;
	JmsString retData;

	char retValData[STRINGSIZE];


	retData.stringType = CSTRING;
	retData.uniOrC.string = retValData;
	retData.allocatedSize = STRINGSIZE;

	JmsString clientID;

	clientID.stringType = CSTRING;
	clientID.uniOrC.string = (char*)CLIENTID;

	JmsString jstring;
	JmsString cfname;
	JmsString wlsusername;
	JmsString wlspassword;
	JmsString jms_topic_name;
	jms_topic_name.stringType = CSTRING;
	jms_topic_name.uniOrC.string = (char*)TOPIC_NAME;

	int count = 0;

	jstring.stringType = CSTRING;
	jstring.uniOrC.string = (char*)WEBLOGIC_URL;

	cfname.stringType = CSTRING;
	cfname.uniOrC.string = (char*)CONNECTION_FACTORY;

	wlsusername.stringType = CSTRING;
	wlsusername.uniOrC.string = (char*)WLS_USRNAME;

	wlspassword.stringType = CSTRING;
	wlspassword.uniOrC.string = (char*)WLS_PASSWD;

	if ((retCode = JmsContextCreate(&jstring, NULL, &wlsusername, &wlspassword, &jmsContext, 0))
		!= JMS_NO_ERROR) {
		(void)fprintf(stdout, "ERROR: Could not create a context: %d,%s\n", retCode, jstring.uniOrC.string);
		return (1);
	}

	jstring.uniOrC.string = (char*)CONNECTION_FACTORY;
	if ((retCode = JmsContextCreateConnectionFactory(jmsContext, &jstring,
		&jmsFactory, 0)) != JMS_NO_ERROR) {
		(void)fprintf(stdout, "ERROR: Could not create a connection factory: %d\n", retCode);
		return (1);
	}

	if ((retCode = JmsConnectionFactoryCreateConnection(jmsFactory,
		&jmsConnection, 0)) != JMS_NO_ERROR) {
		(void)fprintf(stdout, "ERROR: Could not create a connection: %d\n", retCode);
		return (1);
	}

	if ((retCode = JmsConnectionSetClientId(jmsConnection, &clientID, 0)) != JMS_NO_ERROR) {
		(void)fprintf(stdout, "ERROR: Could not create the consumer1: %d\n", retCode);
		return(1);
	}

	if ((retCode = JmsConnectionCreateSession(jmsConnection, 0,
		AUTO_ACKNOWLEDGE, &jmsReaderSession, 0)) != JMS_NO_ERROR) {
		(void)fprintf(stdout, "ERROR: Could not create reader connection: %d\n", retCode);
		return(1);
	}

	/*if ((retCode = JmsConnectionCreateSession(jmsConnection, 0,
	AUTO_ACKNOWLEDGE, &jmsWriterSession, 0)) != JMS_NO_ERROR) {
	(void) fprintf(stdout, "ERROR: Could not create writer connection: %d\n", retCode);
	return(1);
	}*/

	jstring.uniOrC.string = (char*)TOPIC_NAME_R;
	if ((retCode = JmsDestinationCreate(jmsContext, &jstring, &jmsDestination, 0))
		!= JMS_NO_ERROR) {
		(void)fprintf(stdout, "ERROR: Could not find the topic: %d\n", retCode);
		return(1);
	}

	/*if ((retCode = JmsSessionCreateTemporaryTopic (jmsReaderSession,&jmsDestination, 0)) != JMS_NO_ERROR) {
	(void) fprintf(stdout, "ERROR: Could not create reader connection: %d\n", retCode);
	return(1);
	}
	*/

	/* if ((retCode = JmsSessionCreateProducer(jmsWriterSession, jmsDestination,
	&jmsProducer, 0)) != JMS_NO_ERROR) {
	(void) fprintf(stdout, "ERROR: Could not create the producer: %d\n", retCode);
	return(1);
	}*/


/*
	if ((retCode = JmsConnectionGetClientId(jmsConnection, &retData, 0))
		!= JMS_NO_ERROR) {
		(void)fprintf(stdout, "ERROR: Could not Get the Clientid: %d\n", retCode);


		return (1);
	}

	if (strcmp(retValData, CLIENTID)) {
		(void)fprintf(stdout, "ERROR: Client id NOT Match: %d\n", retCode);

		//return (0);
	}

	(void)fprintf(stdout, "ClientID: %s\n", retValData);
*/

/*	if ((retCode = 	if ((retCode = JmsSessionCreateProducer(jmsReaderSession, jmsDestination, &jmsProducer, 0))
		!= JMS_NO_ERROR) {
		(void)fprintf(stdout, "ERROR: Could not Create the Producer: %d\n", retCode);
		return NULL;
	}
*/
	//create Consumer;
	if ((retCode = JmsSessionCreateConsumer(jmsReaderSession, jmsDestination, NULL,0,&jmsConsumer, 0))
		!= JMS_NO_ERROR) {
		(void)fprintf(stdout, "ERROR: Could not Create the Consumer: %d\n", retCode);
		return NULL;
	}

	if ((retCode = JmsDestinationDestroy(jmsDestination, 0))
		!= JMS_NO_ERROR) {
		(void)fprintf(stdout, "ERROR: Could not destroy the destination: %d\n", retCode);
		return(1);
	}

	if ((retCode = JmsConsumerSetMessageListener(jmsConsumer,
		MessageListener, NULL, 0)) != JMS_NO_ERROR) {
		(void)fprintf(stdout, "ERROR: Could not set the message listener: %d\n", retCode);
		return(1);
	}
	//
	if ((retCode = JmsConnectionSetExceptionListener(jmsConnection,
		ConnectionExceptionListener, NULL, 0)) != JMS_NO_ERROR) {
		(void)fprintf(stdout, "ERROR: Could not set the Connection Exception Handler listener: %d\n", retCode);
		return(1);
	}
	//
	if ((retCode = JmsConnectionStart(jmsConnection, (JMS64I)0))
		!= JMS_NO_ERROR) {
		(void)fprintf(stdout, "ERROR: Could not start the connection: %d\n", retCode);
		return(1);
	}

	while (gotMessage == 0) {

#ifdef WIN32
		Sleep(1 * 1000);
#else
		sleep(1);
#endif
	}

	printf("\nClosing...");

	if ((retCode = JmsConsumerClose(jmsConsumer, 0))
		!= JMS_NO_ERROR) {
		(void)fprintf(stdout, "ERROR: Could not close consumer: %d\n", retCode);
		return(1);
	}
	if ((retCode = JmsSessionClose(jmsReaderSession, 0))
		!= JMS_NO_ERROR) {
		(void)fprintf(stdout, "ERROR: Could not close reader session: %d\n", retCode);
		return(1);
	}

	if ((retCode = JmsConnectionClose(jmsConnection, 0))
		!= JMS_NO_ERROR) {
		(void)fprintf(stdout, "ERROR: Could not close connection: %d\n", retCode);
		return(1);
	}

	if ((retCode = JmsConnectionFactoryDestroy(jmsFactory, 0))
		!= JMS_NO_ERROR) {
		(void)fprintf(stdout, "ERROR: Could not close connection factory: %d\n", retCode);
		return(1);
	}
	if ((retCode = JmsContextDestroy(jmsContext, 0))
		!= JMS_NO_ERROR) {
		(void)fprintf(stdout, "ERROR: Could not destroy context: %d\n", retCode);
		return(1);
	}

	(void)fprintf(stdout, "INFO: Completed normally!\n");
	return (0);
}

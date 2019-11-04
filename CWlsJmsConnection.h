#pragma once
#include "Jms.h"
#include <string>

class CWlsJmsConnection
{
	//Handles
private: JmsContext* jmsContext = NULL;
private: JmsConnectionFactory* jmsFactory = NULL;
private: JmsConnection* jmsConnection = NULL;
private: std::string m_lastErrorText;
private: int m_lastError;

public:CWlsJmsConnection();
public:~CWlsJmsConnection();
	  //Create the Connection to WLS JMS Server
	  //On Success, it returns JMS_NO_ERROR(0)
	  //On Failure, it returns JMS_INPUT_PARAM_ERROR,JMS_MALLOC_ERROR,JMS_JVM_ERROR,JMS_NEED_SPACE,JMS_INVALID_EXCEPTION_TYPE or JMS_SYSTEM_ERROR
	  //              Call GetLastError to find out the detailed errors.
	  //Both the init functions are the same return values.
public:int initw(JmsString url, JmsString connectionfactory, JmsString username, JmsString password);
	  //Create the Connection to WLS JMS Server using the native strings
public:int init(char* url, char* connectionfactory, char* username, char* password);
	  //Get the Last error occured
public:std::string GetLastErrorText();
	  //Get the last Error Code 
public: int GetLastError();
	  //Set the last Error code & last Error Text, default behavior is to reset the last error code and Error Text
	  //Internal use only.
protected:void setLastError(int iErrorCode = JMS_NO_ERROR, std::string strErrorText="");
public:void Close();
public:


};


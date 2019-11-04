#include "CWlsJmsConnection.h"

CWlsJmsConnection::CWlsJmsConnection()
{
}

CWlsJmsConnection::~CWlsJmsConnection()
{
	Close();
}

int CWlsJmsConnection::initw(JmsString url, JmsString connectionfactory, JmsString username, JmsString password)
{
	setLastError();
	return 0;
}

int CWlsJmsConnection::init(char* url, char* connectionfactory, char* username, char* password)
{
	setLastError();
	return 0;
}

std::string CWlsJmsConnection::GetLastErrorText()
{
	return m_lastErrorText;
}

int CWlsJmsConnection::GetLastError()
{
	return m_lastError;
}

void CWlsJmsConnection::setLastError(int iErrorCode, std::string strErrorText)
{
	m_lastErrorText = strErrorText;
	m_lastError = iErrorCode;
}

void CWlsJmsConnection::Close()
{
}

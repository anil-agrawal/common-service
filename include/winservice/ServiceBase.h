#ifndef CServiceBase_H
#define CServiceBase_H

// #include <strsafe.h>
#include <stdio.h>
#include <winsock2.h>
#include <windows.h>
#include <assert.h>
#include "../../include/CommonUtil.h"

class CServiceBase
{
public:
  static BOOL Run(CServiceBase &service);

  // Service object constructor.
  CServiceBase(LPSTR pszServiceName,
               BOOL fCanStop = TRUE,
               BOOL fCanShutdown = TRUE,
               BOOL fCanPauseContinue = FALSE);

  // Service object destructor.
  virtual ~CServiceBase(void);

  void Stop();

protected:
  virtual void OnStart(DWORD dwArgc, LPSTR *pszArgv);
  virtual void OnStop();
  virtual void OnPause();
  virtual void OnContinue();
  virtual void OnShutdown();
  void SetServiceStatus(DWORD dwCurrentState,
                        DWORD dwWin32ExitCode = NO_ERROR,
                        DWORD dwWaitHint = 0);

  void WriteEventLogEntry(const char *pszMessage, WORD wType);

  void WriteErrorLogEntry(const char *pszFunction,
                          DWORD dwError = GetLastError());

private:
  static void WINAPI ServiceMain(DWORD dwArgc, LPSTR *lpszArgv);
  static void WINAPI ServiceCtrlHandler(DWORD dwCtrl);

  void Start(DWORD dwArgc, LPSTR *pszArgv);
  void Pause();
  void Continue();
  void Shutdown();
  static CServiceBase *s_service;
  LPSTR m_name;
  SERVICE_STATUS m_status;
  SERVICE_STATUS_HANDLE m_statusHandle;
};

#endif
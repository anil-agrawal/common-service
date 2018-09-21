#ifndef CServiceBaseImpl_H
#define CServiceBaseImpl_H

#include "../../include/ServiceProcess.h"
#include "../../include/winservice/ServiceBase.h"
#include <WinReg.h>
#include <iostream>

class CServiceBaseImpl : public CServiceBase
{
public:
  CServiceBaseImpl(LPSTR pszServiceName,
                   BOOL fCanStop = TRUE,
                   BOOL fCanShutdown = TRUE,
                   BOOL fCanPauseContinue = FALSE);

  static ServiceProcess *serviceProcess;

  virtual ~CServiceBaseImpl(void);

protected:
  BOOL m_bDefaultDontShowUIStatus;
  virtual void OnStart(DWORD dwArgc, LPSTR *pszArgv);
  virtual void OnStop();
  virtual void OnShutdown();
  static DWORD WINAPI ServiceWorkerThread(void *lpParam);
};

#endif
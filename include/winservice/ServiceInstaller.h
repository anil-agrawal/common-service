#ifndef CServiceInstaller_H
#define CServiceInstaller_H

#include <stdio.h>
#include <winsock2.h>
#include <windows.h>
#include "../../include/CommonUtil.h"

class CServiceInstaller
{
public:
  void InstallService(LPSTR pszServiceName,
                      LPSTR pszDisplayName,
                      LPSTR pszServiceDesc,
                      DWORD dwStartType,
                      LPSTR pszDependencies,
                      LPSTR pszAccount,
                      LPSTR pszPassword);

  void UninstallService(LPSTR pszServiceName);
  void RunService(LPSTR pszServiceName);
  void StopService(LPSTR pszServiceName);
};
#endif
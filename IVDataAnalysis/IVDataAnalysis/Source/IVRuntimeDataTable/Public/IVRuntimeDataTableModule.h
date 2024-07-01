#pragma once

#include "CoreMinimal.h"
#include "Modules/ModuleManager.h"
DECLARE_LOG_CATEGORY_EXTERN(LogIVDataTable, Log, All);
DECLARE_LOG_CATEGORY_EXTERN(LogIVExcel, Log, All);

class FIVRuntimeDataTableModule : public IModuleInterface
{
public:
    virtual void StartupModule() override;
    virtual void ShutdownModule() override;
};

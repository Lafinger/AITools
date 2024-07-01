#include "IVRuntimeDataTableModule.h"

#include "Interfaces/IPluginManager.h"

#define LOCTEXT_NAMESPACE "FIVRuntimeDataTableModule"

DEFINE_LOG_CATEGORY(LogIVDataTable);
DEFINE_LOG_CATEGORY(LogIVExcel);
void FIVRuntimeDataTableModule::StartupModule()
{

	
}

void FIVRuntimeDataTableModule::ShutdownModule()
{

}


IMPLEMENT_MODULE(FIVRuntimeDataTableModule, IVRuntimeDataTable)
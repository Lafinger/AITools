#include "IVExecutableExtension.h"

#define LOCTEXT_NAMESPACE "FIVExecutableExtensionModule"

DEFINE_LOG_CATEGORY(IVProcess);

void FIVExecutableExtensionModule::StartupModule()
{
    
}

void FIVExecutableExtensionModule::ShutdownModule()
{
    
}

#undef LOCTEXT_NAMESPACE
    
IMPLEMENT_MODULE(FIVExecutableExtensionModule, IVExecutableExtension)
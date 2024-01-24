#include "SecondModule.h"

DEFINE_LOG_CATEGORY(SecondModule);

#define LOCTEXT_NAMESPACE "FSecondModule"

void FSecondModule::StartupModule()
{
	UE_LOG(SecondModule, Warning, TEXT("SecondModule module has been loaded"));
}

void FSecondModule::ShutdownModule()
{
	UE_LOG(SecondModule, Warning, TEXT("SecondModule module has been unloaded"));
}

#undef LOCTEXT_NAMESPACE

IMPLEMENT_MODULE(FSecondModule, SecondModule)
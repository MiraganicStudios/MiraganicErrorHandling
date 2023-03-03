#include "ECErrorCodesEditorModule.h"

#include "ECGraphPinFactory_ErrorCode.h"
#include "EdGraphUtilities.h"

void FECErrorCodesEditorModule::StartupModule()
{
	ErrorCodePinFactory = MakeShareable(new FECGraphPinFactory_ErrorCode());
	FEdGraphUtilities::RegisterVisualPinFactory(ErrorCodePinFactory);
}

void FECErrorCodesEditorModule::ShutdownModule()
{
	if (ErrorCodePinFactory.IsValid())
	{
		FEdGraphUtilities::UnregisterVisualPinFactory(ErrorCodePinFactory);
		ErrorCodePinFactory.Reset();		
	}
}
    
IMPLEMENT_MODULE(FECErrorCodesEditorModule, ErrorCodesEditor)
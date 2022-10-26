// Distributed under the MIT License (MIT) (see accompanying LICENSE file)

#pragma once

class FImGuiModuleManager;
class UGameViewportClient;
class UImGuiInputHandler;

class FImGuiInputHandlerFactory
{
public:

	static UImGuiInputHandler* NewHandler(const FStringClassReference& HandlerClassReference, FImGuiModuleManager* ModuleManager, UGameViewportClient* GameViewport, int32 ContextIndex);
    static UImGuiInputHandler* NewHandler(UPackage* OuterPkg, const FStringClassReference& HandlerClassReference, FImGuiModuleManager* ModuleManager, int ContextIndex);


	static void ReleaseHandler(UImGuiInputHandler* Handler);
};

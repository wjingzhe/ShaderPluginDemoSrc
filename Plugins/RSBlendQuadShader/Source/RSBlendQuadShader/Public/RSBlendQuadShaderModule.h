/******************************************************************************
* The MIT License (MIT)
*
* Copyright (c) 2016 Jingz
*
* Permission is hereby granted, free of charge, to any person obtaining a copy
* of this software and associated documentation files (the "Software"), to deal
* in the Software without restriction, including without limitation the rights
* to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
* copies of the Software, and to permit persons to whom the Software is
* furnished to do so, subject to the following conditions:
*
* The above copyright notice and this permission notice shall be included in
* all copies or substantial portions of the Software.
*
* THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
* IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
* FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
* AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
* LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
* OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
* THE SOFTWARE.
******************************************************************************/

#pragma once

#include "ModuleManager.h"
#include "Private/RSBlendQuadShaderDeclaration.h"
#include "EngineModule.h"
#include <atomic>

class RSBLENDQUADSHADER_API FRSBlendQuadShaderModule : public IModuleInterface
{
public:
	FRSBlendQuadShaderModule(){}
	/** IModuleInterface implementation */
	virtual void StartupModule() override;
	virtual void ShutdownModule() override;

	FRSBlendQuadShaderModule(int32 SizeX, int32 SizeY, ERHIFeatureLevel::Type ShaderFeatureLevel);
	~FRSBlendQuadShaderModule();

	/************************************************************************/
	/* Let the user change rendertarget during runtime if they want to      */
	/************************************************************************/
	void ExecutePixelShader(FTexture2DRHIRef NewInputTexture, FShaderResourceViewRHIRef NewInputTextureSRV);

	//Only call this from the render thread
	void ExecutePixelShaderInternal();//FPostOpaqueRenderParameters& p



private:
	//jingz 得改成多线程安全的变量
	std::atomic<bool> bIsPixelShaderExecuting;
	bool bMustRegenerateSRV;
	bool bIsUnloading;

	ERHIFeatureLevel::Type FeatureLevel;

	FTexture2DRHIRef TempInputTexture;
	//Since we are only reading from the resource, we do not need a UAV;an SRV is sufficient
	FShaderResourceViewRHIRef TempInputTextureSRV;
};
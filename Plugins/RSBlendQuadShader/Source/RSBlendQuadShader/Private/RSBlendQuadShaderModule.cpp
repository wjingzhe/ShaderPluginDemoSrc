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
#define MACRO_TO_STRING(x) MACRO_TO_STRING_HELPER(x)
#define MACRO_TO_STRING_HELPER(x) #x


#include "RSBlendQuadShaderPrivatePCH.h"
#define LOCTEXT_NAMESPACE "FRSBlendQuadShaderModule"
#include "RSBlendQuadShaderModule.h"
#include "Runtime/Renderer/Private/PostProcess/SceneRenderTargets.h"

void FRSBlendQuadShaderModule::StartupModule()
{
	// This code will execute after your module is loaded into memory; the exact timing is specified in the .uplugin file per-module
}

void FRSBlendQuadShaderModule::ShutdownModule()
{
	// This function may be called during shutdown to clean up your module.  For modules that support dynamic reloading,
	// we call this function before unloading the module.
}


//It seems to be the convention to expose all vertex declarations as globals, and then reference them as externs in the headers where they are needed.
//It kind of makes sense since they do not contain any parameters that change and are purely used as their names suggest, as declarations :)
TGlobalResource<FVertexPos4AndUV2> GTextureVertexPosAndUV;

FRSBlendQuadShaderModule::FRSBlendQuadShaderModule(int32 SizeX, int32 SizeY, ERHIFeatureLevel::Type ShaderFeatureLevel)
{
	FeatureLevel = ShaderFeatureLevel;

	bMustRegenerateSRV = false;
	bIsPixelShaderExecuting = false;
	bIsUnloading = false;

	TempInputTexture = nullptr;
	TempInputTextureSRV = nullptr;
}


FRSBlendQuadShaderModule::~FRSBlendQuadShaderModule()
{
	bIsUnloading = true;
	TempInputTexture = nullptr;
	TempInputTextureSRV = nullptr;
}


void FRSBlendQuadShaderModule::ExecutePixelShader(FTexture2DRHIRef NewInputTexture, FShaderResourceViewRHIRef NewInputTextureSRV)
{
	if (bIsUnloading || bIsPixelShaderExecuting) //Skip this execution round if we are already executing
	{
		return;
	}

	bIsPixelShaderExecuting = true;

	this->TempInputTexture = NewInputTexture;
	this->TempInputTextureSRV = NewInputTextureSRV;

	FString temp = MACRO_TO_STRING(INC_DWORD_STAT(STAT_SkelMeshDrawCalls));


	//FPostOpaqueRenderDelegate PostOpaqueRender;
	//PostOpaqueRender.BindLambda(
	//	[this](FPostOpaqueRenderParameters& p)
	//{
	//	this->ExecutePixelShaderInternal();
	//}
	//);
	//GetRendererModule().RegisterPostOpaqueRenderDelegate(PostOpaqueRender);

	//This macro sends the function we declare inside to be run on the render thread.
	// What we do is essentially just send this class and tell the render thread to run 
	// the internal render function as soon as it can.
	//i am still not 100% certain on the thread safety of this ,if you are getting crashes, 
	//depending on how advanced code you have in the start of the ExecutePixelShader function, 
	//you might have to use a lock

	//ENQUEUE_UNIQUE_RENDER_COMMAND_ONEPARAMETER(
	//	FPixelShaderRunner,
	//	FRSBlendQuadShaderModule*, RSBlendQuadShader, this, { RSBlendQuadShader->ExecutePixelShaderInternal(); }
	//);

}

void FRSBlendQuadShaderModule::ExecutePixelShaderInternal()
{
	check(IsInRenderingThread());

	if (bIsUnloading)//if we are about to unload, so just clean up the SRV
	{
		return;
	}


	//拿到当前绘制命令链表
	FRHICommandListImmediate& RHICmdList = GRHICommandList.GetImmediateCommandList();

	//This is where the magic happens
	SetRenderTarget(RHICmdList, FSceneRenderTargets::Get(RHICmdList).GetSceneColorTexture(), FTexture2DRHIRef());
	//CurrentTexture = CurrentRenderTarget->GetRenderTargetResource()->GetRenderTargetTexture();
	//SetRenderTarget(RHICmdList, CurrentTexture, FTexture2DRHIRef());
	RHICmdList.SetBlendState(TStaticBlendState<>::GetRHI());//jingz todo
	RHICmdList.SetRasterizerState(TStaticRasterizerState<>::GetRHI());
	RHICmdList.SetDepthStencilState(TStaticDepthStencilState<false, CF_Always>::GetRHI());

	static FGlobalBoundShaderState BoundShaderState;
	TShaderMapRef<FVertexShaderBlendQuad> VertexShader(GetGlobalShaderMap(FeatureLevel));
	TShaderMapRef<FPixelShaderBlendQuad> PixelShader(GetGlobalShaderMap(FeatureLevel));
	SetGlobalBoundShaderState(RHICmdList, FeatureLevel, BoundShaderState, GTextureVertexPosAndUV.VertexDeclarationRHI, *VertexShader, *PixelShader);

	PixelShader->SetShaderResourceViewRHI(RHICmdList, TempInputTextureSRV, TempInputTexture);

	//Draw a fullscreen quad that we can run our pixel shader on
	FVertexPos4AndUV2::FTextureVertex Vertices[4];
	Vertices[0].Position = FVector4(-1.0f, 1.0f, 0, 1.0f);
	Vertices[1].Position = FVector4(1.0f, 1.0f, 0, 1.0f);
	Vertices[2].Position = FVector4(-1.0f, -1.0f, 0, 1.0f);
	Vertices[3].Position = FVector4(1.0f, -1.0f, 0, 1.0f);
	Vertices[0].UV = FVector2D(0, 0);
	Vertices[1].UV = FVector2D(1, 0);
	Vertices[2].UV = FVector2D(0, 1);
	Vertices[3].UV = FVector2D(1, 1);

	DrawPrimitiveUP(RHICmdList, PT_TriangleStrip, 2, Vertices, sizeof(Vertices[0]));
	PixelShader->UnbindBuffers(RHICmdList);

	bIsPixelShaderExecuting = false;
}

#undef LOCTEXT_NAMESPACE

IMPLEMENT_MODULE(FRSBlendQuadShaderModule, RSBlendQuadShader)
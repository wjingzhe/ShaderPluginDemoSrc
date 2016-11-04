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


#include "GlobalShader.h"
#include "UniformBuffer.h"
#include "RHICommandList.h"

//this is the type we use as vertices from our fullscreen quad
//



class FVertexPos4AndUV2 :public FRenderResource
{
public:
	struct FTextureVertex
	{
		FVector4 Position;
		FVector2D UV;
	};
public:
	FVertexDeclarationRHIRef VertexDeclarationRHI;

	virtual void InitRHI() override
	{
		FVertexDeclarationElementList Elements;
		uint32 Stride = sizeof(FTextureVertex);
		Elements.Add(FVertexElement(0, STRUCT_OFFSET(FTextureVertex, Position), VET_Float4, 0, Stride));
		Elements.Add(FVertexElement(0, STRUCT_OFFSET(FTextureVertex, UV), VET_Float2, 1, Stride));
		VertexDeclarationRHI = RHICreateVertexDeclaration(Elements);
	}


	virtual void ReleaseRHI() override
	{
		//jingz todo something different
		VertexDeclarationRHI->Release();
	}

};


class FVertexShaderBlendQuad :public FGlobalShader
{
	DECLARE_SHADER_TYPE(FVertexShaderBlendQuad, Global);

public:

	static bool ShouldCache(EShaderPlatform)
	{
		return true;
	}

	FVertexShaderBlendQuad(const ShaderMetaType::CompiledShaderInitializerType & Initalizer)
		:FGlobalShader(Initalizer)
	{}

	FVertexShaderBlendQuad() {}

};


//this class is what encapsulate the shader in the engine.
//it is the main bridge between the HLSL located in the engine directory
// and the engine itself
class FPixelShaderBlendQuad :public FGlobalShader
{
	DECLARE_SHADER_TYPE(FPixelShaderBlendQuad, Global);

public:
	FPixelShaderBlendQuad(){}
	explicit FPixelShaderBlendQuad(const ShaderMetaType::CompiledShaderInitializerType& Initializer);
	static bool ShouldCache(EShaderPlatform Platform)
	{
		return IsFeatureLevelSupported(Platform, ERHIFeatureLevel::SM5);
	}

	virtual bool Serialize(FArchive& Ar)override
	{
		bool bShaderHasOutdatedParams = FGlobalShader::Serialize(Ar);
		Ar << TextureParameter;
		Ar << TextureSampler;
		return bShaderHasOutdatedParams;
	}

	void SetShaderResourceViewRHI(FRHICommandList& RHICmdList, FShaderResourceViewRHIRef TextureParameterSRV, FTexture2DRHIRef InputTexture);
	void UnbindBuffers(FRHICommandList& RHICmdList);

private:
	//This is how you declare resource that are going to be made  available in the HLSL
	FShaderResourceParameter TextureParameter;
	FShaderResourceParameter TextureSampler;
};
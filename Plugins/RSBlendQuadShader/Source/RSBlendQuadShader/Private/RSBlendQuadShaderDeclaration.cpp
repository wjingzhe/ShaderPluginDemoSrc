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

#include "RSBlendQuadShaderPrivatePCH.h"
#include "RSBlendQuadShaderDeclaration.h"
#include "ShaderParameterUtils.h"
#include "RHIStaticStates.h"



FPixelShaderBlendQuad::FPixelShaderBlendQuad(const ShaderMetaType::CompiledShaderInitializerType& Initializer)
	:FGlobalShader(Initializer)
{
	//This call is what lets the shader system know that the surface OutputSurface is going to be avaiable in the shader.
	//The second parameter is the name it will be known by in the shader
	TextureParameter.Bind(Initializer.ParameterMap, TEXT("TextureParameter")); //The text parameter here is the name of the parameter in the shader
	TextureSampler.Bind(Initializer.ParameterMap, TEXT("TextureParameterSampler")); //The text parameter here is the name of the parameter in the shader

}

void FPixelShaderBlendQuad::SetShaderResourceViewRHI(FRHICommandList & RHICmdList, FShaderResourceViewRHIRef TextureParameterSRV, FTexture2DRHIRef InputTexture)
{
	FPixelShaderRHIParamRef PixelShaderRHI = GetPixelShader();

	//this actually sets the shader resource view to the texture parameter in the shader
	if (TextureParameter.IsBound())
	{
		SetTextureParameter(RHICmdList, PixelShaderRHI, TextureParameter, TextureSampler, TStaticSamplerState<SF_AnisotropicPoint, AM_Clamp, AM_Clamp, AM_Clamp>::GetRHI(), InputTexture);
		//RHICmdList.SetShaderResourceViewParameter(PixelShaderRHI, TextureParameter.GetBaseIndex(), FShaderResourceViewRHIRef());
	}
}


void FPixelShaderBlendQuad::UnbindBuffers(FRHICommandList & RHICmdList)
{
	FPixelShaderRHIParamRef PixelShaderRHI = GetPixelShader();
	if (TextureParameter.IsBound())
	{
		RHICmdList.SetShaderResourceViewParameter(PixelShaderRHI, TextureParameter.GetBaseIndex(), FShaderResourceViewRHIParamRef());
	}
}


//This os what will instantiate the shader into the engine from the engine/Shaders folder
//                       ShaderType                           ShaderFileName     Shader function entry name            Type
IMPLEMENT_SHADER_TYPE(, FVertexShaderBlendQuad, TEXT("BlendQuadShader"), TEXT("MainVertexShader"), SF_Vertex);
IMPLEMENT_SHADER_TYPE(, FPixelShaderBlendQuad, TEXT("BlendQuadShader"), TEXT("MainPixelShader"), SF_Pixel);

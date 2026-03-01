
struct OBJ_ATTRIBUTES
{
	float3      Kd; // diffuse reflectivity
	float	    d; // dissolve (transparency) 
	float3      Ks; // specular reflectivity
	float       Ns; // specular exponent
	float3      Ka; // ambient reflectivity
	float       sharpness; // local reflection map sharpness
	float3      Tf; // transmission filter
	float       Ni; // optical density (index of refraction)
	float3      Ke; // emissive reflectivity
	float		illum; // illumination model
};

struct SHADER_MODEL_DATA
{
    float4x4 worldMatrix;
    OBJ_ATTRIBUTES material;
};
// This is how you declare and access a Vulkan storage buffer in HLSL
StructuredBuffer<SHADER_MODEL_DATA> SceneData : register(t1, space0);

cbuffer SHADER_SCENE_DATA : register(b0, space0)
{
    float4 sunDirection, sunColor, sunAmbient, camPos;
    float4x4 viewMatrix, projectionMatrix;
};

Texture2D textureDiffuse : register(t0, space1);
SamplerState sampLinear : register(s0, space1);

// an ultra simple hlsl pixel shader
float4 main(float4 pos : SV_POSITION, float3 nrm : NORMAL,
            float3 posW : WORLD, float3 uvw : TEXCOORD, uint index : INDEX) : SV_TARGET
{
    // Sample Texture - Dw
    float4 textureColor = textureDiffuse.Sample(sampLinear, float2(uvw.x, uvw.y));

    // Load lighting & material
    float4 sunDir = sunDirection;
    float4 sunCol = sunColor;
    float4 ambient = sunAmbient;
    float4 diffuse = float4(SceneData[index].material.Kd, 1);
    float4 specular = float4(SceneData[index].material.Ks, 1);

    // If scene ambient is too dark - DW
    if (length(ambient.rgb) < 0.2)
        ambient = float4(0.4, 0.4, 0.4, 1.0);
    
    // If sun is too dim - DW
    if (length(sunCol.rgb) < 0.2)
        sunCol = float4(1.2, 1.2, 1.2, 1.0);

    // If material is black, force white so texture shows - DW
    if (length(diffuse.rgb) < 0.1)
        diffuse = float4(1.0, 1.0, 1.0, 1.0);

    // Calculate Lighting
    float lightRatio = saturate(dot(-sunDir.xyz, normalize(nrm)));
    float3 viewDir = normalize(camPos.xyz - posW);
    float3 bounce = reflect(sunDir.xyz, normalize(nrm));
    float intense = max(pow(saturate(dot(viewDir, bounce)), SceneData[index].material.Ns), 0);
    
    float4 reflected = sunCol * specular * intense;
    float4 direct = lightRatio * sunCol;
    
    // Combine
    float4 finalColor = saturate(direct + ambient) * diffuse * textureColor + reflected;

    // Gamma Correction
    return pow(finalColor, 1.0 / 2.2);
}
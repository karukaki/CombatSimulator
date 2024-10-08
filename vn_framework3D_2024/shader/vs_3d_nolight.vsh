struct vertexIn
{
    float4 pos : POSITION0;
	float4 nor : NORMAL0;
	float4 col : COLOR0;
	float2 tx0 : TEXCOORD0;
};
 
struct vertexOut
{
    float4 pos : SV_POSITION;
    float4 col : COLOR0;
	float2 tx0 : TEXCOORD0;
};
 
cbuffer ConstantBuffer
{
	float4x4	WVP;		//World*View*Proj
	float4x4	World;		//ワールド行列
	float4		LightDir;	//平行光源の(逆)方向
	float4		LightCol;	//平行光源の色
	float4		LightAmb;	//環境光の色
	float4		CameraPos;	//カメラのワールド座標
	float4		Diffuse;	//マテリアルの拡散色
	float4		Ambient;	//マテリアルの環境色
	float4		Specular;	//マテリアルの鏡面反射光
}

vertexOut main(vertexIn IN )
{
	vertexOut OUT;
 
	//頂点座標をビューポート座標に変換
	OUT.pos = mul(IN.pos, WVP);

	//頂点カラー
	OUT.col = IN.col;

	//マテリアルカラー
	OUT.col *= Diffuse;
	OUT.col.xyz += Ambient.xyz;	//持ち上げる色

	//テクスチャ座標
	OUT.tx0 = IN.tx0;

    return OUT;
}
struct vertexIn
{
    float4 pos : POSITION0;
    float4 col : COLOR0;
};
 
struct vertexOut
{
    float4 pos : SV_POSITION;
    float4 col : COLOR0;
};
 
cbuffer ConstantBuffer
{
	float4x4	WVP;		//World*View*Proj
}
 
vertexOut main(vertexIn IN )
{
	vertexOut OUT;
 
	//頂点座標をビューポート座標に変換
	OUT.pos = mul(IN.pos, WVP);

	//頂点カラー
	OUT.col = IN.col;

    return OUT;
}
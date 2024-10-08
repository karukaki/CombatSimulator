//--------------------------------------------------------------//
//	"vs_2d.vsh"													//
//		頂点シェーダ(2Dスクリーン座標)							//
//													2023/12/01	//
//														Ichii	//
//--------------------------------------------------------------//

float4 screen : register( b0 );

//入力用
struct vertexIn
{
	float4 pos : POSITION0;
	float4 col : COLOR0;
	float2 tx0 : TEXCOORD0;
};

//出力用
struct vertexOut
{
	float4 pos : SV_POSITION;
	float4 col : COLOR0;
	float2 tx0 : TEXCOORD0;
};

vertexOut main(vertexIn IN)
{
	vertexOut OUT;

	OUT.pos = float4((IN.pos.xy + screen.zw) * screen.xy, 0.0, 1.0);
	OUT.col = IN.col;
	OUT.tx0 = IN.tx0;

	return OUT;
}

struct VSInput {
    float3 position : POSITION;
    float3 normal : NORMAL;
};

struct PSInput {
    float4 position : SV_POSITION;
    float3 normal : NORMAL;
};

cbuffer cb0 : register(b0) {
    float4x4 model; // 64 bytes
    float4x4 view; // 64 bytes
    float4x4 projection; // 64 bytes

    float4 eye; // 16 bytes
    float4 center; // 16 bytes
    float4 up; // 16 bytes

    uint angle; // 4 bytes
    float3 padding; // 12 bytes
}

// Vertex Shader
PSInput VSMain(VSInput input) {
    PSInput output;
    // Compute the rotation matrix
    float rotation_speed = 0.01f;
    
    float cosTheta = cos(angle * rotation_speed);
    float sinTheta = sin(angle * rotation_speed);
    
    //position
    float4 localPos = float4(input.position, 1.0f);
    float4 worldPos = mul(model, localPos);
    float4 viewPos = mul(view, worldPos);
    float4 clipPos = mul(projection, viewPos);
    
    output.position = clipPos;
    //(projection, mul(view, worldPos));
    output.normal = input.normal;
    return output;
}

// Pixel Shader
float4 PSMain(PSInput input) : SV_TARGET {
    float3 dir_light_color = float3(0.5, 0.5, 0.5);
    float3 dir_light_direction = normalize(float3(0, -1, 0));
    float dir_light_brightness = dot(input.normal, -dir_light_direction);
    float3 dir_light_final_color = dir_light_brightness * dir_light_color;
    
    float3 ambient_color = float3(1.0, 0.41, 0.70);
    float3 final_color = ambient_color + dir_light_final_color;
    
    return float4(final_color.x, final_color.y, final_color.z, 1.0);
}
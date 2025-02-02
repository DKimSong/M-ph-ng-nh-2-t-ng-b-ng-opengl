#version 400
in vec4 vPosition;
in vec4 vColor;
in vec3 vNormal;
out vec4 color;

uniform vec4 AmbientProduct, DiffuseProduct, SpecularProduct;
// AmbientProduct: anh sang moi truong
// DiffuseProduct: anh sang khuech tan
// SpecularProduct: anh sang phan chieu sang choi

uniform vec4 LightPosition;
//vi tri anh sang
uniform float Shininess;
//do bong cua vat


uniform mat4 model;
uniform mat4 projection;
uniform mat4 view;

void main()
{
//Mo hinh chieu sang Blinn - Phong (Phong sua doi)

	// Transform vertex position into eye coordinates
	vec3 pos = (view * model * vPosition).xyz;
	vec3 L = normalize( LightPosition.xyz - pos );
	vec3 E = normalize( -pos );
	vec3 H = normalize(L+E);

	// Transform vertex normal into eye coordinates
	vec3 N = normalize( view * model* vec4(vNormal, 0.0) ).xyz;

	// Compute terms in the illumination equation
	vec4 ambient = AmbientProduct;
	float Kd = max( dot(L, N), 0.0 );
	vec4 diffuse = Kd*DiffuseProduct;
	float Ks = pow( max(dot(N, H), 0.0), Shininess );
	vec4 specular = Ks * SpecularProduct;
	if( dot(L, N) < 0.0 ) specular = vec4(0.0, 0.0, 0.0, 1.0);

	color = ambient + diffuse + specular;

	color.a = 1.0;

    gl_Position = projection * view * model * vPosition/vPosition.w;	
}//
// raylib_pbr (c) Nikolas Wipper 2021

#ifndef RAYLIB_PBR_SRC_SHADERS_H_
#define RAYLIB_PBR_SRC_SHADERS_H_

const char pbr_vs[] = "#version 330 core\n"
                      "in vec3 vertexPosition;\n"
                      "in vec2 vertexTexCoord;\n"
                      "in vec3 vertexNormal;\n"
                      "out vec2 tex_coords;\n"
                      "out vec3 vert_pos;\n"
                      "out vec3 vert_norm;\n"
                      "uniform mat4 matView;\n"
                      "uniform mat4 matProjection;\n"
                      "uniform mat4 matModel;\n"
                      "uniform mat4 mvp;\n"
                      "void main(){\n"
                      "tex_coords=vertexTexCoord;\n"
                      "vert_pos=vec3(matModel*vec4(vertexPosition,1.0));\n"
                      "vert_norm=transpose(inverse(mat3(matModel)))*vertexNormal;\n"
                      "gl_Position=mvp*vec4(vertexPosition,1.0);\n"
                      "}";

const char pbr_fs[] = "#version 330 core\n"
                      "in vec2 tex_coords;\n"
                      "in vec3 vert_pos;\n"
                      "in vec3 vert_norm;\n"
                      "out vec4 FragColor;\n"
                      "uniform sampler2D albedoMap;\n"
                      "uniform sampler2D normalMap;\n"
                      "uniform sampler2D metallicMap;\n"
                      "uniform sampler2D roughnessMap;\n"
                      "uniform sampler2D aoMap;\n"
                      "#define LIGHT_POINT 1\n"
                      "#define LIGHT_SPOT 2\n"
                      "#define LIGHT_SUN 3\n"
                      "struct Light{\n"
                      "vec3 pos;\n"
                      "vec3 color;\n"
                      "vec3 target;\n"
                      "float intensity;\n"
                      "int type;\n"
                      "int on;\n"
                      "};\n"
                      "uniform Light lights[100];\n"
                      "uniform vec3 camPos;\n"
                      "const float PI=3.14159265359;\n"
                      "vec3 GetNormalFromMap(){\n"
                      "vec3 tangentNormal=texture(normalMap,tex_coords).xyz*2.0-1.0;\n"
                      "vec3 Q1=dFdx(vert_pos);\n"
                      "vec3 Q2=dFdy(vert_pos);\n"
                      "vec2 st1=dFdx(tex_coords);\n"
                      "vec2 st2=dFdy(tex_coords);\n"
                      "vec3 N=normalize(vert_norm);\n"
                      "vec3 T=normalize(Q1*st2.t-Q2*st1.t);\n"
                      "vec3 B=-normalize(cross(N,T));\n"
                      "mat3 TBN=mat3(T,B,N);\n"
                      "return normalize(TBN*tangentNormal);\n"
                      "}\n"
                      "float DistributionGGX(vec3 N,vec3 H,float roughness){\n"
                      "float a=roughness*roughness;\n"
                      "float a2=a*a;\n"
                      "float NdotH=max(dot(N,H),0.0);\n"
                      "float NdotH2=NdotH*NdotH;\n"
                      "float nom=a2;\n"
                      "float denom=(NdotH2*(a2-1.0)+1.0);\n"
                      "denom=PI*denom*denom;\n"
                      "return nom/denom;\n"
                      "}\n"
                      "float GeometrySchlickGGX(float NdotV,float roughness){\n"
                      "float r=(roughness+1.0);\n"
                      "float k=(r*r)/8.0;\n"
                      "float nom=NdotV;\n"
                      "float denom=NdotV*(1.0-k)+k;\n"
                      "return nom/denom;\n"
                      "}\n"
                      "float GeometrySmith(vec3 N,vec3 V,vec3 L,float roughness){\n"
                      "float NdotV=max(dot(N,V),0.0);\n"
                      "float NdotL=max(dot(N,L),0.0);\n"
                      "float ggx2=GeometrySchlickGGX(NdotV,roughness);\n"
                      "float ggx1=GeometrySchlickGGX(NdotL,roughness);\n"
                      "return ggx1*ggx2;\n"
                      "}\n"
                      "vec3 FresnelSchlick(float cosTheta,vec3 F0){\n"
                      "return F0+(1.0-F0)*max(1.0-cosTheta,0.0);\n"
                      "}\n"
                      "void main(){\n"
                      "vec3 albedo=pow(texture(albedoMap,tex_coords).rgb,vec3(2.2));\n"
                      "float metallic=texture(metallicMap,tex_coords).r;\n"
                      "float roughness=texture(roughnessMap,tex_coords).r;\n"
                      "float ao=texture(aoMap,tex_coords).r;\n"
                      "vec3 N=GetNormalFromMap();\n"
                      "vec3 V=normalize(camPos-vert_pos);\n"
                      "vec3 F0=vec3(0.04);\n"
                      "F0=mix(F0,albedo,metallic);\n"
                      "vec3 Lo=vec3(0);\n"
                      "for(int i=0;i<100;++i){\n"
                      "if(lights[i].on==0) continue;\n"
                      "vec3 L;\n"
                      "vec3 radiance=lights[i].color.rgb*lights[i].intensity;\n"
                      "if(lights[i].type==LIGHT_POINT){\n"
                      "L=normalize(lights[i].pos-vert_pos);\n"
                      "float distance=length(lights[i].pos-vert_pos);\n"
                      "float attenuation=1.0/(distance*distance);\n"
                      "radiance*=attenuation;\n"
                      "}else if(lights[i].type==LIGHT_SPOT){\n"
                      "L=-normalize(lights[i].target-lights[i].pos);\n"
                      "float distance=length(lights[i].pos-vert_pos);\n"
                      "float attenuation=1.0/(distance*distance);\n"
                      "radiance*=attenuation;\n"
                      "}else if(lights[i].type==LIGHT_SUN){\n"
                      "L=normalize(lights[i].target);\n"
                      "}\n"
                      "vec3 H=normalize(V+L);\n"
                      "float NDF=DistributionGGX(N,H,roughness);\n"
                      "float G=GeometrySmith(N,V,L,roughness);\n"
                      "vec3 F=FresnelSchlick(max(dot(H,V),0.0),F0);\n"
                      "vec3 nominator=NDF*G*F;\n"
                      "float denominator=4*max(dot(N,V),0.0)*max(dot(N,L),0.0)+0.001;\n"
                      "vec3 specular=nominator/denominator;\n"
                      "vec3 kD=vec3(1.0)-F;\n"
                      "kD*=1.0-metallic;\n"
                      "float NdotL=max(dot(N,L),0.0);\n"
                      "Lo+=(kD*albedo/PI+specular)*radiance*NdotL;\n"
                      "}\n"
                      "vec3 surrounding_light=vec3(0.03);\n"
                      "vec3 ambient=surrounding_light*albedo*ao;\n"
                      "vec3 color=ambient+Lo;\n"
                      "color=color/(color+vec3(1.0));\n"
                      "color=pow(color,vec3(1.0/2.2));\n"
                      "FragColor=vec4(color,1.0);\n"
                      "}";

#endif //RAYLIB_PBR_SRC_SHADERS_H_

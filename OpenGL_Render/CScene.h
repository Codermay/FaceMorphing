#ifndef  _SCENE_H_  
#define	_SCENE_H_ 
#include "GL/glew.h"
#include "CModel.h"
struct Scene
{
	int     num_models;
	Model * Models[99];

	void    Add(Model * n)
	{
		Models[num_models++] = n;
	}
	void Render(mat4 view, mat4 proj, vec3 Pos2, mat4 rollPitchYaw)
	{
		for (int i = 0; i < num_models; i++) {
			Models[i]->Render(view, proj, Pos2, rollPitchYaw);
		}
	}

	struct Compiler
	{
		GLuint            program;
		Compiler(GLuint vertex_shader, GLuint pixel_shader)
		{
			GLuint vShader = vertex_shader;
			GLuint fShader = pixel_shader;

			program = glCreateProgram();
			glAttachShader(program, vShader);
			glAttachShader(program, fShader);

			glLinkProgram(program);
			GLint r;
			glGetProgramiv(program, GL_LINK_STATUS, &r);
			if (!r)
			{
				GLchar msg[1024];
				glGetProgramInfoLog(program, sizeof(msg), 0, msg);
				fprintf(stderr, "Error: GL_LINK-%s\n", msg);
			}

			glDetachShader(program, vShader);
			glDetachShader(program, fShader);
		}
	};
	GLuint CreateShader(GLenum type, const GLchar* src)
	{
		GLuint shader = glCreateShader(type);

		glShaderSource(shader, 1, &src, NULL);
		glCompileShader(shader);

		GLint r;
		glGetShaderiv(shader, GL_COMPILE_STATUS, &r);
		if (!r)
		{
			GLchar msg[1024];
			glGetShaderInfoLog(shader, sizeof(msg), 0, msg);
			if (msg[0]) {
				fprintf(stderr, "Error: GL_COMPILE-%s\n", msg);
			}
			return 0;
		}

		return shader;
	}


	Scene() : num_models(0) // Main world
	{

		static const GLchar* VertexShaderSrc =
			"#version 330\n"
			"uniform mat4 mModel;\n"
			"uniform mat4 mView;\n"
			"uniform mat4 mProj;\n"
			"uniform int textureON;"
			"uniform int lightON;"
			"uniform	vec3 lightPosition;"
			"uniform	vec3 eyePosition;"
			"in      vec3 Position;\n"
			"in      vec4 Color;\n"
			"in      vec3 texCoord;\n"
			"in      vec3 Normal;\n"
			"out     vec4 fColor;\n"
			"out	 vec3 L,N,E;\n"
			"out	 vec3 fTexCoord;\n"
			"out	 vec4 gl_Position;"
			"void main()\n"
			"{\n"
			"	vec4 updatePos=mView*mModel*vec4(Position,1.0);\n"
			"   vec4 updateNormal=mView*mModel*vec4(Normal,0.0);\n"
			"   gl_Position = mProj*updatePos;\n"
			"   fColor      = Color;\n"
			"   if(lightON==1){\n"
			"	L=(mView*vec4(lightPosition,1.0)).xyz-updatePos.xyz;\n"
			"	N=updateNormal.xyz;\n"
			"   E=(mView*vec4(eyePosition,1.0)).xyz-updatePos.xyz;\n"
			"	}else{L=vec3(0,0,0),N=vec3(0,0,0),E=vec3(0,0,0);}\n"
			"	if(textureON==1)\n"
			"		fTexCoord = texCoord;\n"
			"	else\n"
			"		fTexCoord = vec3(0,0,0);\n"
			"	 \n"
			"}\n";

		static const char* FragmentShaderSrc =
			"#version 330\n"
			"uniform	sampler2D	tex; \n"
			"uniform	mat4		mView;"
			"uniform	mat4		mModel;"
			"uniform	int			lightON;"
			"uniform	int			textureON;"
			"uniform	vec4		Kambient, Kdiffuse, Kspecular;"
			"uniform	float		Kshininess;"
			"uniform	vec4		Lambient, Ldiffuse, Lspecular;"
			"in			vec4		fColor;\n"
			"in			vec3		L,N,E;\n"
			"in			vec3		fTexCoord;\n"
			"out		vec4		fragColor;\n"
			"void main()\n"
			"{\n"
			"vec4 oColor; \n"
			"if (lightON == 1){ \n"
			"vec3 nL = normalize(L); \n"
			"vec3 nE = normalize(E); \n"
			"vec3 nN = normalize(N); \n"
			"vec3 nH = normalize(nL + nE); \n"
			"float diffuseC = max(dot(nL, nN), 0.0); \n"
			"vec4 diffuse = diffuseC*Kdiffuse*fColor*Ldiffuse; \n"
			"float specularC = pow(max(dot(nN, nH), 0.0), Kshininess); \n"
			"vec4  specular = specularC*Kspecular*Lspecular; \n"
			"vec4  ambient = Kambient*fColor*Lambient; \n"
			"oColor = diffuse + specular + ambient; \n"
			"oColor.a = 1.0; \n"
			"} \n"
			"else \n"
			"oColor = fColor; \n"
			"	if(textureON==0){\n"
			"		fragColor=oColor;}\n"
			"	else\n"
			"		{fragColor=texture(tex, fTexCoord.xy) * oColor;}\n"
			"}\n";

		GLuint    vshader = CreateShader(GL_VERTEX_SHADER, VertexShaderSrc);
		GLuint    fshader = CreateShader(GL_FRAGMENT_SHADER, FragmentShaderSrc);

		// Make textures
		Compiler SceneCompiler=Compiler(vshader, fshader);
		glDeleteShader(vshader);
		glDeleteShader(fshader);
		//setupLights();
		// Construct geometry
	
		Model *bg = new Model();
		bg->Init(SceneCompiler.program);
		Add(bg);
		ObjModel *bg2 = new ObjModel();
		bg2->Init(SceneCompiler.program,"3obj.obj","rentou.png");
		Add((Model*)bg2);
		ObjModel *bg3 = new ObjModel();
		bg3->Init(SceneCompiler.program, "3obj.obj", "t5.png", vec3(0.15, 0, 0));
		Add((Model*)bg3);
		ObjModel *bg4 = new ObjModel();
		bg4->Init(SceneCompiler.program, "3obj.obj", "result_rentou.png", vec3(0.3, 0, 0));
		Add((Model*)bg4);
	}

	~Scene()
	{
		for (int i = 0; i < num_models; i++)
			delete Models[i];
	}


};
#endif
//"void main()\n"
//"{\n"
//"vec4 oClr; \n"
//"if(lightON==1){"
//
//"	mat3 normalMatrix = transpose(inverse(mat3(mModel)));"
//"	vec3 normal = normalize(normalMatrix * fNormal);"
//"	vec3 fragPosition = vec3(mModel * vec4(fPosition, 1));"
//"	vec3 surfaceToLight = lightPosition - fragPosition;"
//"	float brightness = dot(normal, surfaceToLight) / (length(surfaceToLight) * length(normal));"
//"	brightness = clamp(brightness, 0.3, 0.9); "
//"   if(dot(normal, surfaceToLight)<0)"
//"		oClr= vec4(brightness * fColor.rgb,fColor.a);\n"
//"   else    \n   "
//"	oClr= vec4(brightness * fColor.rgb,fColor.a);\n"
//"	}else"
//"		{oClr=fColor;}\n"
//"	if(textureON==0){\n"
//"		fragColor=oClr;}\n"
//"	else\n"
//"		{fragColor=texture(tex, fTexCoord.xy) * oClr;}\n"
//"}\n";
#ifndef  _MODELS_H_  
#define	_MODELS_H_ 
#include <gl/glew.h>
#include <opencv2\opencv.hpp>
#include "CmyGLm.h"
#include <string>
#include "objloader.h"
using namespace glm;
using namespace OLoader;
class Model
{
public:
	struct VertexBuffer
	{
		GLuint    id;
		size_t size;
		VertexBuffer(void* vertices, size_t num_Vertices)
		{
			size = num_Vertices;
			float* fx = (float*)vertices;
			glGenBuffers(1, &id);
			glBindBuffer(GL_ARRAY_BUFFER, id);
			glBufferData(GL_ARRAY_BUFFER, size * 13 * sizeof(GLfloat), vertices, GL_STATIC_DRAW);
		}
		~VertexBuffer()
		{
			glDeleteBuffers(1, &id);
		}
		void Bind()
		{
			glBindBuffer(GL_ARRAY_BUFFER, id);
		}
	};
	//----------------------------------------------------------------
	struct IndexBuffer
	{
		GLuint    id;
		size_t size;
		IndexBuffer(void* indices, size_t num_Indices)
		{
			size = num_Indices;
			glGenBuffers(1, &id);
			glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, id);
			glBufferData(GL_ELEMENT_ARRAY_BUFFER, size*sizeof(GLint), indices, GL_STATIC_DRAW);
		}
		~IndexBuffer()
		{
			glDeleteBuffers(1, &id);
		}
		void Bind()
		{
			glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, id);
		}
	};
	struct Texture
	{
		GLuint id;
		Texture(std::string picPath)
		{
			cv::Mat textureT = cv::imread(picPath);
			if (textureT.rows > 0 && textureT.cols > 0)
			{
				glGenTextures(1, &id);
				glBindTexture(GL_TEXTURE_2D, id);
				float color[] = { 1.0f, 0.0f, 0.0f, 1.0f };
				glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, color);
				glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
				glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);
				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
				//glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);

				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

				//glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
				glGenerateMipmap(GL_TEXTURE_2D);
				cv::Mat TextureMat;
				TextureMat.create(textureT.size(), CV_8UC4);
				if (textureT.channels() == 4)
					cv::cvtColor(textureT, TextureMat, CV_BGRA2RGBA, 4);
				if (textureT.channels() == 3)
					cv::cvtColor(textureT, TextureMat, CV_BGR2RGBA, 4);
				if (textureT.channels() == 1)
					cv::cvtColor(textureT, TextureMat, CV_GRAY2RGBA, 4);
				cv::flip(TextureMat, TextureMat, 0);
				glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, TextureMat.cols, TextureMat.rows, 0, GL_RGBA, GL_UNSIGNED_BYTE, TextureMat.data);
			}
			else
				id = -1;
		}
		~Texture()
		{
			if (id != -1)
				glDeleteTextures(1, &id);
		}
		void Bind()
		{
			if (id != -1)
			{
				glActiveTexture(GL_TEXTURE0);
				glBindTexture(GL_TEXTURE_2D, id);
			}

		}
	};
	struct VertexArray
	{
		GLuint    vao;
		GLuint program;
		VertexBuffer *vbo;
		IndexBuffer *ebo;
		Texture *tex;
		bool texON;
		VertexArray(GLuint _program)
		{
			program = _program;
			glGenVertexArrays(1, &vao);
			glBindVertexArray(vao);
		}
		~VertexArray()
		{
			glDeleteVertexArrays(1, &vao);
		}
		void update(VertexBuffer *_vbo, IndexBuffer *_ebo, Texture *_tex)
		{
			vbo = _vbo;
			ebo = _ebo;
			tex = _tex;
			texON = (tex->id == -1) ? false : true;
			GLuint posLoc = glGetAttribLocation(program, "Position");
			GLuint colorLoc = glGetAttribLocation(program, "Color");
			GLuint normalLoc = glGetAttribLocation(program, "Normal");
			GLuint texCoordlLoc = glGetAttribLocation(program, "texCoord");
			glBindVertexArray(vao);
			glVertexAttribPointer(posLoc, 3, GL_FLOAT, GL_FALSE, 13 * sizeof(GLfloat), NULL);
			glEnableVertexAttribArray(posLoc);
			glVertexAttribPointer(colorLoc, 4, GL_FLOAT, GL_FALSE, 13 * sizeof(GLfloat), (GLvoid*)(3 * sizeof(GLfloat)));
			glEnableVertexAttribArray(colorLoc);
			glVertexAttribPointer(texCoordlLoc, 3, GL_FLOAT, GL_FALSE, 13 * sizeof(GLfloat), (GLvoid*)(7 * sizeof(GLfloat)));
			glEnableVertexAttribArray(texCoordlLoc);
			glVertexAttribPointer(normalLoc, 3, GL_FLOAT, GL_FALSE, 13 * sizeof(GLfloat), (GLvoid*)(10 * sizeof(GLfloat)));
			glEnableVertexAttribArray(normalLoc);
		}
		void Bind()
		{
			glBindVertexArray(vao);
		}
		void BindAll()
		{
			glBindVertexArray(vao);
			ebo->Bind();
			vbo->Bind();
			tex->Bind();
		}
	};

	struct Vertex
	{
		vec3  Pos;
		vec4  Color;
		vec3  TexCoord;
		vec3  Normal;
	};
	struct Light {
		GLuint ON;
		vec3 position;
		vec4 ambient;
		vec4 diffuse;
		vec4 specular;
	}light;
	struct Material{
		vec4 ambient;
		vec4 diffuse;
		vec4 specular;
		GLfloat shininess;
	}material;
	struct Object{
		vec3     Pos;
		vec3     Scl;
		vec3     Rot;
		mat4     ModelT;
	}object;
	struct Camera{
		vec3 eyePosition;
		mat4 eyeRollPitchYaw;
		mat4 view;
		mat4 proj;
	}camera;
	VertexArray* VA;
	Texture* texture;
	GLenum drawType;
	GLuint program;
	Model() :
		drawType(GL_TRIANGLES)
	{
		object.Scl = vec3(1.0, 1.0, 1.0);

		light.ON = 1;
		light.position = vec3(0.0, 30.0, 30.0);
		light.ambient = vec4(0.9,0.9,0.9,1.0);
		light.diffuse = vec4(0.9, 0.9, 0.9, 1.0);
		light.specular = vec4(0.9, 0.9, 0.9, 1.0);
		material.ambient = vec4(0.7, 0.7, 0.7, 1.0);
		material.diffuse = vec4(0.3, 0.3, 0.3, 1.0);
		material.specular = vec4(0.1, 0.1, 0.1, 1.0);
		material.shininess = 0.2;
	}

	virtual ~Model()   {
		delete VA;
	}

	void  Init(GLuint _program) {
		program = _program;
		VA = new VertexArray(_program);
		texture = createTexture("");
	};
	virtual VertexBuffer createVBO()
	{
		static GLfloat _vertices[] = {
			0.5f, 0.5f, -1.0f,
			1.0f, 0.0f, 0.0f, 1.0,
			1.0f, 1.0f, 1.0f,
			0.0f, 0.0f, 1.0f,

			0.5f, 0.0f, -1.0f,
			0.0f, 1.0f, 0.0f, 1.0,
			1.0f, 0.0f, 1.0f,
			0.0f, 0.0f, 1.0f,

			0.0f, 0.0f, -1.0f,
			0.0f, 0.0f, 1.0f, 1.0,
			0.0f, 0.0f, 1.0f,
			0.0f, 0.0f, 1.0f,

			0.0f, 0.5f, -1.0f,
			1.0f, 1.0f, 1.0f, 1.0,
			0.0f, 1.0f, 1.0f,
			0.0f, 0.0f, 1.0f,
		};
		return VertexBuffer(_vertices, 8);
	}
	virtual IndexBuffer createEBO()
	{
		static GLuint _index[] = {
			0, 1, 3,
			1, 2, 3,
		};
		return IndexBuffer(_index, 6);
	}
	virtual Texture* createTexture(string filename)
	{
		Texture* t = new Texture(filename);
		return t;
	}

	virtual void updateMatrix(mat4 view, mat4 proj, vec3 eyePos, mat4 RollPitchYaw)
	{
		camera.eyePosition = eyePos;
		camera.eyeRollPitchYaw = RollPitchYaw;
		camera.view = view;
		camera.proj = proj;
	};
	virtual mat4& GetTMatrix()
	{
		object.ModelT = Scaling(object.Scl);
		object.ModelT = RotationX(object.Rot.v[0])*RotationY(object.Rot.v[1])*RotationZ(object.Rot.v[2])*object.ModelT;
		object.ModelT = Translation(object.Pos) * object.ModelT;
		return object.ModelT;
	}
	virtual void Render(mat4 view, mat4 proj, vec3 eyePos, mat4 RollPitchYaw)
	{
		updateMatrix(view, proj, eyePos, RollPitchYaw);

		mat4 matModel = GetTMatrix();
		glUseProgram(program);
		//glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
		glPointSize(10);
		GLint TextureON = (VA->texON) ? 1 : 0;
		VertexBuffer VB = createVBO();
		IndexBuffer EB = createEBO();
		VA->update(&VB, &EB, texture);

		//"uniform mat4 mModel;\n"
		//	"uniform mat4 mView;\n"
		//	"uniform mat4 mProj;\n"
		//	"uniform int textureON;"
		//	"uniform int lightON;"
		//  "uniform	vec4 lightPosition;"
		//	"uniform	vec4 eyePosition;"
		//	"uniform	vec4		Kambient, Kdiffuse, Kspecular;"
		//	"uniform	float		Kshininess;"
		//	"uniform	vec4		Lambient, Ldiffuse, Lspecular;"
		glUniformMatrix4fv(glGetUniformLocation(program, "mModel"), 1, GL_FALSE, matModel.m);
		glUniformMatrix4fv(glGetUniformLocation(program, "mView"), 1, GL_FALSE, view.m);
		glUniformMatrix4fv(glGetUniformLocation(program, "mProj"), 1, GL_FALSE, proj.m);
		glUniform1i(glGetUniformLocation(program, "textureON"), TextureON);
		glUniform1i(glGetUniformLocation(program, "lightON"), light.ON);
		glUniform3fv(glGetUniformLocation(program, "lightPosition"), 1, light.position.v);
		glUniform3fv(glGetUniformLocation(program, "eyePosition"), 1, camera.eyePosition.v);
		glUniform4fv(glGetUniformLocation(program, "Lambient"), 1, light.ambient.v);
		glUniform4fv(glGetUniformLocation(program, "Ldiffuse"), 1, light.diffuse.v);
		glUniform4fv(glGetUniformLocation(program, "Lspecular"), 1, light.specular.v);
		glUniform4fv(glGetUniformLocation(program, "Kambient"), 1, material.ambient.v);
		glUniform4fv(glGetUniformLocation(program, "Kdiffuse"), 1, material.diffuse.v);
		glUniform4fv(glGetUniformLocation(program, "Kspecular"), 1, material.specular.v);
		glUniform1f(glGetUniformLocation(program, "Kshininess"), material.shininess);

		VA->BindAll();
		glDrawElements(drawType, VA->ebo->size, GL_UNSIGNED_INT, 0);
		//glDrawArrays(drawType, 0, 3);
		glUseProgram(0);
	}
};

class ObjModel :public Model
{
public:
	TriangleMesh mesh;
	vector<Vertex> vs;
	vector<int> is;
	void  Init(GLuint _program, string objfile, string texturefile,vec3 _pos=vec3(0,0,0))
	{
		program = _program;
		VA = new VertexArray(_program);
		texture = createTexture(texturefile);
		object.Scl = vec3(0.05, 0.05, 0.05);
		object.Pos = _pos;
		loadObj(objfile, mesh, false);
		if (mesh.verts.size() > 0)
		{
			for (int i = 0; i < mesh.verts.size(); i++)
			{
				Vertex vx;
				vx.Pos = mesh.verts[i];
				vx.Color = vec4(1.0, 1.0, 1.0, 1.0);
				vs.push_back(vx);
			}
			for (int f = 0; f < mesh.faces.size(); f++)
			{
				int a, b, c;
				a = mesh.faces[f].v[0] - 1;
				b = mesh.faces[f].v[1] - 1;
				c = mesh.faces[f].v[2] - 1;
				is.push_back(a);
				is.push_back(b);
				is.push_back(c);
				vs[a].TexCoord = mesh.vtextures[mesh.faces[f].t[0] - 1];
				vs[b].TexCoord = mesh.vtextures[mesh.faces[f].t[1] - 1];
				vs[c].TexCoord = mesh.vtextures[mesh.faces[f].t[2] - 1];
				vec3 va, vb, vc;
				va = vs[a].Pos;
				vb = vs[b].Pos;
				vc = vs[c].Pos;
				vec3 v1, v2, nor;
				v1 = vb - va;
				v2 = vb - vc;
				nor = cross(v2, v1);
				normalise(nor);
				vs[a].Normal = nor;
				vs[b].Normal = nor;
				vs[c].Normal = nor;
			}

		}

	}
	VertexBuffer createVBO()
	{
		return VertexBuffer(vs.data(), vs.size());
	}
	IndexBuffer createEBO()
	{
		return IndexBuffer(is.data(), is.size());
	}
	Texture* createTexture(string filename)
	{
		Texture* t = new Texture(filename);
		return t;
	}


};

#endif
#ifndef MESH_LOADER_H
#define MESH_LOADER_H

#include <SOIL/SOIL.h>
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>
#include <glm/glm.hpp>
#include <vector>
#include <iostream>
#include <GL/glew.h>
#include <GLFW/glfw3.h>

#define ASSIMP_LOAD_FLAGS (aiProcess_Triangulate | aiProcess_GenSmoothNormals | aiProcess_FlipUVs | aiProcess_JoinIdenticalVertices)

// warp normal, position, texture coordinate
struct Vertex
{
	glm::vec3 m_pos;
	glm::vec3 m_normal;
	glm::vec2 m_texc;

	Vertex(const glm::vec3& pos, const glm::vec3& normal)
	{
		m_pos = pos;
		m_normal = normal;
	}

	Vertex(const glm::vec3& pos, const glm::vec3& normal, const glm::vec2& texc)
	{
		m_pos = pos;
		m_normal = normal;
		m_texc = texc;
	}
};

// warp material texture info
struct TextureEntry
{
	GLuint m_TID;
	std::string m_filename;
	std::string m_name;	// diffuse, specular, bump or ambient etc

	TextureEntry() {}
	TextureEntry(GLuint tid, const std::string& filename, const std::string& name)
	{
		m_TID = tid;
		m_filename = filename;
		m_name= name;
	}
};

class Mesh
{
public:
		Mesh() {}
		~Mesh();

		void LoadMesh(const std::string& file);
		void InitMesh(unsigned int idx, const aiMesh *paiMesh);
		void RenderMesh();

		enum {
			Material_Diffuse,
			Material_Specular,
			Material_Ambient,
			Material_Max
		};

		struct MeshEntry {
			void Init(const std::vector<Vertex>& Vertices,
					const std::vector<unsigned int>& Indices);
			GLuint m_VB;
			GLuint m_IB;
			GLuint m_VA;
			unsigned int m_NumIndices;
			TextureEntry m_TXE[Material_Max];	// store per mesh entry texture
		};

		std::vector<MeshEntry> m_Entries;
		std::vector<TextureEntry> m_TextureEntries;	// store non duplicate texture objects
};

void Mesh::LoadMesh(const std::string& file)
{
	Assimp::Importer Importer;

	const aiScene* pScene = Importer.ReadFile(file.c_str(), ASSIMP_LOAD_FLAGS);

	assert(pScene != NULL);

	std::cout << "======== " << pScene->mNumMeshes << " MeshEntries" << std::endl;

	m_Entries.resize(pScene->mNumMeshes);

	for (int i = 0; i < pScene->mNumMeshes; i++) {
		const aiMesh *paiMesh = pScene->mMeshes[i];
		InitMesh(i, paiMesh);
	}

	for (int i = 0; i < pScene->mNumMeshes; i++) {
		// per mesh entry init materials, assume one on one map per entry and per property
		const aiMesh *paiMesh = pScene->mMeshes[i];
		aiMaterial *paiMaterial = pScene->mMaterials[paiMesh->mMaterialIndex];

		assert(paiMaterial->GetTextureCount(aiTextureType_DIFFUSE) == 1);

		aiString filename;
		paiMaterial->GetTexture(aiTextureType_DIFFUSE, 0, &filename);

		// search if it duplicate
		bool duplicate = false;

		for (int j = 0; j < m_TextureEntries.size(); j++) {
			if (std::strcmp(m_TextureEntries[j].m_filename.c_str(), filename.C_Str())) {
				duplicate = true;
				break;
			}
		}

		if (!duplicate) {
			// first time create, filename contains not full directory information
			std::string path = std::string(filename.C_Str());
			path = "objs/" + path;
			std::cout << "texture full path: " << path << std::endl;

			GLuint textureID;
			glGenTextures(1, &textureID);

			int w, h, c;
			unsigned char *img = SOIL_load_image(path.c_str(), &w, &h, &c, SOIL_LOAD_RGBA);

			glBindTexture(GL_TEXTURE_2D, textureID);
			glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, w, h, 0, GL_RGBA, GL_UNSIGNED_BYTE, img);
			SOIL_free_image_data(img);

			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

			TextureEntry tex(textureID, std::string(filename.C_Str()), std::string());

			// update
			this->m_Entries[i].m_TXE[Material_Diffuse] = tex;
			this->m_TextureEntries.push_back(tex);
		}
	}
}

void Mesh::InitMesh(unsigned int idx, const aiMesh *paiMesh) {

	std::vector<Vertex> Vertices; // constain pos, norm, texcoord etc

	std::vector<unsigned int> Indices;

	assert(paiMesh->HasNormals());
	assert(paiMesh->HasTextureCoords(0));

	for (int i = 0; i < paiMesh->mNumVertices; i++) {
		const aiVector3D *pPos = &(paiMesh->mVertices[i]);
		const aiVector3D *pNormal = &(paiMesh->mNormals[i]);
		const aiVector3D *pTexc = &(paiMesh->mTextureCoords[0][i]);

		Vertex v(glm::vec3(pPos->x, pPos->y, pPos->z),
				glm::vec3(pNormal->x, pNormal->y, pNormal->z),
				glm::vec2(pTexc->x, pTexc->y));

		Vertices.push_back(v);
	}

	for (int i = 0; i < paiMesh->mNumFaces; i++) {
		const aiFace& Face = paiMesh->mFaces[i];
		assert(Face.mNumIndices == 3);
		Indices.push_back(Face.mIndices[0]);
		Indices.push_back(Face.mIndices[1]);
		Indices.push_back(Face.mIndices[2]);
	}

	std::cout << "======== " << paiMesh->mNumVertices << " Vertices at MeshEntry("
		<< idx << ")" << std::endl;

	std::cout << "======== " << paiMesh->mNumFaces << " Faces at MeshEntry("
		<< idx << ")" << std::endl;

	m_Entries[idx].Init(Vertices, Indices);
}

void Mesh::RenderMesh()
{
	for (int i = 0; i < m_Entries.size(); i++) {
		glBindVertexArray(m_Entries[i].m_VA);
		glBindBuffer(GL_ARRAY_BUFFER, m_Entries[i].m_VB);
		// attribute 0 store position
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (GLvoid *)0);
		glEnableVertexAttribArray(0);

		// attribute 1 store normal
		glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (GLvoid *)12);
		glEnableVertexAttribArray(1);

		// attribute 2 store texc
		glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (GLvoid *)24);
		glEnableVertexAttribArray(2);

		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_Entries[i].m_IB);

		// texture unit 0 store diffuse
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, m_Entries[i].m_TXE[Material_Diffuse].m_TID);

		// uniform diffuse texture outside
		glDrawElements(GL_TRIANGLES, m_Entries[i].m_NumIndices, GL_UNSIGNED_INT, 0);
		glBindVertexArray(0);
	}
}

void Mesh::MeshEntry::Init(const std::vector<Vertex>& Vertices,
		const std::vector<unsigned int>& Indices)
{
	glGenVertexArrays(1, &m_VA);
	glBindVertexArray(m_VA);
	glGenBuffers(1, &m_VB);
	glBindBuffer(GL_ARRAY_BUFFER, m_VB);
	glBufferData(GL_ARRAY_BUFFER, sizeof(Vertex) * Vertices.size(), &Vertices[0], GL_STATIC_DRAW);

	glGenBuffers(1, &m_IB);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_IB);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(unsigned int) * Indices.size(), &Indices[0], GL_STATIC_DRAW);
	glBindVertexArray(0);
	this->m_NumIndices = Indices.size();
}
#endif

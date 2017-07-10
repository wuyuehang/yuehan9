#ifndef MESH_LOADER_H
#define MESH_LOADER_H

#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>
#include <glm/glm.hpp>
#include <vector>
#include <iostream>

#define ASSIMP_LOAD_FLAGS (aiProcess_Triangulate | aiProcess_GenSmoothNormals | aiProcess_FlipUVs | aiProcess_JoinIdenticalVertices)

// warp normal, position, texture coordinate
struct Vertex
{
	glm::vec3 m_pos;
	glm::vec3 m_normal;

	Vertex(const glm::vec3& pos, const glm::vec3& normal)
	{
		m_pos = pos;
		m_normal = normal;
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

		struct MeshEntry {
			void Init(const std::vector<Vertex>& Vertices,
					const std::vector<unsigned int>& Indices);
			GLuint m_VB;
			GLuint m_IB;
			GLuint m_VA;
			unsigned int m_NumIndices;
		};

		std::vector<MeshEntry> m_Entries;
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
}

void Mesh::InitMesh(unsigned int idx, const aiMesh *paiMesh) {

	std::vector<Vertex> Vertices; // constain pos, norm, texcoord etc

	std::vector<unsigned int> Indices;

	assert(paiMesh->HasNormals());
	assert(paiMesh->HasTextureCoords(0));

	for (int i = 0; i < paiMesh->mNumVertices; i++) {
		const aiVector3D *pPos = &(paiMesh->mVertices[i]);
		const aiVector3D *pNormal = &(paiMesh->mNormals[i]);

		Vertex v(glm::vec3(pPos->x, pPos->y, pPos->z),
				glm::vec3(pNormal->x, pNormal->y, pNormal->z));

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
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_Entries[i].m_IB);

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

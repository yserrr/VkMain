#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>
#include <mesh.hpp>
#include <stdexcept>
#include <vertex.hpp>

#ifndef IMPORTER_HPP
#define IMPORTER_HPP 

class Importer {
public:
    Importer(){}
    //mesh들고옴
    Mesh loadModel(const char* filepath, MemoryAllocator &allocator) 
    {
        Assimp::Importer importer;
        const aiScene* scene = importer.ReadFile(filepath,
            aiProcess_Triangulate |
            aiProcess_FlipUVs |
            aiProcess_CalcTangentSpace |
            aiProcess_GenNormals);

        if (!scene || !scene->HasMeshes()) {
            spdlog::error("Failed to load model or no meshes found: {}", importer.GetErrorString());
            throw std::runtime_error("Failed to load model or no meshes found.");
        }

        std::vector<Vertex> vertices;
        std::vector<uint32_t> indices;

        for (unsigned int m = 0; m < scene->mNumMeshes; ++m) {
            aiMesh* mesh = scene->mMeshes[m];

            for (unsigned int i = 0; i < mesh->mNumVertices; ++i) {
                Vertex vertex{};

                // position
                vertex.position[0] = mesh->mVertices[i].x;
                vertex.position[1] = mesh->mVertices[i].y;
                vertex.position[2] = mesh->mVertices[i].z;

                // normal
                if (mesh->HasNormals()) {
                    vertex.normal[0] = mesh->mNormals[i].x;
                    vertex.normal[1] = mesh->mNormals[i].y;
                    vertex.normal[2] = mesh->mNormals[i].z;
                } else {
                    vertex.normal[0] = vertex.normal[1] = vertex.normal[2] = 0.0f;
                }

                // texCoord (first UV channel only)
                if (mesh->HasTextureCoords(0)) {
                    vertex.uv[0] = mesh->mTextureCoords[0][i].x;
                    vertex.uv[1] = mesh->mTextureCoords[0][i].y;
                } else {
                    vertex.uv[0] = vertex.uv[1] = 0.0f;
                }

                // tangent
                if (mesh->HasTangentsAndBitangents()) {
                    vertex.tangent[0] = mesh->mTangents[i].x;
                    vertex.tangent[1] = mesh->mTangents[i].y;
                    vertex.tangent[2] = mesh->mTangents[i].z;

                    vertex.bitangent[0] = mesh->mBitangents[i].x;
                    vertex.bitangent[1] = mesh->mBitangents[i].y;
                    vertex.bitangent[2] = mesh->mBitangents[i].z;
                } else {
                    vertex.tangent[0] = vertex.tangent[1] = vertex.tangent[2] = 0.0f;
                    vertex.bitangent[0] = vertex.bitangent[1] = vertex.bitangent[2] = 0.0f;
                }

                // color (only first color set)
                if (mesh->HasVertexColors(0)) {
                    vertex.color[0] = mesh->mColors[0][i].r;
                    vertex.color[1] = mesh->mColors[0][i].g;
                    vertex.color[2] = mesh->mColors[0][i].b;
                    vertex.color[3] = mesh->mColors[0][i].a;
                } else {
                    vertex.color[0] = vertex.color[1] = vertex.color[2] = 1.0f; // 기본 흰색
                    vertex.color[3] = 1.0f;
                }

                // boneIndices and boneWeights 초기화 (0으로)
                for (int j = 0; j < 4; ++j) {
                    vertex.boneIndices[j] = 0;
                    vertex.boneWeights[j] = 0.0f;
                }
                // 본 정보는 aiBone에서 따로 채워야 함. (여기선 생략)
                vertices.push_back(vertex);
            }
            // 인덱스
            for (unsigned int f = 0; f < mesh->mNumFaces; ++f) {
                aiFace face = mesh->mFaces[f];
                for (unsigned int idx = 0; idx < face.mNumIndices; ++idx) {
                    indices.push_back(face.mIndices[idx]);
                }
            }
        }
        spdlog::info("모델 로드 성공");
        return Mesh(vertices, indices, allocator);
    }
};

#endif

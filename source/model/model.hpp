#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <memory>
#include <mesh.hpp>
#include <material.hpp>

class Model{
public:
  Model(std::shared_ptr<Mesh> mesh, std::shared_ptr<Material> material)
    : mesh(mesh), material(material), modelMatrix(1.0f) {}

  void setPosition(const glm::vec3 &pos)
  {
    modelMatrix = glm::translate(glm::mat4(1.0f), pos);
  }

  void setRotation(float angleRadians, const glm::vec3 &axis)
  {
    modelMatrix = glm::rotate(glm::mat4(1.0f), angleRadians, axis);
  }

  void setScale(const glm::vec3 &scale)
  {
    modelMatrix = glm::scale(glm::mat4(1.0f), scale);
  }

  void setModelMatrix(const glm::mat4 &matrix)
  {
    modelMatrix = matrix;
  }

  const glm::mat4 &getModelMatrix() const
  {
    return modelMatrix;
  }

  std::shared_ptr<Mesh> getMesh() const
  {
    return mesh;
  }

  std::shared_ptr<Material> getMaterial() const
  {
    return material;
  }

private:
  std::shared_ptr<Mesh>     mesh;
  std::shared_ptr<Material> material;
  glm::mat4                 modelMatrix;
};
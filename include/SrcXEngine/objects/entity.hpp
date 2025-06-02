#include <iostream>
#include <SrcXEngine/model.hpp>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <list>
#include <memory>

struct Transform {
    glm::vec3 position = { 0.0f, 0.0f, 0.0f };
    glm::vec3 eulerAngleRot = { 0.0f, 0.0f, 0.0f };
    glm::vec3 scale = { 1.0f, 1.0f, 1.0f };

    glm::mat4 model = glm::mat4(1.0f);
};

class Transform {
protected:
    glm::vec3 pos = { 0.0f, 0.0f, 0.0f };
    glm::vec3 eulerRot = { 0.0f, 0.0f, 0.0f };
    glm::vec3 scale = { 1.0f, 1.0f, 1.0f };

    glm::mat4 model = glm::mat4(1.0f);

    bool isDirty = true;

protected:
    glm::mat4 getLocalModelMatrix() {
        const glm::mat4 transformX = glm::rotate(glm::mat4(1.0f), glm::radians(eulerRot.x));
        const glm::mat4 transformY = glm::rotate(glm::mat4(1.0f), glm::radians(eulerRot.y));
        const glm::mat4 transformZ = glm::rotate(glm::mat4(1.0f), glm::radians(eulerRot.z));

        const glm::mat4 rotationMatrix = transformY * transformX * transformZ;

        return glm::translate(glm::mat4(1.0f), pos) * rotationMatrix * glm::scale(glm::mat4(1.0f), scale);
    }
public:

    void computeModel() {
        model = getLocalModelMatrix();
        isDirty = false;
    }

    void computeModel(const glm::mat4& parentGlobalModel) {
        model = parentGlobalModel * getLocalModelMatrix();
        isDirty = false;
    }

    void setLocalPosition(const glm::vec3& newPosition) {
        pos = newPosition;
        isDirty = true;
    }

    const glm::vec3& getLocalPosition() {
        return pos;
    }

    const glm::mat4& getModelMatrix() {
        return model;
    }

    bool isDirty() {
        return isDirty;
    }
};

class Entity : public Model {
public:

    Transform transform;

    std::list<std::unique_ptr<Entity>> children;
        Entity* parent = nullptr;

    Entity(string const& path, bool gamma = false) : Model(path, gamma) {}

    template<typename... TArgs>
    void addChild(const TArgs&... args) {
        children.emplace_back(std::make_unique<Entity>(args...));
        children.back()->parent = this;
    }

    void updateSelfAndChild() {
        if (parent)
            transform.model = parent->transform.model * transform.getLocalModelMatrix();
        else
            transform.model = transform.getLocalModelMatrix();

        for (auto&& child : children) {
            child->updateSelfAndChild();
        }
    }
};
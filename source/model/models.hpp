#include <vector>
#include <memory>
#include <model.hpp>

class Models {
public:
Models() = default;

void addObject(std::shared_ptr<Model> Object) {
     objects.push_back(std::move(Object));
}
void removeObject(size_t index) {
    if (index < objects.size()) {
        objects.erase(objects.begin() + index);
    }
}

//std::shared_ptr<Object> getObject(size_t index) const {
//    if (index < object.size()) {
//        return object[index];
//    }
//    return nullptr;
//}
size_t size() const {
    return objects.size();
}
const std::vector<std::shared_ptr<Model>>& getAll() const {
    return objects;
}

private:
    std::vector<std::shared_ptr<Model>> objects;
};

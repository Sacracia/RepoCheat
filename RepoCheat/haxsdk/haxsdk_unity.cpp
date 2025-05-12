#include "haxsdk_unity.h"

#include <cmath>

namespace Unity {
    static const char* MODULE = "UnityEngine";
    static const char* NAMESPACE = "UnityEngine";

    Class* AsyncOperation::GetClass() {
        static Class* pClass = Class::Find(MODULE, NAMESPACE, "AsyncOperation");
        return pClass;
    }

    bool AsyncOperation::GetIsDone() {
        static HaxMethod<bool(*)(AsyncOperation*)> method(AsyncOperation::GetClass()->FindMethod("get_isDone"));
        return HaxSdk::GetGlobals().backend & HaxBackend_Mono ? *(bool*)method.Invoke(this, nullptr)->Unbox() : method.ptr(this);
    }

    float AsyncOperation::GetProgress() {
        static HaxMethod<float(*)(AsyncOperation*)> method(AsyncOperation::GetClass()->FindMethod("get_progress"));
        return HaxSdk::GetGlobals().backend & HaxBackend_Mono ? *(float*)method.Invoke(this, nullptr)->Unbox() : method.ptr(this);
    }

    Class* Behaviour::GetClass() {
        static Class* pClass = Class::Find(MODULE, NAMESPACE, "Behaviour");
        return pClass;
    }

    bool Behaviour::GetEnabled() {
        static HaxMethod<bool(*)(Behaviour*)> method(Behaviour::GetClass()->FindMethod("get_enabled"));
        return HaxSdk::GetGlobals().backend & HaxBackend_Mono ? *(bool*)method.Invoke(this, nullptr)->Unbox() : method.ptr(this);
    }

    bool Behaviour::GetIsActiveAndEnabled() {
        static HaxMethod<bool(*)(Behaviour*)> method(Behaviour::GetClass()->FindMethod("get_isActiveAndEnabled"));
        return HaxSdk::GetGlobals().backend & HaxBackend_Mono ? *(bool*)method.Invoke(this, nullptr)->Unbox() : method.ptr(this);
    }

    void Behaviour::SetEnabled(bool value) {
        static HaxMethod<void(__fastcall*)(Behaviour*,bool)> method(Behaviour::GetClass()->FindMethod("set_enabled"));
        if (HaxSdk::GetGlobals().backend & HaxBackend_Mono) {
            void* args[] = { &value };
            method.Invoke(this, args);
            return;
        }

        method.ptr(this, value);
    }

    Class* BoxCollider::GetClass() {
        static Class* pClass = Class::Find(MODULE, NAMESPACE, "BoxCollider");
        return pClass;
    }

    Vector3 BoxCollider::InternalGetCenter() {
        static HaxMethod<void(*)(BoxCollider*,Vector3*)> method(BoxCollider::GetClass()->FindMethod("INTERNAL_get_center"));
        Vector3 result;
        method.ptr(this, &result);
        return result;
    }

    Vector3 BoxCollider::InternalGetSize() {
        static HaxMethod<void(*)(BoxCollider*,Vector3*)> method(BoxCollider::GetClass()->FindMethod("INTERNAL_get_size"));
        Vector3 result;
        method.ptr(this, &result);
        return result;
    }

    void BoxCollider::InternalSetCenter(Vector3 value) {
        static HaxMethod<void(*)(BoxCollider*,Vector3*)> method(BoxCollider::GetClass()->FindMethod("INTERNAL_set_center"));
        method.ptr(this, &value);
    }

    void BoxCollider::InternalSetSize(Vector3 value) {
        static HaxMethod<void(*)(BoxCollider*,Vector3*)> method(BoxCollider::GetClass()->FindMethod("INTERNAL_set_size"));
        method.ptr(this, &value);
    }

    Vector3 BoxCollider::GetCenter() {
        static HaxMethod<Vector3(*)(BoxCollider*)> method(BoxCollider::GetClass()->FindMethod("get_center"));
        return HaxSdk::GetGlobals().backend & HaxBackend_Mono ? *(Vector3*)method.Invoke(this, nullptr)->Unbox() : method.ptr(this);
    }

    Vector3 BoxCollider::GetSize() {
        static HaxMethod<Vector3(*)(BoxCollider*)> method(BoxCollider::GetClass()->FindMethod("get_size"));
        return HaxSdk::GetGlobals().backend & HaxBackend_Mono ? *(Vector3*)method.Invoke(this, nullptr)->Unbox() : method.ptr(this);
    }

    Class* Camera::GetClass() {
        static Class* pClass = Class::Find(MODULE, NAMESPACE, "Camera");
        return pClass;
    }

    Camera* Camera::GetMain() {
        static HaxMethod<Camera*(*)()> method(Camera::GetClass()->FindMethod("get_main"));
        return HaxSdk::GetGlobals().backend & HaxBackend_Mono ? (Camera*)method.Invoke(nullptr, nullptr) : method.ptr();
    }

    Vector3 Camera::WorldToScreenPoint(Vector3 position) {
        static HaxMethod<Vector3(__fastcall*)(Camera*,Vector3)> method(Camera::GetClass()->FindMethod("WorldToScreenPoint", "UnityEngine.Vector3(UnityEngine.Vector3)"));
        if (HaxSdk::GetGlobals().backend & HaxBackend_Mono) {
            void* args[] = { &position };
            return *(Vector3*)method.Invoke(this, args)->Unbox();
        }

        return method.ptr(this, position);
    }

    Vector3 Camera::WorldToScreenPoint_Injected(Vector3 position, Camera::MonoOrStereoscopicEye eye) {
        static HaxMethod<void(__fastcall*)(Camera*, Vector3*,Int32,Vector3*)> method(Camera::GetClass()->FindMethod("WorldToScreenPoint_Injected"));
        printf("%p\n", method.ptr);
        Vector3 res;
        method.ptr(this, &position, eye, &res);
        return res;
    }

    Vector3 Camera::INTERNAL_CALL_WorldToScreenPoint(Vector3 position) {
        static HaxMethod<void(__fastcall*)(Camera*,Vector3*,Vector3*)> method(Camera::GetClass()->FindMethod("INTERNAL_CALL_WorldToScreenPoint"));
        Vector3 res;
        method.ptr(this, &position, &res);
        return res;
    }

    float Camera::GetOrthographicSize() {
        static HaxMethod<float(*)(Camera*)> method(Camera::GetClass()->FindMethod("get_orthographicSize"));
        return HaxSdk::GetGlobals().backend & HaxBackend_Mono ? *(float*)method.Invoke(nullptr, nullptr)->Unbox() : method.ptr(this);
    }

    void Camera::SetOrthographicSize(float value) {
        static HaxMethod<void(*)(Camera*, float)> method(Camera::GetClass()->FindMethod("set_orthographicSize"));
        if (HaxSdk::GetGlobals().backend & HaxBackend_Mono) {
            void* args[] = { &value };
            method.Invoke(this, args);
            return;
        }

        method.ptr(this, value);
    }

    Int32 Camera::GetPixelWidth() {
        static HaxMethod<Int32(*)(Camera*)> method(Camera::GetClass()->FindMethod("get_pixelWidth"));
        return HaxSdk::GetGlobals().backend & HaxBackend_Mono ? *(Int32*)method.Invoke(this, nullptr)->Unbox() : method.ptr(this);
    }

    Int32 Camera::GetPixelHeight() {
        static HaxMethod<Int32(*)(Camera*)> method(Camera::GetClass()->FindMethod("get_pixelHeight"));
        return HaxSdk::GetGlobals().backend & HaxBackend_Mono ? *(Int32*)method.Invoke(this, nullptr)->Unbox() : method.ptr(this);
    }

    Matrix4x4 Camera::GetProjectionMatrix() {
        static HaxMethod<Matrix4x4(*)(Camera*)> method(Camera::GetClass()->FindMethod("get_projectionMatrix"));
        return HaxSdk::GetGlobals().backend & HaxBackend_Mono ? *(Matrix4x4*)method.Invoke(this, nullptr)->Unbox() : method.ptr(this);
    }

    Matrix4x4 Camera::GetWorldToCameraMatrix() {
        static HaxMethod<Matrix4x4(*)(Camera*)> method(Camera::GetClass()->FindMethod("get_worldToCameraMatrix"));
        return HaxSdk::GetGlobals().backend & HaxBackend_Mono ? *(Matrix4x4*)method.Invoke(this, nullptr)->Unbox() : method.ptr(this);
    }

    float Camera::GetFarClipPlane() {
        static HaxMethod<float(*)(Camera*)> method(Camera::GetClass()->FindMethod("get_farClipPlane"));
        return HaxSdk::GetGlobals().backend & HaxBackend_Mono ? *(float*)method.Invoke(this, nullptr)->Unbox() : method.ptr(this);
    }

    void Camera::SetFarClipPlane(float value) {
        static HaxMethod<void(*)(Camera*, float)> method(Camera::GetClass()->FindMethod("set_farClipPlane"));
        if (HaxSdk::GetGlobals().backend & HaxBackend_Mono) {
            void* args[] = { &value };
            method.Invoke(this, args);
            return;
        }
        method.ptr(this, value);
    }

    float Camera::GetNearClipPlane() {
        static HaxMethod<float(*)(Camera*)> method(Camera::GetClass()->FindMethod("get_nearClipPlane"));
        return HaxSdk::GetGlobals().backend & HaxBackend_Mono ? *(float*)method.Invoke(this, nullptr)->Unbox() : method.ptr(this);
    }

    float Camera::GetFieldOfView() {
        static HaxMethod<float(*)(Camera*)>method(Camera::GetClass()->FindMethod("get_fieldOfView"));
        return HaxSdk::GetGlobals().backend & HaxBackend_Mono ? *(float*)method.Invoke(this, nullptr)->Unbox() : method.ptr(this);
    }

    Class* Collider::GetClass() {
        static Class* pClass = Class::Find(MODULE, NAMESPACE, "Collider");
        return pClass;
    }

    bool Collider::GetEnabled() {
        static HaxMethod<bool(__fastcall*)(Collider*)> method(Collider::GetClass()->FindMethod("get_enabled"));
        return HaxSdk::GetGlobals().backend & HaxBackend_Mono ? *(bool*)method.Invoke(this, nullptr)->Unbox() : method.ptr(this);
    }

    void Collider::SetEnabled(bool value) {
        static HaxMethod<void(__fastcall*)(Collider*,bool)> method(Collider::GetClass()->FindMethod("set_enabled"));
        if (HaxSdk::GetGlobals().backend & HaxBackend_Mono) {
            void* args[] = { &value };
            method.Invoke(this, args);
            return;
        }

        method.ptr(this, value);
    }

    Bounds Collider::GetBounds() {
        static HaxMethod<Bounds(__fastcall*)(Collider*)> method(Collider::GetClass()->FindMethod("get_bounds"));
        return HaxSdk::GetGlobals().backend & HaxBackend_Mono ? *(Unity::Bounds*)method.Invoke(this, nullptr)->Unbox() : method.ptr(this);
    }

    Class* Object::GetClass() {
        static Class* pClass = Class::Find(MODULE, NAMESPACE, "Object");
        return pClass;
    }

    Object* Object::Instantiate(Object* original) {
        static HaxMethod<Object*(*)(Object*)> method(Object::GetClass()->FindMethod("Instantiate", "UnityEngine.Object(UnityEngine.Object)"));
        if (HaxSdk::GetGlobals().backend & HaxBackend_Mono) {
            void* args[] = { original };
            return (Object*)method.Invoke(nullptr, args);
        }
        return method.ptr(original);
    }

    Object* Object::Instantiate(Object* original, Vector3 position, Quaternion rotation) {
        static HaxMethod<Object*(*)(Object*,Vector3,Quaternion)> method(Object::GetClass()->FindMethod("Instantiate", "UnityEngine.Object(UnityEngine.Object,UnityEngine.Vector3,UnityEngine.Quaternion)"));
        if (HaxSdk::GetGlobals().backend & HaxBackend_Mono) {
            void* args[] = { original, &position, &rotation };
            return (Object*)method.Invoke(nullptr, args);
        }
        return method.ptr(original, position, rotation);
    }

    System::Array<Object*>* Object::FindObjectsOfType(System::Type* pType) {
        static HaxMethod<System::Array<Object*>*(*)(System::Type*)> method(Object::GetClass()->FindMethod("FindObjectsOfType", "UnityEngine.Object[](System.Type)"));
        if (HaxSdk::GetGlobals().backend & HaxBackend_Mono) {
            void* args[] = { pType };
            return (System::Array<Object*>*)method.Invoke(nullptr, args);
        }
        return method.ptr(pType);
    }

    Object* Object::FindObjectOfType(System::Type* pType) {
        static HaxMethod<Object*(*)(System::Type*)> method(Object::GetClass()->FindMethod("FindObjectOfType", "UnityEngine.Object(System.Type)"));
        if (HaxSdk::GetGlobals().backend & HaxBackend_Mono) {
            void* args[] = { pType };
            return (Object*)method.Invoke(nullptr, args);
        }

        return method.ptr(pType);
    }

    void Object::Destroy(Object* pObj, float t) {
        static HaxMethod<void(*)(Object*,float)> method(Object::GetClass()->FindMethod("Destroy"));
        if (HaxSdk::GetGlobals().backend & HaxBackend_Mono) {
            void* args[] = { pObj, &t};
            method.Invoke(nullptr, args);
            return;
        }

        method.ptr(pObj, t);
    }

    System::String* Object::GetName() {
        static HaxMethod<System::String*(*)(Object*)> method(Object::GetClass()->FindMethod("get_name"));
        return HaxSdk::GetGlobals().backend & HaxBackend_Mono ? (System::String*)method.Invoke(this, nullptr) : method.ptr(this);
    }

    void Object::SetName(System::String* pName) {
        static HaxMethod<void(*)(Object*, System::String*)> method(Object::GetClass()->FindMethod("set_name"));
        if (HaxSdk::GetGlobals().backend & HaxBackend_Mono) {
            void* args[] = { pName };
            method.Invoke(this, args);
            return;
        }

        method.ptr(this, pName);
    }

    void Object::SetHideFlags(HideFlags flags) {
        static HaxMethod<void(*)(Object*, HideFlags)> method(Object::GetClass()->FindMethod("set_hideFlags"));
        if (HaxSdk::GetGlobals().backend & HaxBackend_Mono) {
            void* args[] = { &flags };
            method.Invoke(this, args);
            return;
        }

        method.ptr(this, flags);
    }

    Class* GameObject::GetClass() {
        static Class* pClass = Class::Find(MODULE, NAMESPACE, "GameObject");
        return pClass;
    }

    GameObject* GameObject::New(const char* name) {
        GameObject* newGameObject = (GameObject*)System::Object::New(GameObject::GetClass());
        auto* pName = System::String::New(name);

        static HaxMethod<void(*)(GameObject*, System::String*)> method(GameObject::GetClass()->FindMethod(".ctor", "System.Void(System.String)"));
        if (HaxSdk::GetGlobals().backend & HaxBackend_Mono) {
            void* args[1] = { pName };
            method.Invoke(newGameObject, args);
            return newGameObject;
        }
        
        method.ptr(newGameObject, pName);
        return newGameObject;
    }

    Transform* GameObject::GetTransform() {
        static HaxMethod<Transform*(*)(GameObject*)> method(GameObject::GetClass()->FindMethod("get_transform"));
        return HaxSdk::GetGlobals().backend & HaxBackend_Mono ? (Transform*)method.Invoke(this, nullptr) : method.ptr(this);
    }

    void GameObject::SetLayer(Int32 value) {
        static HaxMethod<void(*)(GameObject*,Int32)> method(GameObject::GetClass()->FindMethod("set_layer"));
        if (HaxSdk::GetGlobals().backend & HaxBackend_Mono) {
            void* args[] = { &value };
            method.Invoke(this, args);
            return;
        }

        method.ptr(this, value);
    }

    bool GameObject::GetActiveSelf() {
        static HaxMethod<bool(*)(GameObject*)> method(GameObject::GetClass()->FindMethod("get_activeSelf"));
        return HaxSdk::GetGlobals().backend & HaxBackend_Mono ? *(bool*)method.Invoke(this, nullptr)->Unbox() : method.ptr(this);
    }

    Component* GameObject::GetComponent(System::Type* pType) {
        static HaxMethod<Component*(*)(GameObject*,System::Type*)> method(GameObject::GetClass()->FindMethod("GetComponent", "UnityEngine.Component(System.Type)"));
        if (HaxSdk::GetGlobals().backend & HaxBackend_Mono) {
            void* args[] = { pType };
            return (Component*)method.Invoke(this, args);
        }

        return method.ptr(this, pType);
    }

    Component* GameObject::AddComponent(System::Type* pType) {
        static HaxMethod<Component*(*)(GameObject*,System::Type*)> method(GameObject::GetClass()->FindMethod("AddComponent", "UnityEngine.Component(System.Type)"));
        if (HaxSdk::GetGlobals().backend & HaxBackend_Mono) {
            void* args[] = { pType };
            return (Component*)method.Invoke(this, args);
        }

        return method.ptr(this, pType);
    }

    System::Array<Component*>* GameObject::GetComponentsInChildren(System::Type* pType, bool includeInactive = false) {
        static HaxMethod<System::Array<Component*>*(*)(GameObject*,System::Type*,bool)> method(GameObject::GetClass()->FindMethod("GetComponentsInChildren", "Component[](System.Type,System.Boolean)"));
        if (HaxSdk::GetGlobals().backend & HaxBackend_Mono) {
            void* args[] = { pType, &includeInactive };
            return (System::Array<Component*>*)method.Invoke(this, args);
        }

        return method.ptr(this, pType, includeInactive);
    }

    void GameObject::SetActive(bool value) {
        static HaxMethod<void(*)(GameObject*,bool)> method(GameObject::GetClass()->FindMethod("set_active"));
        if (HaxSdk::GetGlobals().backend & HaxBackend_Mono) {
            void* args[] = { &value };
            method.Invoke(this, args);
            return; 
        }

        return method.ptr(this, value);
    }

    bool GameObject::GetActive() {
        static HaxMethod<bool(*)(GameObject*)> method(GameObject::GetClass()->FindMethod("get_active"));
        return HaxSdk::GetGlobals().backend & HaxBackend_Mono ? *(bool*)method.Invoke(this, nullptr)->Unbox() : method.ptr(this);
    }

    bool GameObject::GetActiveInHierarchy() {
        static HaxMethod<bool(*)(GameObject*)> method(GameObject::GetClass()->FindMethod("get_activeInHierarchy"));
        return HaxSdk::GetGlobals().backend & HaxBackend_Mono ? *(bool*)method.Invoke(this, nullptr)->Unbox() : method.ptr(this);
    }

    Transform* Component::GetTransform() {
        static HaxMethod<Transform*(*)(Component*)> method(Component::GetClass()->FindMethod("get_transform"));
        return HaxSdk::GetGlobals().backend & HaxBackend_Mono ? (Transform*)method.Invoke(this, nullptr) : method.ptr(this);
    }

    Class* Component::GetClass() {
        static Class* pClass = Class::Find(MODULE, NAMESPACE, "Component");
        return pClass;
    }

    GameObject* Component::GetGameObject() {
        static HaxMethod<GameObject*(*)(Component*)> method(Component::GetClass()->FindMethod("get_gameObject"));
        return HaxSdk::GetGlobals().backend & HaxBackend_Mono ? (GameObject*)method.Invoke(this, nullptr) : method.ptr(this);
    }

    Component* Component::GetComponentInChildren(System::Type* pType) {
        static HaxMethod<Component*(*)(Component*,System::Type*)> method(Component::GetClass()->FindMethod("GetComponentInChildren", "UnityEngine.Component(System.Type)"));
        if (HaxSdk::GetGlobals().backend & HaxBackend_Mono) {
            void* args[] = { pType };
            return (Component*)method.Invoke(this, args);
        }

        return method.ptr(this, pType);
    }

    System::Array<Component*>* Component::GetComponentsInChildren(System::Type* pType) {
        static HaxMethod<System::Array<Component*>*(*)(Component*,System::Type*)> method(Component::GetClass()->FindMethod("GetComponentsInChildren", "UnityEngine.Component[](System.Type)"));
        if (HaxSdk::GetGlobals().backend & HaxBackend_Mono) {
            void* args[] = { pType };
            return (System::Array<Component*>*)method.Invoke(this, args);
        }

        return method.ptr(this, pType);
    }

    Component* Component::GetComponent(System::Type* pType) {
        static HaxMethod<Component*(*)(Component*,System::Type*)> method(this->GetClass()->FindMethod("GetComponent", "UnityEngine.Component(System.Type)"));
        if (HaxSdk::GetGlobals().backend & HaxBackend_Mono) {
            void* args[] = { pType };
            return (Component*)method.Invoke(this, args);
        }

        return method.ptr(this, pType);
    }

    Class* Shader::GetClass() {
        static Class* pClass = Class::Find(MODULE, NAMESPACE, "Shader");
        return pClass;
    }

    Shader* Shader::Find(System::String* pName) {
        static HaxMethod<Shader*(*)(System::String*)> method(Shader::GetClass()->FindMethod("Find", nullptr));
        if (HaxSdk::GetGlobals().backend & HaxBackend_Mono) {
            void* args[] = { pName };
            return (Shader*)method.Invoke(nullptr, args);
        }

        return method.ptr(pName);
    }

    Class* Material::GetClass() {
        static Class* pClass = Class::Find(MODULE, NAMESPACE, "Material");
        return pClass;
    }

    Material* Material::New(Shader* pShader) {
        Material* pMaterial = (Material*)System::Object::New(Material::GetClass());
        static HaxMethod<void(*)(Material*, Shader*)> method(Material::GetClass()->FindMethod(".ctor", "System.Void(UnityEngine.Shader)"));
        if (HaxSdk::GetGlobals().backend & HaxBackend_Mono) {
            void* args[1] = { pShader };
            method.Invoke(pMaterial, args);
            return pMaterial;
        }

        method.ptr(pMaterial, pShader);
        return pMaterial;
    }

    void Material::SetInt(System::String* pName, Int32 value) {
        static HaxMethod<void(*)(Material*,System::String*,Int32)> method(Material::GetClass()->FindMethod("SetInt", "System.Void(System.String,System.Int32)"));
        if (HaxSdk::GetGlobals().backend & HaxBackend_Mono) {
            void* args[] = { pName, &value };
            method.Invoke(this, args);
            return;
        }

        return method.ptr(this, pName, value);
    }

    void Material::SetColor(Unity::Color color) {
        static HaxMethod<void(*)(Material*, Unity::Color)> method(Material::GetClass()->FindMethod("set_color", nullptr));
        if (HaxSdk::GetGlobals().backend & HaxBackend_Mono) {
            void* args[] = { &color };
            method.Invoke(this, args);
            return;
        }

        return method.ptr(this, color);
    }

    System::Type* KeyCode::GetSystemType() {
        static System::Type* pType = Class::Find(MODULE, NAMESPACE, "KeyCode")->GetSystemType();
        return pType;
    }

    Class* Screen::GetClass() {
        static Class* pClass = Class::Find(MODULE, NAMESPACE, "Screen");
        return pClass;
    }

    Int32 Screen::GetHeight() {
        static HaxMethod<Int32(*)()> method(Screen::GetClass()->FindMethod("get_height", nullptr));
        return HaxSdk::GetGlobals().backend & HaxBackend_Mono ? *(Int32*)method.Invoke(nullptr, nullptr)->Unbox() : method.ptr();
    }

    Int32 Screen::GetWidth() {
        static HaxMethod<Int32(*)()> method(Screen::GetClass()->FindMethod("get_width", nullptr));
        return HaxSdk::GetGlobals().backend & HaxBackend_Mono ? *(Int32*)method.Invoke(nullptr, nullptr)->Unbox() : method.ptr();
    }

    Class* Transform::GetClass() {
        static Class* pClass = Class::Find(MODULE, NAMESPACE, "Transform");
        return pClass;
    }

    Vector3 Transform::GetPosition() {
        static HaxMethod<Vector3(*)(Transform*)> method(Transform::GetClass()->FindMethod("get_position"));
        return HaxSdk::GetGlobals().backend & HaxBackend_Mono ? *(Vector3*)method.Invoke(this, nullptr)->Unbox() : method.ptr(this);
    }

    void Transform::SetPosition(Vector3 value) {
        static HaxMethod<void(*)(Transform*,Vector3)> method(Transform::GetClass()->FindMethod("set_position"));
        if (HaxSdk::GetGlobals().backend & HaxBackend_Mono) {
            void* args[] = { &value };
            method.Invoke(this, args);
            return;
        }

        method.ptr(this, value);
    }

    Transform* Transform::GetParent() {
        static HaxMethod<Transform*(*)(Transform*)> method(Transform::GetClass()->FindMethod("get_parent"));
        return HaxSdk::GetGlobals().backend & HaxBackend_Mono ? (Transform*)method.Invoke(this, nullptr) : method.ptr(this);
    }

    void Transform::SetParent(Transform* value) {
        static HaxMethod<void(*)(Transform*,Transform*)> method(Transform::GetClass()->FindMethod("set_parent"));
        if (HaxSdk::GetGlobals().backend & HaxBackend_Mono) {
            void* args[] = { value };
            method.Invoke(this, args);
            return;
        }

        method.ptr(this, value);
    }

    Vector3 Transform::GetForward() {
        static HaxMethod<Vector3(*)(Transform*)> method(Transform::GetClass()->FindMethod("get_forward"));
        return HaxSdk::GetGlobals().backend & HaxBackend_Mono ? *(Vector3*)method.Invoke(this, nullptr)->Unbox() : method.ptr(this);
    }

    float Unity::Vector3::Distance(const Unity::Vector3& a, const Unity::Vector3& b) {
        Vector3 vector = { a.x - b.x, a.y - b.y, a.z - b.z };
        return std::sqrt(vector.x * vector.x + vector.y + vector.y + vector.z + vector.z);
    }

    float Unity::Vector3::Distance(Unity::Vector3& other) {
        Vector3 vector = { this->x - other.x, this->y - other.y, this->z - other.z };
        return std::sqrt(vector.x * vector.x + vector.y + vector.y + vector.z + vector.z);
    }

    float Unity::Vector3::Distance(Unity::Vector3&& other) {
        Vector3 vector = { this->x - other.x, this->y - other.y, this->z - other.z };
        return std::sqrt(vector.x * vector.x + vector.y + vector.y + vector.z + vector.z);
    }

    Unity::Vector4 Unity::Matrix4x4::operator*(const Unity::Vector4& v) {
        Unity::Vector4 result = { 0 };
        result.x = m00 * v.x + m01 * v.y + m02 * v.z + m03 * v.w;
        result.y = m10 * v.x + m11 * v.y + m12 * v.z + m13 * v.w;
        result.z = m20 * v.x + m21 * v.y + m22 * v.z + m23 * v.w;
        result.w = m30 * v.x + m31 * v.y + m32 * v.z + m33 * v.w;
        return result;
    }

    Unity::Vector3 Unity::Matrix4x4::MultiplyPoint(Unity::Vector3& point) {
        Vector3 result;
        result.x = m00 * point.x + m01 * point.y + m02 * point.z + m03;
        result.y = m10 * point.x + m11 * point.y + m12 * point.z + m13;
        result.z = m20 * point.x + m21 * point.y + m22 * point.z + m23;
        float num = m30 * point.x + m31 * point.y + m32 * point.z + m33;
        num = 1.f / num;
        result.x *= num;
        result.y *= num;
        result.z *= num;
        return result;
    }

    Class* Light::GetClass() {
        static Class* pClass = Class::Find(MODULE, NAMESPACE, "Light");
        return pClass;
    }

    void Light::SetIntensity(float value) {
        static HaxMethod<void(*)(Light*,float)> method(Light::GetClass()->FindMethod("set_intensity"));
        if (HaxSdk::GetGlobals().backend & HaxBackend_Mono) {
            void* args[] = { &value };
            method.Invoke(this, args);
            return;
        }

        method.ptr(this, value);
    }

    float Light::GetIntensity() {
        static HaxMethod<float(*)(Light*)> method(Light::GetClass()->FindMethod("get_intensity"));
        return HaxSdk::GetGlobals().backend & HaxBackend_Mono ? *(float*)method.Invoke(this, nullptr)->Unbox() : method.ptr(this);
    }

    void Light::SetRange(float value) {
        static HaxMethod<void(*)(Light*, float)> method(Light::GetClass()->FindMethod("set_range"));
        if (HaxSdk::GetGlobals().backend & HaxBackend_Mono) {
            void* args[] = { &value };
            method.Invoke(this, args);
            return;
        }

        method.ptr(this, value);
    }

    float Light::GetRange() {
        static HaxMethod<float(*)(Light*)> method(Light::GetClass()->FindMethod("get_range"));
        return HaxSdk::GetGlobals().backend & HaxBackend_Mono ? *(float*)method.Invoke(this, nullptr)->Unbox() : method.ptr(this);
    }

    Class* LineRenderer::GetClass() {
        static Class* pClass = Class::Find(MODULE, NAMESPACE, "LineRenderer");
        return pClass;
    }

    void LineRenderer::SetStartWidth(float value) {
        static HaxMethod<void(*)(LineRenderer*, float)> method(LineRenderer::GetClass()->FindMethod("set_startWidth"));
        if (HaxSdk::GetGlobals().backend & HaxBackend_Mono) {
            void* args[] = { &value };
            method.Invoke(this, args);
            return;
        }

        method.ptr(this, value);
    }

    void LineRenderer::SetEndWidth(float value) {
        static HaxMethod<void(*)(LineRenderer*, float)> method(LineRenderer::GetClass()->FindMethod("set_endWidth"));
        if (HaxSdk::GetGlobals().backend & HaxBackend_Mono) {
            void* args[] = { &value };
            method.Invoke(this, args);
            return;
        }

        method.ptr(this, value);
    }

    void LineRenderer::SetPositionCount(Int32 value) {
        static HaxMethod<void(*)(LineRenderer*, Int32)> method(LineRenderer::GetClass()->FindMethod("set_positionCount"));
        if (HaxSdk::GetGlobals().backend & HaxBackend_Mono) {
            void* args[] = { &value };
            method.Invoke(this, args);
            return;
        }

        method.ptr(this, value);
    }

    void LineRenderer::SetPosition(Int32 index, Unity::Vector3 position) {
        static HaxMethod<void(*)(LineRenderer*, Int32, Unity::Vector3)> method(LineRenderer::GetClass()->FindMethod("SetPosition", "System.Void(System.Int32,UnityEngine.Vector3)"));
        if (HaxSdk::GetGlobals().backend & HaxBackend_Mono) {
            void* args[] = { &index, &position };
            method.Invoke(this, args);
            return;
        }

        method.ptr(this, index, position);
    }

    Class* Renderer::GetClass() {
        static Class* pClass = Class::Find(MODULE, NAMESPACE, "Renderer");
        return pClass;
    }

    Bounds Renderer::GetBounds() {
        static HaxMethod<Bounds(*)(Renderer*)> method(Renderer::GetClass()->FindMethod("get_bounds"));
        return HaxSdk::GetGlobals().backend & HaxBackend_Mono ? *(Bounds*)method.Invoke(this, nullptr)->Unbox() : method.ptr(this);
    }

    Material* Renderer::GetSharedMaterial() {
        static HaxMethod<Material*(*)(Renderer*)> method(Renderer::GetClass()->FindMethod("get_sharedMaterial"));
        return HaxSdk::GetGlobals().backend & HaxBackend_Mono ? (Material*)method.Invoke(this, nullptr) : method.ptr(this);
    }

    /*void Renderer::SetSharedMaterial(Material* pMaterial) {
        static HaxMethod<void(*)(Renderer*, Material*)> method(Renderer::GetClass()->FindMethod("set_sharedMaterial"));
        if (HaxSdk::GetGlobals().backend & HaxBackend_Mono) {
            void* args[] = { pMaterial };
            method.Invoke(this, args);
            return;
        }
        method.ptr(this, pMaterial);
    }*/

    void Renderer::SetMaterial(Material* pMaterial) {
        static HaxMethod<void(*)(Renderer*, Material*)> method(Renderer::GetClass()->FindMethod("set_material"));
        if (HaxSdk::GetGlobals().backend & HaxBackend_Mono) {
            void* args[] = { pMaterial };
            method.Invoke(this, args);
            return;
        }
        method.ptr(this, pMaterial);
    }

    void Renderer::SetEnabled(bool value) {
        static HaxMethod<void(*)(Renderer*, bool)> method(Renderer::GetClass()->FindMethod("set_enabled"));
        if (HaxSdk::GetGlobals().backend & HaxBackend_Mono) {
            void* args[] = { &value };
            method.Invoke(this, args);
            return;
        }
        method.ptr(this, value);
    }

    void Renderer::SetSortingOrder(Int32 value) {
        static HaxMethod<void(*)(Renderer*, Int32)> method(Renderer::GetClass()->FindMethod("set_sortingOrder"));
        if (HaxSdk::GetGlobals().backend & HaxBackend_Mono) {
            void* args[] = { &value };
            method.Invoke(this, args);
            return;
        }

        method.ptr(this, value);
    }

    Class* MeshRenderer::GetClass() {
        static Class* pClass = Class::Find(MODULE, NAMESPACE, "MeshRenderer");
        return pClass;
    }

    bool IsNotNull(Component* pComponent) {
        static HaxMethod<bool (*)(Component*)> method(Unity::Object::GetClass()->FindMethod("op_Implicit"));

        if (!pComponent)
            return false;

        if (HaxSdk::GetGlobals().backend & HaxBackend_Mono) {
            void* args[] = { pComponent };
            return *(bool*)method.Invoke(nullptr, args)->Unbox();
        }

        return method.ptr(pComponent);
    }
}

namespace Photon {
    using namespace Unity;

    const char* MODULE = "PhotonUnityNetworking";
    const char* NAMESPACE = "Photon.Pun";

    ::Class* PhotonNetwork::GetClass() {
        static Class* pClass = Class::Find(MODULE, NAMESPACE, "PhotonNetwork");
        return pClass;
    }

    GameObject* PhotonNetwork::InstantiateRoomObject(System::String* pName, Vector3 position, Quaternion rotation, Byte group, void* pData) {
        static HaxMethod<Unity::GameObject*(*)(System::String*, Vector3, Quaternion, Byte, void*)> method(PhotonNetwork::GetClass()->FindMethod("InstantiateRoomObject"));
        if (HaxSdk::GetGlobals().backend & HaxBackend_Mono) {
            void* args[] = { pName, &position, &rotation, &group, pData };
            return (GameObject*)method.Invoke(nullptr, args);
        }
        return method.ptr(pName, position, rotation, group, pData);
    }
}
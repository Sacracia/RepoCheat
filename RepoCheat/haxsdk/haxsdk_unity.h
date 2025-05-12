#pragma once

#include <cmath>

#include "haxsdk.h"

namespace Unity {
    struct                                      AsyncOperation;
    struct                                      Behaviour;
    struct                                      BoxCollider;
    struct                                      Bounds;
    struct                                      Camera;
    struct                                      Component;
    struct                                      Collider;
    struct                                      Collider2D;
    struct                                      Color;
    struct                                      GameObject;
    struct                                      KeyCode;
    struct                                      Light;
    struct                                      LineRenderer;
    struct                                      Object;
    struct                                      Renderer;
    struct                                      Rect;
    struct                                      Screen;
    struct                                      Sprite;
    struct                                      Shader;
    struct                                      Material;
    struct                                      Transform;
    struct                                      Vector2;
    struct                                      Vector3;
    struct                                      Quaternion;
}

namespace Unity {
    enum HideFlags : Int32 {
        None = 0,
        HideInHierarchy = 1,
        HideInInspector = 2,
        DontSaveInEditor = 4,
        NotEditable = 8,
        DontSaveInBuild = 16,
        DontUnloadUnusedAsset = 32,
        DontSave = 52,
        HideAndDontSave = 61
    };

    struct Object : System::Object {
        Object() = delete;
        Object(const Object&) = delete;

        static ::Class*                         GetClass();
        static Object*                          Instantiate(Object* original);
        static Object*                          Instantiate(Object* original, Vector3 position, Quaternion rotation);;
        static System::Array<Object*>*          FindObjectsOfType(System::Type* type);
        static Object*                          FindObjectOfType(System::Type* type);
        static void                             Destroy(Object* obj, float t = 0.f);

        System::String*                         GetName();
        void                                    SetName(System::String* pName);
        void                                    SetHideFlags(HideFlags flags);

        void*                                   m_CachedPtr;
    };

    struct Component : Object {
        Component() = delete;
        Component(const Component&) = delete;

        static ::Class*                         GetClass();

        Transform*                              GetTransform();
        GameObject*                             GetGameObject();

        Component*                              GetComponentInChildren(System::Type*);
        System::Array<Component*>*              GetComponentsInChildren(System::Type*);
        Component*                              GetComponent(System::Type*);

    };

    struct Shader {
        static ::Class*                         GetClass();

        static Shader*                          Find(System::String* pName);
    };

    struct Material : Unity::Object {
        static Class* GetClass();

        static Material* New(Shader* pShader);

        void SetInt(System::String* pName, Int32 value);
        void SetColor(Unity::Color color);
    };

    struct AsyncOperation {
        AsyncOperation() = delete;
        AsyncOperation(const AsyncOperation&) = delete;

        static ::Class*                         GetClass();

        bool                                    GetIsDone();
        float                                   GetProgress();
    };

    struct Behaviour : Component {
        Behaviour() = delete;
        Behaviour(const Behaviour&) = delete;

        static ::Class*                         GetClass();

        bool                                    GetEnabled();
        bool                                    GetIsActiveAndEnabled();
        void                                    SetEnabled(bool value);
    };

    struct Collider : Component {
        Collider() = delete;
        Collider(const Collider&) = delete;

        static ::Class*                         GetClass();

        bool                                    GetEnabled();
        void                                    SetEnabled(bool value);
        Unity::Bounds                           GetBounds();
    };

    struct BoxCollider : Collider {
        BoxCollider() = delete;
        BoxCollider(const BoxCollider&) = delete;

        static ::Class*                         GetClass();

        Vector3                                 InternalGetCenter();
        Vector3                                 InternalGetSize();
        void                                    InternalSetCenter(Vector3 value);
        void                                    InternalSetSize(Vector3 value);
        Vector3                                 GetCenter();
        Vector3                                 GetSize();
    };

    struct Vector2 {
        Vector2                                 operator+(const Unity::Vector2& a) const { return Vector2(x + a.x, y + a.y); }
        Vector2                                 operator*(float mult) const { return Vector2(x * mult, y * mult); }

        float x;
        float y;
    };

    struct Vector3 {
        Vector3() : x(0.f), y(0.f), z(0.f) {}
        Vector3(float x_, float y_, float z_) : x(x_), y(y_), z(z_) {}
        Vector3(float x_, float y_) : x(x_), y(y_), z(0.f) {}

        static float                            Distance(const Unity::Vector3& a, const Unity::Vector3& b);
        static Vector3                          Min(const Vector3& a, const Vector3& b) { return Vector3((std::min)(a.x, b.x), (std::min)(a.y, b.y), (std::min)(a.z, b.z)); }
        static Vector3                          Max(const Vector3& a, const Vector3& b) { return Vector3((std::max)(a.x, b.x), (std::max)(a.y, b.y), (std::max)(a.z, b.z)); }

        float                                   Distance(Unity::Vector3& other);
        float                                   Distance(Unity::Vector3&& other);

        Vector2                                 ToVector2()                                 { return Unity::Vector2(this->x, this->y); }

        Vector3                                 operator+(const Unity::Vector3& a) const    { return Vector3(x + a.x, y + a.y, z + a.z); }
        Vector3                                 operator-(const Unity::Vector3& a) const    { return Vector3(x - a.x, y - a.y, z - a.z); }
        Vector3                                 operator*(float mult) const                 { return Vector3(x * mult, y * mult, z * mult); }

        static Vector3                          zero()                                      { return Vector3(0, 0, 0); }
        static Vector3                          one()                                       { return Vector3(1, 1, 1); }
        static Vector3                          up()                                        { return Vector3(0, 1, 0); }
        static Vector3                          down()                                      { return Vector3(0, -1, 0); }
        static Vector3                          left()                                      { return Vector3(-1, 0, 0); }
        static Vector3                          right()                                     { return Vector3(1, 0, 0); }
        static Vector3                          forward()                                   { return Vector3(0, 0, 1); }
        static Vector3                          back()                                      { return Vector3(0, 0, -1); }

        float x;
        float y;
        float z;
    };

    struct Vector4 {
        float x, y, z, w;
    };

    struct Matrix4x4 {
        Vector4 operator*(const Vector4& v);

        Vector3 MultiplyPoint(Vector3& point);

        float m00, m10, m20, m30;
        float m01, m11, m21, m31;
        float m02, m12, m22, m32;
        float m03, m13, m23, m33;
    };

    struct Bounds {
        Bounds(const Vector3& _center, const Vector3& _size) : center(_center), extents(_size * 0.5f) {}

        inline Vector3 GetSize() { return extents * 2.f; }
        inline Vector3 SetSize(const Vector3& size) { extents = size * 0.5f; }

        inline Vector3 GetMin() { return center - extents; }
        inline Vector3 GetMax() { return center + extents; }

        void SetMinMax(const Vector3& min, const Vector3& max) { extents = (max - min) * 0.5f; center = min + extents; }
        void Encapsulate(const Vector3& point) { SetMinMax(Vector3::Min(GetMin(), point), Vector3::Max(GetMax(), point)); }
        void Encapsulate(const Bounds& bounds) { Encapsulate(bounds.center - bounds.extents); Encapsulate(bounds.center + bounds.extents); }

        Vector3 center;
        Vector3 extents;
    };

    struct Camera : Behaviour {
        enum MonoOrStereoscopicEye : Int32 {
            Left,
            Right,
            Mono
        };

        Camera() = delete;
        Camera(const Camera&) = delete;

        static ::Class*                         GetClass();
        static Camera*                          GetMain();

        Vector3                                 WorldToScreenPoint(Vector3 position);
        Vector3                                 WorldToScreenPoint_Injected(Vector3 position, MonoOrStereoscopicEye eye);
        Vector3                                 INTERNAL_CALL_WorldToScreenPoint(Vector3 position);

        float                                   GetOrthographicSize();
        void                                    SetOrthographicSize(float value);

        Int32                                   GetPixelWidth();
        Int32                                   GetPixelHeight();
        Matrix4x4                               GetProjectionMatrix();
        Matrix4x4                               GetWorldToCameraMatrix();
        float                                   GetFarClipPlane();
        void                                    SetFarClipPlane(float value);
        float                                   GetNearClipPlane();
        float                                   GetFieldOfView();        
    };

    struct Collider2D : Behaviour {
        Collider2D() = delete;
        Collider2D(const Collider2D&) = delete;
    };

    struct Color {
        static Color                            red()       { return Color(1.f, 0.f, 0.f, 1.f); }
        static Color                            green()     { return Color(0.f, 1.f, 0.f, 1.f); }
        static Color                            blue()      { return Color(0.f, 0.f, 1.f, 1.f); }
        static Color                            white()     { return Color(1.f, 1.f, 1.f, 1.f); }
        static Color                            black()     { return Color(0.f, 0.f, 0.f, 1.f); }
        static Color                            yellow()    { return Color(1.f, 0.92156863f, 0.015686275f, 1.f); }
        static Color                            cyan()      { return Color(0.f, 1.f, 1.f, 1.f); }
        static Color                            magenta()   { return Color(1.f, 0.f, 1.f, 1.f); }
        static Color                            gray()      { return Color(0.5f, 0.5f, 0.5f, 0.5f); }
        static Color                            grey()      { return Color(0.5f, 0.5f, 0.5f, 0.5f); }
        static Color                            clear()     { return Color(0.f, 0.f, 0.f, 0.f); }

        float r;
        float g;
        float b;
        float a;
    };

    struct GameObject : Object {
        GameObject() = delete;
        GameObject(const GameObject&) = delete;

        static ::Class*                         GetClass();
        static inline GameObject*               New() { return (GameObject*)System::Object::New(GetClass())->Ctor(); }
        static GameObject*                      New(const char* name);
        Transform*                              GetTransform();
        void                                    SetLayer(Int32 value);
        bool                                    GetActiveSelf();
        Component*                              GetComponent(System::Type* type);
        Component*                              AddComponent(System::Type* componentType);
        System::Array<Component*>*              GetComponentsInChildren(System::Type* pType, bool includeInactive);
        void                                    SetActive(bool value);
        bool                                    GetActive();
        bool                                    GetActiveInHierarchy();
    };

    struct KeyCode {
        static System::Type*                    GetSystemType();
    };

    struct Screen {
        static Class*                           GetClass();
        static Int32                            GetWidth();
        static Int32                            GetHeight();
    };

    struct Transform : Component {
        Transform() = delete;
        Transform(const Transform&) = delete;

        static ::Class*                         GetClass();

        Vector3                                 GetPosition();
        void                                    SetPosition(Unity::Vector3 value);
        Transform*                              GetParent();
        void                                    SetParent(Unity::Transform* value);
        Vector3                                 GetForward();
    };

    struct Quaternion {
        static Unity::Quaternion                identity() { return Quaternion(0.f, 0.f, 0.f, 1.f); }

        float x;
        float y;
        float z;
        float w;
    };

    struct Light : Behaviour {
        static ::Class*                         GetClass();
    public:
        void                                    SetIntensity(float value);
        float                                   GetIntensity();
        void                                    SetRange(float value);
        float                                   GetRange();
    };

    struct Renderer : Component {
        static ::Class*                         GetClass();

        Bounds                                  GetBounds();
        Material*                               GetSharedMaterial();
        //void                                    SetSharedMaterial(Material* pMaterial);
        void                                    SetMaterial(Material* pMaterial);
        void                                    SetEnabled(bool value);
        void                                    SetSortingOrder(Int32 value);
    };

    struct LineRenderer : Renderer {
        static ::Class*                         GetClass();

        void                                    SetStartWidth(float value);
        void                                    SetEndWidth(float value);
        void                                    SetPositionCount(Int32 value);
        void                                    SetPosition(Int32 index, Unity::Vector3 position);
    };

    struct MeshRenderer : Renderer {
        static ::Class*                         GetClass();


    };

    struct Rect {
        static Rect MinMaxRect(float xmin, float ymin, float xmax, float ymax) { return Rect(xmin, ymin, xmax - xmin, ymax - ymin); }

        Vector2 GetCenter() { return Vector2(x + width / 2.f, y + height / 2.f); }

        float x;
        float y;
        float width;
        float height;
    };

    bool IsNotNull(Unity::Component* pComponent);
}

namespace Photon {
    struct PhotonNetwork;

    struct PhotonNetwork {
        static ::Class* GetClass();

        static Unity::GameObject* InstantiateRoomObject(System::String* pName, Unity::Vector3 position, Unity::Quaternion rotation, Byte group, void* pData);
    };
}
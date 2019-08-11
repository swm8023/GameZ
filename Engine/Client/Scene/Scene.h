#pragma once
#include <Core/CoreHeader.h>

namespace z {
class RenderScene;
class RenderItem;
class Camera;

class Scene : public RefCounter {
public:
	Scene();
	bool Load();

	void Tick() {}

	void ColloectEnv(RenderScene*);
	void CollectItems(RenderScene*);

	Camera* GetCamera() {
		return mCamera;
	}

private:
	bool mIsEditor;
	RefCountPtr<Camera> mCamera;

	std::vector<RefCountPtr<RenderItem>> items;
};

}
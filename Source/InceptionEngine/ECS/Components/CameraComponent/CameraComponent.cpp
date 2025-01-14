
#include "IE_PCH.h"
#include "CameraComponent.h"
#include "ECS/Entity/Entity.h"
#include "ECS/World.h"
#include "ECS/Components/TransformComponent/TransformComponent.h"


namespace inceptionengine
{
	CameraComponent::CameraComponent(EntityID entityID, std::reference_wrapper<World> world)
		:mEntityID(entityID), mWorld(world)
	{

	}




	void CameraComponent::SetPosAndForward(Vec3f position, Vec3f forward)
	{
		mPosition = Vec4f(position.x, position.y, position.z, 1.0f);
		mForwardPoint = Vec4f(forward.x, forward.y, forward.z, 1.0f);
	}

	/*
	move this to camera system because it need to communicate with transform component
	*/
	Matrix4x4f CameraComponent::CameraMatrix() const
	{
		Matrix4x4f globalPosition = mWorld.get().GetEntity(mEntityID).GetComponent<TransformComponent>().GetWorldTransform();
		globalPosition = Translate(globalPosition[3]);

		Vec4f worldPosition = globalPosition * mPosition;
		Vec4f worldForward = globalPosition * mForwardPoint;

		return LookAt(Vec3f(worldPosition.x, worldPosition.y, worldPosition.z), Vec3f(worldForward.x, worldForward.y, worldForward.z), Vec3f(0.0f, 1.0f, 0.0f));
	}

	/*
	not accurate, we should translate to the pivot before rotate and then translate back
	*/
	void CameraComponent::RotateHorizontal(float degree)
	{
		auto lookAtDirection = mForwardPoint - mPosition;
		auto rotateionAxis = CrossProduct(Vec3f(lookAtDirection.x, lookAtDirection.y, lookAtDirection.z), Vec3f(0.0f, 1.0f, 0.0f));
		mPosition = Rotate(mPosition, degree, rotateionAxis);
		mForwardPoint = Rotate(mForwardPoint, degree, rotateionAxis);
	}

	void CameraComponent::RotateVertical(float degree)
	{
		mPosition = Rotate(mPosition, degree, Vec3f(0.0f, 1.0f, 0.0f));
		mForwardPoint = Rotate(mForwardPoint, degree, Vec3f(0.0f, 1.0f, 0.0f));
		if (mCameraControlYaw)
		{
			TransformComponent& transformComponent = mWorld.get().GetEntity(mEntityID).GetComponent<TransformComponent>();
			transformComponent.Rotate(Vec3f(0.0f, 1.0f, 0.0f), degree);
		}
	}

	void CameraComponent::PrintCameraPramameter()
	{
		std::cout << "position: " << VecToString(mPosition) << std::endl;
		std::cout << "position: " << VecToString(mForwardPoint) << std::endl;
	}

	void CameraComponent::LiftUp(float amount)
	{
		mPosition.y += amount;
		mForwardPoint.y += amount;
	}

	Vec3f CameraComponent::GetForwardVec() const
	{
		Vec3f temp = Vec3f(mForwardPoint - mPosition);
		temp.y = 0;
		return NormalizeVec(temp);
	}
	void CameraComponent::SetCameraControlYaw(bool enable)
	{
		mCameraControlYaw = enable;
	}
	bool CameraComponent::GetCameraControlYaw() const
	{
		return mCameraControlYaw;
	}
	Vec4f CameraComponent::GetCameraRefPosition() const
	{
		return mPosition;
	}
}

#pragma once
#include <Spore\BasicIncludes.h>

using namespace App;
using namespace Editors;

class FEReconEditorsHandles
{
public:

	static void Initialize() {}

	static void Dispose() {}

	static void AttachDetours();
};

void RingSetScale(EditorBaseHandle* pHandle, float scale)
{
	ModelPtr pModel;
	if (pHandle->mpModel != nullptr) {
		pModel = pHandle->mpModel;
		pModel->mTransform.SetScale(scale);
		*(short*)((int)&pModel->mTransform + 2) += 1;
	}
}
void HandleSetScale(EditorBaseHandle* pHandle, float scale)
{
	ModelPtr pModel;
	if (pHandle->mpModel != nullptr) {
		pModel = pHandle->mpModel;
		pModel->mTransform.SetScale(scale * pHandle->mDefaultScale);
		*(short*)((int)&pModel->mTransform + 2) += 1;
	}
	if (pHandle->mpOverdrawModel != nullptr) {
		pModel = pHandle->mpOverdrawModel;
		pModel->mTransform.SetScale(scale * pHandle->mDefaultScale);
		*(short*)((int)&pModel->mTransform + 2) += 1;
	}
}

/// resize handle scales
member_detour(EditorRigblock_SetHandleScale, EditorRigblock, void())
{
	void detoured()
	{
		if (Editor.mpPropList != nullptr && Editor.mpPropList->HasProperty(id("editorHandleScale")))
		{
			float editorHandleScale;
			Property::GetFloat(Editor.mpPropList.get(), id("editorHandleScale"), editorHandleScale);

			if (this->mpEditorModel != nullptr) {
				float resizeHandles = editorHandleScale * 0.5f;
				if (!this->mMorphHandles.empty())
				{
					for (const EditorBaseHandlePtr& handle : this->mMorphHandles)
						//handle->SetScale(resizeHandles);
						HandleSetScale(handle.get(), resizeHandles);
				}
				if (this->mpRotationBallHandle != nullptr)
					//this->mpRotationBallHandle->SetScale(resizeHandles * 0.16666f);
					RingSetScale(this->mpRotationBallHandle.get(), resizeHandles * 0.16666f);

				for (int i = 0; i < 3; i++)
				{
					if (this->mAxisHandles[i] != nullptr)
						RingSetScale(this->mAxisHandles[i].get(), resizeHandles * 0.5f);
						//this->mAxisHandles[i]->SetScale(resizeHandles * 0.5f);
				}
			}
		}
		else
			original_function(this);
	}
};

class cSPEditorHandleRotationRingPlaceholder
	: public EditorBaseHandle
{
public:
	uint32_t mModelInstance;
	Vector3 mRotationAxis;
	Vector3 mForwardVector;
	uint32_t mRotationAxisID;
	uint32_t mHandlePlacement;
	uint32_t mAnimID;
	float mUnscaledRadius;
	float mHandleRotation;
	float mOffset;
	float mCustomScale;
	float mCurrentAnimTime;
	float mTime;
	bool mExists;
	bool mActLikeBall;
	bool mIsHiddenHandle;
	float mDistanceFromBoundingBox;
	float field_94;
};
ASSERT_SIZE(cSPEditorHandleRotationRingPlaceholder, 0x9C);

/// resize ring radius
member_detour(cSPEditorHandleRotationRing_GetRadius, cSPEditorHandleRotationRingPlaceholder, float())
{
	float detoured()
	{
		float res = original_function(this);
		if (Editor.mpPropList != nullptr && Editor.mpPropList->HasProperty(id("editorHandleScale"))) {
			float editorHandleScale;
			Property::GetFloat(Editor.mpPropList.get(), id("editorHandleScale"), editorHandleScale);
			float magic = 3.5f - ((editorHandleScale / 4) * 0.5f);
			if (magic <= 0) magic = 1;
			if (this->mActLikeBall == false) {
				if (this->mpRigblock->mpEditorModel != nullptr) {
					float resizeHandles = editorHandleScale * 0.5f;
					res = (resizeHandles * 0.05f + this->mUnscaledRadius) * this->mpRigblock->size * this->mCustomScale;
				}
			}
			else res = this->mDistanceFromBoundingBox * this->mpRigblock->size;
		}
		return res;
	}
};

member_detour(cSPEditorHandleRotationRing_GetHandleTime, cSPEditorHandleRotationRingPlaceholder, float())
{
	float detoured()
	{
		float res = original_function(this);
		if (Editor.mpPropList != nullptr && Editor.mpPropList->HasProperty(id("editorHandleScale"))) {
			float editorHandleScale;
			Property::GetFloat(Editor.mpPropList.get(), id("editorHandleScale"), editorHandleScale);

			if ((this->mpRigblock->mpModelWorld == nullptr) || (this->mpModel == nullptr)) {
				res = 0;
			}
			else {
				res = editorHandleScale;
				float modelBoundsSize = res * 0.5f;
				float fStart;
				float fEnd;
				this->mpRigblock->mpModelWorld->GetAnimationRange(this->mpModel.get(), this->mAnimID, fStart, fEnd, 0);
				// GetRadius
				res = CALL
				(
					Address(ModAPI::ChooseAddress(0x481cb0, 0x485610)),
					float,
					Args(cSPEditorHandleRotationRingPlaceholder*),
					Args(this)
				);
				res = (fEnd - fStart) * res * 0.333f * (1.0f / modelBoundsSize) * 0.85714287f + fStart;
			}
			return res;
		}
		return res;
	}
};

void FEReconEditorsHandles::AttachDetours()
{
	EditorRigblock_SetHandleScale::attach(Address(ModAPI::ChooseAddress(0x449930, 0x44a3a0)));
	cSPEditorHandleRotationRing_GetRadius::attach(Address(ModAPI::ChooseAddress(0x481cb0, 0x485610)));
	cSPEditorHandleRotationRing_GetHandleTime::attach(Address(ModAPI::ChooseAddress(0x481fc0, 0x485920)));
};

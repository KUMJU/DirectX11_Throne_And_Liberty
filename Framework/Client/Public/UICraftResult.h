#pragma once

#include "UIBase.h"

BEGIN(Engine)

class CVIInstancing;

END

BEGIN(Client)

class CUICraftResult : public CUIBase
{
	using Super = CUIBase;

public:
	CUICraftResult();
	virtual ~CUICraftResult();

public:
	virtual HRESULT Initialize(vector<string>& _strTextureTags, const string& _strMaskTextureTag, const string& _strNoiseTextureTag, CUIBase::UIDesc& _UIDesc, CVIInstancing::InstanceDesc& _InstanceDesc, vector<_float2>& _vUIPos, vector<_float2>& _vUISize);
	virtual void	PriorityTick(_float _fTimeDelta);
	virtual void	Tick(_float _fTimeDelta);
	virtual void	LateTick(_float _fTimeDelta);
	virtual HRESULT Render();

public: /* Get & Set Function */
	void SetCur(_float _fCur) { m_fCur = _fCur; }
	void ResetRatio();
	void RenderScript();

	void SetSuccessType(_int _iSuccessType) { m_iSuccessType = _iSuccessType; }
	_int GetSuccessType() { return m_iSuccessType; }

public:
	HRESULT RenderFont();

private:
	void InitUpdateTexture();
	void UpdateTexture();

private:
	HRESULT AddComponent(vector<string>& _strTextureTags, const string& _strMaskTextureTag, const string& _strNoiseTextureTag, CVIInstancing::InstanceDesc& _InstanceDesc, vector<_float2>& _vUIPos, vector<_float2>& _vUISize);

public:
	static shared_ptr<CUICraftResult> Create(vector<string>& _strTextureTags, const string& _strMaskTextureTag, const string& _strNoiseTextureTag, CUIBase::UIDesc& _UIDesc, CVIInstancing::InstanceDesc& _InstanceDesc, vector<_float2>& _vUIPos, vector<_float2>& _vUISize);

private:
	_float m_fTimeAcc{ 0.f };

private:
	_uint m_iInstanceRenderState{ 0 };

private:
	vector<_float2> m_vSlotUVRatio;

private:
	_float m_fRatio{ 0.f };
	_float m_fMax{ 1.f };
	_float m_fCur{ 0.f };

private:
	_bool m_bIsScriptEnabled{ false };

private:
	_int m_iSuccessType{ 0 };

};

END

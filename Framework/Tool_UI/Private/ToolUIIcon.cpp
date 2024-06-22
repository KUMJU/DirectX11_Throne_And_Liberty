#include "pch.h"
#include "ToolUIIcon.h"

#include "GameInstance.h"
#include "Texture.h"
#include "VIInstancePoint.h"

CToolUIIcon::CToolUIIcon()
{

}

CToolUIIcon::~CToolUIIcon()
{

}

HRESULT CToolUIIcon::Initialize(vector<string>& _strTextureTags, const string& _strMaskTextureTag, const string& _strNoiseTextureTag, CToolUIBase::ToolUIDesc& _ToolUIDesc, CVIInstancing::InstanceDesc& _InstanceDesc, _float2 _initPos)
{
    CTransform::TRANSFORM_DESC pGameObjectDesc;

    pGameObjectDesc.fSpeedPerSec = 10.f;
    pGameObjectDesc.fRotationPerSec = XMConvertToRadians(90.0f);

    if (FAILED(Super::Initialize(&pGameObjectDesc)))
        return E_FAIL;

    m_ToolUIDesc = _ToolUIDesc;
    m_InstanceDesc = _InstanceDesc;

    if (FAILED(AddComponent(_strTextureTags, _strMaskTextureTag, _strNoiseTextureTag, _InstanceDesc)))
        return E_FAIL;

    if (_initPos == _float2(0.f, 0.f))
    {
        m_fX = _InstanceDesc.vCenter.x;
        m_fY = _InstanceDesc.vCenter.y;
    }

    else
    {
        m_fX = _initPos.x;
        m_fY = _initPos.y;
    }
    
    m_fSizeX = _InstanceDesc.vSize.x;
    m_fSizeY = _InstanceDesc.fSizeY;

    /*Shader Info*/
    m_eShaderType = ESHADER_TYPE::ST_UI;
    m_iShaderPass = 0;
    m_ToolUIDesc.iShaderPassIdx = 0;

	return S_OK;
}

void CToolUIIcon::PriorityTick(_float _fDeltaTime)
{

}

void CToolUIIcon::Tick(_float _fDeltaTime)
{
    KeyInput();

    if (m_strTexKeys.size() > 1 && IconIsMouseOn())
        m_iTexIndex = 1;
    else
        m_iTexIndex = 0;

    if (!m_pUIParent.expired() && (m_pUIParent.lock()->GETPOS != m_vPrevParentPos) && (m_vPrevParentPos != _float3(FLT_MAX, FLT_MAX, FLT_MAX)))
    {
        _float3 moveDir = m_pUIParent.lock()->GETPOS - m_vPrevParentPos;
        moveDir.Normalize();
        _float moveDist = SimpleMath::Vector3::Distance(m_pUIParent.lock()->GETPOS, m_vPrevParentPos);

        _float3 newPos = _float3(m_fX, m_fY, 0.f) + (moveDir * moveDist);
        SetUIPos(newPos.x, newPos.y);

        m_BindWorldMat = m_pTransformCom->GetWorldMatrix();
    }

    else
        m_BindWorldMat = m_pTransformCom->GetWorldMatrix();

    if (!m_pUIChildren.empty())
    {
        for (auto& childPair : m_pUIChildren)
        {
            if (childPair.second.lock() != nullptr)
                childPair.second.lock()->Tick(_fDeltaTime);
        }
    }

    if (!m_pUIParent.expired())
        m_vPrevParentPos = m_pUIParent.lock()->GETPOS;
}

void CToolUIIcon::LateTick(_float _fDeltaTime)
{
    if (FAILED(GAMEINSTANCE->AddRenderGroup(CRenderer::RENDER_UI, shared_from_this())))
        return;

    if (!m_pUIChildren.empty())
    {
        for (auto& childPair : m_pUIChildren)
        {
            if (childPair.second.lock() != nullptr)
                childPair.second.lock()->LateTick(_fDeltaTime);
        }
    }
}

HRESULT CToolUIIcon::Render()
{
    if (FAILED(GAMEINSTANCE->ReadyShader(m_eShaderType, m_iShaderPass)))
        return E_FAIL;

    if (FAILED(GAMEINSTANCE->BindWVPMatrixOrthoGraphic(m_BindWorldMat)))
        return E_FAIL;

    //if (FAILED(GAMEINSTANCE->BindMatrix("g_RotationMatrix", &m_RotationMat)))
    //    return E_FAIL;

    if (FAILED(GAMEINSTANCE->BindRawValue("g_fDiscardAlpha", &m_ToolUIDesc.fDiscardAlpha, sizeof(_float))))
        return E_FAIL;

    switch (m_ToolUIDesc.m_eUITexType)
    {
    case TOOL_UI_TEX_D:
    {
        if (FAILED(GAMEINSTANCE->BindRawValue("g_iTexType", &m_ToolUIDesc.m_eUITexType, sizeof(ETOOL_UI_TEX_TYPE))))
            return E_FAIL;

        if (FAILED(GAMEINSTANCE->BindSRV("g_DiffuseTexture", m_strTexKeys[m_iTexIndex])))
            return E_FAIL;
    }
    break;
    case TOOL_UI_TEX_DM:
    {
        if (FAILED(GAMEINSTANCE->BindRawValue("g_iTexType", &m_ToolUIDesc.m_eUITexType, sizeof(ETOOL_UI_TEX_TYPE))))
            return E_FAIL;

        if (FAILED(GAMEINSTANCE->BindSRV("g_DiffuseTexture", m_strTexKeys[m_iTexIndex])))
            return E_FAIL;

        if (m_ToolUIDesc.m_isMasked)
        {
            if (FAILED(GAMEINSTANCE->BindSRV("g_MaskTexture", m_strMaskKey)))
                return E_FAIL;
        }

    }
    break;
    case TOOL_UI_TEX_M:
    {
        if (m_ToolUIDesc.m_isMasked)
        {
            if (FAILED(GAMEINSTANCE->BindRawValue("g_iTexType", &m_ToolUIDesc.m_eUITexType, sizeof(ETOOL_UI_TEX_TYPE))))
                return E_FAIL;

            if (FAILED(GAMEINSTANCE->BindSRV("g_MaskTexture", m_strMaskKey)))
                return E_FAIL;
        }
    }
    break;
    case TOOL_UI_TEX_NONE:
    {
        if (FAILED(GAMEINSTANCE->BindRawValue("g_iTexType", &m_ToolUIDesc.m_eUITexType, sizeof(ETOOL_UI_TEX_TYPE))))
            return E_FAIL;
    }
    break;
    case TOOL_UI_TEX_DMN:
    {
        if (FAILED(GAMEINSTANCE->BindRawValue("g_iTexType", &m_ToolUIDesc.m_eUITexType, sizeof(ETOOL_UI_TEX_TYPE))))
            return E_FAIL;

        if (FAILED(GAMEINSTANCE->BindSRV("g_DiffuseTexture", m_strTexKeys[m_iTexIndex])))
            return E_FAIL;

        if (m_ToolUIDesc.m_isMasked)
        {
            if (FAILED(GAMEINSTANCE->BindSRV("g_MaskTexture", m_strMaskKey)))
                return E_FAIL;
        }

        if (m_ToolUIDesc.isNoised)
        {
            if (FAILED(GAMEINSTANCE->BindSRV("g_NoiseTexture", m_strNoiseKey)))
                return E_FAIL;
        }
    }
    break;
    case TOOL_UI_TEX_DN:
    {
        if (FAILED(GAMEINSTANCE->BindRawValue("g_iTexType", &m_ToolUIDesc.m_eUITexType, sizeof(ETOOL_UI_TEX_TYPE))))
            return E_FAIL;

        if (FAILED(GAMEINSTANCE->BindSRV("g_DiffuseTexture", m_strTexKeys[m_iTexIndex])))
            return E_FAIL;

        if (m_ToolUIDesc.isNoised)
        {
            if (FAILED(GAMEINSTANCE->BindSRV("g_NoiseTexture", m_strNoiseKey)))
                return E_FAIL;
        }
    }
    break;
    case TOOL_UI_TEX_N:
    {
        if (FAILED(GAMEINSTANCE->BindRawValue("g_iTexType", &m_ToolUIDesc.m_eUITexType, sizeof(ETOOL_UI_TEX_TYPE))))
            return E_FAIL;

        if (m_ToolUIDesc.isNoised)
        {
            if (FAILED(GAMEINSTANCE->BindSRV("g_NoiseTexture", m_strNoiseKey)))
                return E_FAIL;
        }
    }
    break;
    case TOOL_UI_TEX_MN:
    {
        if (FAILED(GAMEINSTANCE->BindRawValue("g_iTexType", &m_ToolUIDesc.m_eUITexType, sizeof(ETOOL_UI_TEX_TYPE))))
            return E_FAIL;

        if (m_ToolUIDesc.m_isMasked)
        {
            if (FAILED(GAMEINSTANCE->BindSRV("g_MaskTexture", m_strMaskKey)))
                return E_FAIL;
        }

        if (m_ToolUIDesc.isNoised)
        {
            if (FAILED(GAMEINSTANCE->BindSRV("g_NoiseTexture", m_strNoiseKey)))
                return E_FAIL;
        }
    }
    break;
    }

    if (FAILED(GAMEINSTANCE->BeginShader()))
        return E_FAIL;

    if (FAILED(m_pBuffer->BindBuffers()))
        return E_FAIL;

    if (FAILED(m_pBuffer->Render()))
        return E_FAIL;

    return S_OK;
}

bool CToolUIIcon::IconIsMouseOn()
{
    POINT mousePt;
    ::GetCursorPos(&mousePt);
    ::ScreenToClient(g_hWnd, &mousePt);

    _float mousePosX = mousePt.x - g_iWinSizeX * 0.5f;
    _float mousePosY = -mousePt.y + g_iWinSizeY * 0.5f;

    if ((mousePosX >= (m_fX - m_fSizeX * 0.5f)) && (mousePosX <= (m_fX + m_fSizeX * 0.5f)) && (mousePosY >= (m_fY - m_fSizeY * 0.5f) &&
        mousePosY <= m_fY + m_fSizeY * 0.5f))
        return true;

    return false;
}

HRESULT CToolUIIcon::AddComponent(vector<string>& _strTextureTags, const string& _strMaskTextureTag, const string& _strNoiseTextureTag, CVIInstancing::InstanceDesc& _InstanceDesc)
{
    shared_ptr<CVIInstancePoint> pBufferCom = dynamic_pointer_cast<CVIInstancePoint>(GAMEINSTANCE->GetBuffer("Buffer_InstancePoint"));
    if (pBufferCom == nullptr)
        return E_FAIL;

    m_pBuffer = dynamic_pointer_cast<CVIInstancePoint>(pBufferCom->Clone(1, &_InstanceDesc));
    if (m_pBuffer == nullptr)
        return E_FAIL;

    m_strTexKeys = _strTextureTags;

    if (m_ToolUIDesc.m_isMasked)
        m_strMaskKey = _strMaskTextureTag;

    if (m_ToolUIDesc.isNoised)
        m_strNoiseKey = _strNoiseTextureTag;

	return S_OK;
}

shared_ptr<CToolUIIcon> CToolUIIcon::Create(vector<string>& _strTextureTags, const string& _strMaskTextureTag, const string& _strNoiseTextureTag, CToolUIBase::ToolUIDesc& _ToolUIDesc, CVIInstancing::InstanceDesc& _InstanceDesc, _float2 _initPos)
{
    shared_ptr<CToolUIIcon> pInstance = make_shared<CToolUIIcon>();

    if (FAILED(pInstance->Initialize(_strTextureTags, _strMaskTextureTag, _strNoiseTextureTag, _ToolUIDesc, _InstanceDesc, _initPos)))
    {
        MessageBox(nullptr, L"Initialize Failed", L"CToolUIIcon::Create", MB_OK);

        pInstance.reset();

        return nullptr;
    }

    return pInstance;
}

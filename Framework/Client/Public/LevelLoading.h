#pragma once

#include "Client_Defines.h"
#include "Level.h"

BEGIN(Client)

class CLevelLoading : public CLevel
{
public:
	CLevelLoading();
	virtual ~CLevelLoading();

public:
	virtual HRESULT Initialize(LEVEL _eNextLevel);
	virtual void Tick(_float _fTimeDelta) override;
	virtual void LateTick(_float _fTimeDelta) override;
	virtual HRESULT Render() override;

public:
	virtual HRESULT ReadyNextLevel(_uint _iNextLevel) override;

	void Free();

private:

	//shared_ptr<class CLoader> m_pLoader = { nullptr };

	class CLoader* m_pLoader = { nullptr };

	LEVEL m_eNextLevel = { LEVEL_END };

public:
	static shared_ptr<CLevelLoading> Create(LEVEL _eNextLevel);
};

END
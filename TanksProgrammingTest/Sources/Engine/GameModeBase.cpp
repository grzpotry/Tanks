#include "GameModeBase.h"

namespace EngineCore
{
    void GameModeBase::Initialize()
    {
        if (m_ActiveScene)
        {
            m_ActiveScene->Initialize(this);
        }
    }
    
    void GameModeBase::UnInitialize()
    {
        if (m_ActiveScene)
        {
            m_ActiveScene->UnInitialize();
        }
    }
    
    void GameModeBase::Update(float DeltaTime)
    {
        if (m_ActiveScene)
        {
            m_ActiveScene->Update(DeltaTime);
        }
    }
    
    void GameModeBase::Draw() const
    {
        if (m_ActiveScene)
        {
            m_ActiveScene->Draw();
        }
    }
}

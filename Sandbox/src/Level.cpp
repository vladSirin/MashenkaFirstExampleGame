﻿#include "Level.h"

#include "Random.h"
#include "Reward.h"
#include "glm/ext/matrix_transform.hpp"


using namespace Mashenka;

void Level::Init()
{
    m_Player.LoadAssets();
    m_Background.LoadAssets();
    Reset();
}

void Level::OnUpdate(Mashenka::TimeStep ts)
{
    // Update Player
    m_Player.OnUpdate(ts);

    // Check for collision
    if (ObstacleCollideTest())
    {
        GameOver();
    }
    if (RewardCollideTest())
        m_Player.RewardHit();

    // Generate reward check
    if (m_Time - m_LastRewardTime > m_RewardSpawnInterval)
    {
        m_LastRewardTime = m_Time;
        GenerateRewards();
    }

    // Generate obstacle check
    if (m_Time - m_LastObstacleSpawnTime > m_ObstacleSpawnInterval)
    {
        m_LastObstacleSpawnTime = m_Time;
        GenerateObstacles();
    }
}

void Level::OnRender()
{
    m_Background.OnRender(m_Player.GetPosition());

    for (auto& element : m_Obstacles)
    {
        element.Render();
    }
    for (auto& element : m_Rewards)
    {
        element.Render();
    }
    m_Player.OnRender();
}

void Level::OnImGuiRender()
{
    //Most UI will be handled in GameLayer
}

void Level::Reset()
{
    m_Player.Reset();
    m_Obstacles.clear();
    m_Rewards.clear();
    m_GameOver = false;
    m_Time = 0.0f;
    m_LastRewardTime = 0.0f;
}

bool Level::ObstacleCollideTest()
{
    for (auto& obstacle : m_Obstacles)
    {
        bool collision =  m_Player.GetAABB().Intersects(obstacle.GetAABB());

        if (collision)
        {
            MK_CORE_INFO("Obstcale Collision detected!");
            return true;
        }
    }
    return false;
}

bool Level::RewardCollideTest()
{
    bool collisionDetected = false;

    for (auto it = m_Rewards.begin(); it != m_Rewards.end();)
    {
        // Check if it collided with the player
        if (m_Player.GetAABB().Intersects(it->GetAABB()))
        {
            // Destroy the intersected reward
            it = m_Rewards.erase(it);
            collisionDetected = true;
        }
        else
        {
            ++it;
        }
    }

    return collisionDetected;
}

void Level::GenerateObstacles()
{
    // Calculate the spawn bounds
    float spawnMargin = 5.0f;
    glm::vec2 spawnTopLeft = {
        m_Player.GetPosition().x - m_Background.GetAspectRatio() / 2.0f - spawnMargin,
        m_Player.GetPosition().y + 1.0f + spawnMargin
    };
    glm::vec2 spawnBottomRight = {
        m_Player.GetPosition().x + m_Background.GetAspectRatio() / 2.0f + spawnMargin,
        m_Player.GetPosition().y - 1.0f - spawnMargin
    };
    glm::vec2 spawnPosition = {
        Random::Range(spawnTopLeft.x, spawnBottomRight.x),
        Random::Range(spawnTopLeft.y, spawnBottomRight.y)
    };

    CreateObstacle(spawnPosition, Random::Range(0.0f, 360.0f));

    // clean up obstacles that are too far away from the player position
    for (auto it = m_Obstacles.begin(); it != m_Obstacles.end();)
    {
        if (abs(it->GetPosition().x - m_Player.GetPosition().x) > m_Background.GetAspectRatio() * 3.0f)
        {
            it = m_Obstacles.erase(it);
        }
        else
        {
            ++it;
        }
    }
    
}

void Level::CreateObstacle(glm::vec2 position, float angle, glm::vec2 scale)
{
    auto obstacle = Obstacle(Obstacle::Type::Triangle, position, scale, angle);
    obstacle.Init();
    m_Obstacles.emplace_back(obstacle);
}

void Level::GenerateRewards()
{
    float distanceFromPlayer = Random::Float() * 2.5f + 1.0f; 
    float angle = Random::Float() * 2.0f * glm::pi<float>();

    glm::vec2 rewardPosition = m_Player.GetPosition() + glm::vec2(distanceFromPlayer * glm::cos(angle),
                                                                  distanceFromPlayer * glm::sin(angle));

    // check if too close to any obstacle, if so retry
    if (ObstacleCollideTest())
    {
        GenerateRewards();
        return;
    }
    
    CreateReward(rewardPosition);
    // clean up rewards that are too far away from the player position or colliding with the obstacle
    for (auto it = m_Rewards.begin(); it != m_Rewards.end();)
    {
        if (abs(it->GetPosition().x - m_Player.GetPosition().x) > m_Background.GetAspectRatio() * 3.0f)
        {
            it = m_Rewards.erase(it);
        }
        else
        {
            ++it;
        }
    }
}

void Level::CreateReward(glm::vec2 position, glm::vec2 scale)
{
    auto reward = Reward(position, scale);
    reward.Init();
    m_Rewards.emplace_back(reward);
}

void Level::GameOver()
{
    m_GameOver = true;
    MK_CORE_WARN("Game Over!");
}

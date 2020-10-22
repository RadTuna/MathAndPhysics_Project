
#include "Precompiled.h"
#include "SoftRenderer.h"

#include <random>

using namespace CK::DD;

// 그리드 그리기
void SoftRenderer::DrawGrid2D()
{
    auto& r = GetRenderer();
    const auto& g = Get2DGameEngine();

    // 그리드 색상
    LinearColor gridColor(LinearColor(0.8f, 0.8f, 0.8f, 0.3f));

    // 뷰의 영역 계산
    Vector2 viewPos = g.GetMainCamera().GetTransform().GetPosition();
    Vector2 extent = Vector2(_ScreenSize.X * 0.5f, _ScreenSize.Y * 0.5f);

    // 좌측 하단에서부터 격자 그리기
    int xGridCount = _ScreenSize.X / _Grid2DUnit;
    int yGridCount = _ScreenSize.Y / _Grid2DUnit;

    // 그리드가 시작되는 좌하단 좌표 값 계산
    Vector2 minPos = viewPos - extent;
    Vector2 minGridPos = Vector2(ceilf(minPos.X / (float)_Grid2DUnit), ceilf(minPos.Y / (float)_Grid2DUnit)) * (float)_Grid2DUnit;
    ScreenPoint gridBottomLeft = ScreenPoint::ToScreenCoordinate(_ScreenSize, minGridPos - viewPos);

    for (int ix = 0; ix < xGridCount; ++ix)
    {
        r.DrawFullVerticalLine(gridBottomLeft.X + ix * _Grid2DUnit, gridColor);
    }

    for (int iy = 0; iy < yGridCount; ++iy)
    {
        r.DrawFullHorizontalLine(gridBottomLeft.Y - iy * _Grid2DUnit, gridColor);
    }

    // 월드의 원점
    ScreenPoint worldOrigin = ScreenPoint::ToScreenCoordinate(_ScreenSize, -viewPos);
    r.DrawFullHorizontalLine(worldOrigin.Y, LinearColor::Red);
    r.DrawFullVerticalLine(worldOrigin.X, LinearColor::Green);
}

void SoftRenderer::Awake2D()
{
    static const size_t agentNum = 5;
    static const float playerSpeed = 100.f;
    static const float agentSpeed = 50.f;
    static const float agentHalfFov = 30.f;
    static const float agentDetectDistance = 200.f;

    static const float spawnHalfLimitX = 300.f;
    static const float spawnHalfLimitY = 200.f;

    std::random_device randDev;
    std::uniform_real_distribution<float> distX(-spawnHalfLimitX, spawnHalfLimitX);
    std::uniform_real_distribution<float> distY(-spawnHalfLimitY, spawnHalfLimitY);

    // 플레이어 생성 및 초기화.
    _Player = std::make_unique<Player>("Player");
    _Player->SetMovementSpeed(playerSpeed);
    _PlayerColor = LinearColor::Green;

    // 에이전트 생성.
    for (size_t i = 0; i < agentNum; ++i)
    {
        _Agents.emplace_back(std::make_unique<Agent>("Agent" + i));
    }

    // 에이전트 초기화.
    _AgentColor = LinearColor::Red;
    for (size_t i = 0; i < _Agents.size(); ++i)
    {
        Agent* agent = _Agents[i].get();

        agent->SetDetectProperties(agentHalfFov, agentDetectDistance);
        agent->SetMovementSpeed(agentSpeed);
        agent->SetMovementLimits(GetScreenSize().Y, GetScreenSize().X);

        const Vector2 initPosition(distX(randDev), distY(randDev));
        agent->GetTransform().SetPosition(initPosition);
    }
}

// 게임 로직
void SoftRenderer::Update2D(float InDeltaSeconds)
{
    auto& g = Get2DGameEngine();
    const InputManager& input = g.GetInputManager();

    const float axisX = input.GetAxis(InputAxis::XAxis);
    const float axisY = input.GetAxis(InputAxis::YAxis);

    _Player->SetInput(axisX, axisY);
    _Player->Update(InDeltaSeconds);

    for (size_t i = 0; i < _Agents.size(); ++i)
    {
        Agent* agent = _Agents[i].get();

        if (agent->IsDetect(*_Player))
        {
            agent->EnterDetect(*_Player);
        }

        agent->Update(InDeltaSeconds);
    }
}

// 렌더링 로직
void SoftRenderer::Render2D()
{
    auto& r = GetRenderer();
    const auto& g = Get2DGameEngine();

    static const LinearColor sightColor = LinearColor::Black;

    // 격자 그리기
    DrawGrid2D();

    // 플레이어와 에이전트를 위한 구체
    static float radius = 10.f;
    static std::vector<Vector2> sphere;
    if (sphere.empty())
    {
        for (float x = -radius; x <= radius; ++x)
        {
            for (float y = -radius; y <= radius; ++y)
            {
                Vector2 target(x, y);
                float sizeSquared = target.SizeSquared();
                float rr = radius * radius;
                if (sizeSquared < rr)
                {
                    sphere.push_back(target);
                }
            }
        }
    }

    // 플레이어를 구체로 렌더링.
    for (const Vector2& v : sphere)
    {
        r.DrawPoint(v + _Player->GetTransform().GetPosition(), _PlayerColor);
    }

    for (size_t i = 0; i < _Agents.size(); ++i)
    {
        Agent* agent = _Agents[i].get();

        // 에이전트의 시야각 선을 렌더링.
        const Vector2 currentPosition = agent->GetTransform().GetPosition();

        float cos;
        float sin;
        Math::GetSinCos(sin, cos, agent->GetTransform().GetRotation() + agent->GetDetectHalfFov());

        Vector2 sightEndPosition;
        sightEndPosition.X = currentPosition.X + cos * agent->GetDetectDistance();
        sightEndPosition.Y = currentPosition.Y + sin * agent->GetDetectDistance();

        r.DrawLine(currentPosition, sightEndPosition, sightColor);

        Math::GetSinCos(sin, cos, agent->GetTransform().GetRotation() - agent->GetDetectHalfFov());

        sightEndPosition.X = currentPosition.X + cos * agent->GetDetectDistance();
        sightEndPosition.Y = currentPosition.Y + sin * agent->GetDetectDistance();

        r.DrawLine(currentPosition, sightEndPosition, sightColor);

        if (!agent->IsDetected())
        {
            _AgentColor = LinearColor::Blue;
        }
        else
        {
            _AgentColor = LinearColor::Red;
        }

        // 에이전트를 구체로 렌더링.
        for (const Vector2& v : sphere)
        {
            r.DrawPoint(v + agent->GetTransform().GetPosition(), _AgentColor);
        }
    }
}


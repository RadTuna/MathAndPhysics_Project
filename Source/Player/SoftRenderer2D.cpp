
#include "Precompiled.h"
#include "SoftRenderer.h"

#include <random>

using namespace CK::DD;

// �׸��� �׸���
void SoftRenderer::DrawGrid2D()
{
    auto& r = GetRenderer();
    const auto& g = Get2DGameEngine();

    // �׸��� ����
    LinearColor gridColor(LinearColor(0.8f, 0.8f, 0.8f, 0.3f));

    // ���� ���� ���
    Vector2 viewPos = g.GetMainCamera().GetTransform().GetPosition();
    Vector2 extent = Vector2(_ScreenSize.X * 0.5f, _ScreenSize.Y * 0.5f);

    // ���� �ϴܿ������� ���� �׸���
    int xGridCount = _ScreenSize.X / _Grid2DUnit;
    int yGridCount = _ScreenSize.Y / _Grid2DUnit;

    // �׸��尡 ���۵Ǵ� ���ϴ� ��ǥ �� ���
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

    // ������ ����
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

    // �÷��̾� ���� �� �ʱ�ȭ.
    _Player = std::make_unique<Player>("Player");
    _Player->SetMovementSpeed(playerSpeed);
    _PlayerColor = LinearColor::Green;

    // ������Ʈ ����.
    for (size_t i = 0; i < agentNum; ++i)
    {
        _Agents.emplace_back(std::make_unique<Agent>("Agent" + i));
    }

    // ������Ʈ �ʱ�ȭ.
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

// ���� ����
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

// ������ ����
void SoftRenderer::Render2D()
{
    auto& r = GetRenderer();
    const auto& g = Get2DGameEngine();

    static const LinearColor sightColor = LinearColor::Black;

    // ���� �׸���
    DrawGrid2D();

    // �÷��̾�� ������Ʈ�� ���� ��ü
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

    // �÷��̾ ��ü�� ������.
    for (const Vector2& v : sphere)
    {
        r.DrawPoint(v + _Player->GetTransform().GetPosition(), _PlayerColor);
    }

    for (size_t i = 0; i < _Agents.size(); ++i)
    {
        Agent* agent = _Agents[i].get();

        // ������Ʈ�� �þ߰� ���� ������.
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

        // ������Ʈ�� ��ü�� ������.
        for (const Vector2& v : sphere)
        {
            r.DrawPoint(v + agent->GetTransform().GetPosition(), _AgentColor);
        }
    }
}


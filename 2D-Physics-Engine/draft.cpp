// Library
#include <iostream>
#include <SFML/Graphics.hpp>
#include <SFML/Audio.hpp>
#include <vector>
#include <string>
#include <algorithm>

class Circle
{
private:

    const float speed = 50;
    const float mass = 0.5;
    const float time = 30;

public:

    sf::CircleShape circle;
    sf::Vector2f velocity;

    Circle(float radius, sf::Vector2f position, sf::Vector2f initialVelocity)
    {
        circle.setRadius(radius);
        circle.setPosition(position);
        circle.setOrigin(radius, radius);
        this->velocity = initialVelocity;
    }

    sf::Vector2f GetPosition() const
    {
        return circle.getPosition();
    }

    sf::Vector2f GetVelocity() const
    {
        return velocity;
    }

    float GetRadius() const
    {
        return circle.getRadius();
    }

    float GetMass() const
    {
        return mass;
    }

    void SetVelocity(sf::Vector2f newVelocity)
    {
        velocity = newVelocity;
    }

    void UpdatePosition(float dt)
    {
        circle.move(velocity * speed * dt / time);
    }

    sf::Color SetColor(sf::Color color)
    {
        circle.setFillColor(color);
        return color;
    }

    void DrawCircle(sf::RenderWindow& window) const
    {
        window.draw(circle);
    }

    void DrawLine(sf::RenderWindow& window, sf::Vector2f start, sf::Vector2f end, sf::Color color) const
    {
        sf::Vertex line[] =
        {
            sf::Vertex(start, color),
            sf::Vertex(end, color)
        };
        window.draw(line, 2, sf::Lines);
    }

};

class CircleCollision
{
public:
    float GetDistance(Circle A, Circle B);
    float TotalRadius(Circle A, Circle B);
    bool WindowCollision(Circle& x, sf::RenderWindow& window) const;
    bool ElasticCollision(CircleCollision& c, Circle& a, Circle& b, float restitution, sf::RenderWindow& window) const;
};

float CircleCollision::GetDistance(Circle A, Circle B)
{
    float distance = sqrt(pow(B.GetPosition().x - A.GetPosition().x, 2) + pow(B.GetPosition().y - A.GetPosition().y, 2));

    return distance;
}

float CircleCollision::TotalRadius(Circle A, Circle B)
{
    float totalRadius = A.GetRadius() + B.GetRadius();
    return totalRadius;
}

bool CircleCollision::WindowCollision(Circle& i, sf::RenderWindow& window) const
{
    sf::Vector2f position = i.GetPosition();
    float radius = i.GetRadius();
    sf::Vector2f velocity = i.GetVelocity();

    bool collision = false;

    if (position.x - radius < 0 || position.x + radius > window.getSize().x)
    {
        velocity.x = -velocity.x;
        collision = true;
    }
    if (position.y - radius < 0 || position.y + radius > window.getSize().y)
    {
        velocity.y = -velocity.y;
        collision = true;
    }

    if (collision)
    {
        i.SetVelocity(velocity);
    }

    return collision;
}

bool CircleCollision::ElasticCollision(CircleCollision& c, Circle& a, Circle& b, float restitution, sf::RenderWindow& window) const
{
    float distance = c.GetDistance(a, b);
    float totalRadius = c.TotalRadius(a, b);

    /* // Calculate positions of circles a and b
     sf::Vector2f positionA = a.GetPosition();
     sf::Vector2f positionB = b.GetPosition();

     // Draw the line representing the distance between circles a and b
     a.DrawLine(window, positionA, positionB, sf::Color::Yellow);*/

    if (distance < totalRadius)
    {


        // Normal Vector
        sf::Vector2f normal = (b.GetPosition() - a.GetPosition()) / distance;

        // Velocities along the normal
        float v1Normal = a.GetVelocity().x * normal.x + a.GetVelocity().y * normal.y;
        float v2Normal = b.GetVelocity().x * normal.x + b.GetVelocity().y * normal.y;

        // Using the provided formulas to compute new velocities
        float v1PrimeNormal = (a.GetMass() * v1Normal + b.GetMass() * v2Normal - b.GetMass() * (v1Normal - v2Normal) * restitution) / (a.GetMass() + b.GetMass());
        float v2PrimeNormal = (a.GetMass() * v1Normal + b.GetMass() * v2Normal - a.GetMass() * (v2Normal - v1Normal) * restitution) / (a.GetMass() + b.GetMass());

        // Update velocities along the normal
        sf::Vector2f v1Prime = a.GetVelocity() + (v1PrimeNormal - v1Normal) * normal;
        sf::Vector2f v2Prime = b.GetVelocity() + (v2PrimeNormal - v2Normal) * normal;

        // Set new velocities
        a.SetVelocity(v1Prime);
        b.SetVelocity(v2Prime);

        sf::Vector2f primeEnd = a.GetPosition() + v1Prime * 100.0f;
        a.DrawLine(window, a.GetPosition(), primeEnd, sf::Color::White);

        // Draw the normal vector for circle a
        sf::Vector2f normalEnd = a.GetPosition() + normal * 100.0f; // Scale the normal for visibility
        a.DrawLine(window, a.GetPosition(), normalEnd, sf::Color::Blue);

        return true;
    }

    return false;

}

int main()
{

    // Settings
    sf::ContextSettings settings;
    settings.antialiasingLevel = 8;

    // Window
    sf::RenderWindow window;

    window.create(sf::VideoMode(800, 800), "Click Game", sf::Style::Titlebar | sf::Style::Close, settings);
    window.setVerticalSyncEnabled(true);
    window.setFramerateLimit(60);

    ///----------------------------------------------------------------------------------------------------

    int totalBalls = 20;

    std::vector<Circle> balls;
    for (int i = 0; i < totalBalls; i++)
    {
        float radius = static_cast<float>(rand() % (30 - 10 + 1) + 10);
        sf::Vector2f position;
        position.x = static_cast<float>(rand() % window.getSize().x);
        position.y = static_cast<float>(rand() % window.getSize().y);
        sf::Vector2f velocity;
        velocity.x = rand() % (2 - 1 + 1) + 1;
        velocity.y = rand() % (2 - 1 + 1) + 1;
        balls.emplace_back(radius, position, velocity);

    }


    while (window.isOpen())
    {

        const int numSteps = 20;
        const float dt = 1.0f / numSteps;

        for (int step = 0; step < numSteps; ++step)
        {
            for (auto& ball : balls)
            {
                ball.UpdatePosition(dt);
            }
        }

        CircleCollision collision;
        window.clear(sf::Color(0, 0, 0));

        for (auto& ball1 : balls)
        {
            for (auto& ball2 : balls)
            {
                if (&ball1 != &ball2)
                {
                    collision.ElasticCollision(collision, ball1, ball2, 1.0f, window);
                };
            }
        }

        // Circle Collision windows
        for (auto& ball : balls)
        {
            if (collision.WindowCollision(ball, window))
            {
                std::cout << "Window Detected" << std::endl;
            }
        }

        sf::Event event;
        while (window.pollEvent(event))
        {
            if (event.type == sf::Event::Closed)
                window.close();
        }


        std::srand(std::time(nullptr));

        for (auto& ball : balls)
        {
            sf::Color randomColor(std::rand() % 256, std::rand() % 256, std::rand() % 256);
            ball.SetColor(randomColor);
            ball.DrawCircle(window);
        }
        window.display();
    }
    return 0;
}
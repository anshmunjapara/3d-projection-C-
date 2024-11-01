#include <SFML/Graphics.hpp>
#include <vector>
#include <cmath>
#include <iostream>
using namespace std;

class Button
{
public:
    Button(float x, float y, float width, float height, const sf::String &text, sf::Font &font, sf::Color bgColor, sf::Color textColor)
    {
        // Set up the button shape
        buttonShape.setSize({width, height});
        buttonShape.setOrigin({width / 2, height / 2});
        buttonShape.setPosition(x, y);
        buttonShape.setFillColor(bgColor);

        // Set up the button text
        buttonText.setFont(font);
        buttonText.setString(text);
        buttonText.setCharacterSize(24);
        buttonText.setFillColor(textColor);

        // Center text within the button
        sf::FloatRect textBounds = buttonText.getLocalBounds();
        buttonText.setOrigin({textBounds.width / 2, (textBounds.height / 2) + textBounds.top});
        buttonText.setPosition(x, y);
    }

    void setFillColor(const sf::Color &color)
    {
        buttonShape.setFillColor(color);
    }

    void setTextColor(const sf::Color &color)
    {
        buttonText.setFillColor(color);
    }

    void setText(const sf::String &text)
    {
        buttonText.setString(text);
        sf::FloatRect textBounds = buttonText.getLocalBounds();
        buttonText.setOrigin({textBounds.width / 2, (textBounds.height / 2) + textBounds.top});
        buttonText.setPosition(buttonShape.getPosition());
    }
    // Draw the button
    void draw(sf::RenderWindow &window)
    {
        window.draw(buttonShape);
        window.draw(buttonText);
    }

    // Check if the button is hovered
    bool isHovered(sf::RenderWindow &window) const
    {
        sf::Vector2i mousePos = sf::Mouse::getPosition(window);
        sf::FloatRect bounds = buttonShape.getGlobalBounds();
        return bounds.contains(static_cast<sf::Vector2f>(mousePos));
    }

    // Check if the button is clicked
    bool isClicked(sf::RenderWindow &window) const
    {
        return isHovered(window) && sf::Mouse::isButtonPressed(sf::Mouse::Left);
    }

private:
    sf::RectangleShape buttonShape;
    sf::Text buttonText;
};

vector<float> multiply(const vector<vector<float>> &mat, const vector<float> &vec);
void drawLines(sf::RenderWindow &window, const vector<vector<float>> &points);

int main()
{

    sf::RenderWindow window(sf::VideoMode(900, 900), "3d Projection");
    window.setFramerateLimit(60);

    sf::Font font;
    if (!font.loadFromFile("arial.ttf"))
    {
        std::cerr << "Failed to load font!\n";
        return -1;
    }
    // tranformation matrix
    vector<std::vector<float>> transformMatrix = {{1, 0, 0}, {0, 1, 0}, {0, 0, 0}};

    // cube vertices
    vector<vector<float>> vertices;
    // Front vertices
    vertices.push_back({1, 1, 1});
    vertices.push_back({1, -1, 1});
    vertices.push_back({-1, -1, 1});
    vertices.push_back({-1, 1, 1});
    // Back vertices
    vertices.push_back({-1, 1, -1});
    vertices.push_back({1, 1, -1});
    vertices.push_back({1, -1, -1});
    vertices.push_back({-1, -1, -1});

    // default angles
    float angle_x = 0;
    float angle_y = 0;
    float angle_z = 0;

    int scale = 100;
    int offset = 450;

    bool autoMove = true;

    sf::Vector2i windowCenter(window.getSize().x / 2, window.getSize().y / 2);
    sf::Mouse::setPosition(windowCenter, window);

    Button btn(200, 100, 200, 50, "Auto", font, sf::Color::White, sf::Color::Black);

    while (window.isOpen())
    {
        sf::Event event;
        while (window.pollEvent(event))
        {
            if (event.type == sf::Event::Closed)
                window.close();
            else if (event.type == sf::Event::MouseMoved)
            {
                if (btn.isHovered(window))
                {
                    btn.setFillColor(sf::Color::Cyan);
                    btn.setTextColor(sf::Color::Black);
                }
                else
                {
                    btn.setFillColor(sf::Color::White);
                    btn.setTextColor(sf::Color::Black);
                }
            }
            else if (event.type == sf::Event::MouseButtonPressed)
            {
                if (btn.isClicked(window))
                {
                    if (autoMove)
                    {
                        autoMove = false;
                        btn.setText("Mouse");
                    }
                    else
                    {
                        autoMove = true;
                        btn.setText("Auto");
                    }

                    angle_x = 0;
                    angle_y = 0;
                    angle_z = 0;
                }
            }
        }

        // get mouse pos and set angle accordingly
        if (autoMove)
        {
            angle_x += 0.02;
            angle_y += 0.02;
            angle_z += 0.02;
        }
        else
        {
            sf::Vector2i mousePosition = sf::Mouse::getPosition(window);
            sf::Vector2i mouseDelta = mousePosition - windowCenter;
            angle_x = -mouseDelta.y * 0.005f;
            angle_y = mouseDelta.x * 0.005f;
        }

        // rotation matrix for x, y and z
        vector<vector<float>> rotationMatrixX = {{1, 0, 0},
                                                 {0, cos(angle_x), -sin(angle_x)},
                                                 {0, sin(angle_x), cos(angle_x)}};

        vector<vector<float>> rotationMatrixY = {{cos(angle_y), 0, sin(angle_y)},
                                                 {0, 1, 0},
                                                 {-sin(angle_y), 0, cos(angle_y)}};

        vector<vector<float>> rotationMatrixZ = {{cos(angle_z), -sin(angle_z), 0},
                                                 {sin(angle_z), cos(angle_z), 0},
                                                 {0, 0, 1}};

        window.clear();

        vector<vector<float>> points;
        for (int i = 0; i < vertices.size(); i++)
        {
            // doing rotation and then transformation for each vertex
            vector<float> vertex = vertices[i];
            vector<float> rotated_x = multiply(rotationMatrixX, vertex);
            vector<float> rotated_y = multiply(rotationMatrixY, rotated_x);
            vector<float> rotated_z = multiply(rotationMatrixZ, rotated_y);
            vector<float> point = multiply(transformMatrix, rotated_z);

            float x = (point[0] * scale) + offset;
            float y = (point[1] * scale) + offset;
            points.push_back({x, y});
        }

        drawLines(window, points);
        btn.draw(window);

        points.clear();
        window.display();
    }

    return 0;
}

vector<float> multiply(const vector<vector<float>> &mat, const vector<float> &vec)
{

    vector<float> result(3, 0.0f);

    // Perform matrix-vector multiplication
    for (int i = 0; i < 3; ++i)
    {
        for (int j = 0; j < 3; ++j)
        {
            result[i] += mat[i][j] * vec[j];
        }
    }

    return result;
}

void drawLines(sf::RenderWindow &window, const vector<vector<float>> &points)
{
    const vector<pair<int, int>> edges = {
        {0, 1}, {1, 2}, {2, 3}, {3, 0}, {4, 5}, {5, 6}, {6, 7}, {7, 4}, {0, 5}, {1, 6}, {2, 7}, {3, 4}};

    for (const auto &edge : edges)
    {
        sf::Vertex line[] = {
            sf::Vertex(sf::Vector2f(points[edge.first][0],
                                    points[edge.first][1])),
            sf::Vertex(sf::Vector2f(points[edge.second][0],
                                    points[edge.second][1]))};
        window.draw(line, 2, sf::Lines);
    }
}

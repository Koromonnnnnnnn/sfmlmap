#include <SFML/Graphics.hpp> // SFML graphics library
#include <cmath>             // For math functions like min, max
#include <algorithm>         // For std::min and std::max
#include <iostream>          // For debugging output

using namespace sf;          
using namespace std;         

int main() {

    RenderWindow window(VideoMode(800, 600), "Camera Following Player");
    window.setFramerateLimit(60); // Limit FPS to 60 to avoid excessive CPU usage

    // Load map texture from file
    // https://www.sfml-dev.org/documentation/2.5.1/classsf_1_1Texture.php
    Texture mapTexture;
    mapTexture.loadFromFile("map.png"); // Ensure "map.png" exists in the working directory
    Sprite map(mapTexture); // Create a sprite to represent the map

    // Create the player as a circle shape
    // https://www.sfml-dev.org/documentation/2.5.1/classsf_1_1CircleShape.php
    CircleShape player(20.f); // Player is a circle with a radius of 20 pixels
    player.setFillColor(Color::Green); // Color the player green
    player.setOrigin(20.f, 20.f);      // Set origin to center (important for smooth movement)
    player.setPosition(400.f, 300.f); // Initial position of the player

    // Create a view (camera) to follow the player
    // https://www.sfml-dev.org/documentation/2.5.1/classsf_1_1View.php
    View view(player.getPosition(), Vector2f(800, 600)); // Center view on player, size matches window

    // RenderTexture for the light overlay
    // https://www.sfml-dev.org/documentation/2.5.1/classsf_1_1RenderTexture.php
    RenderTexture lightTexture;
    lightTexture.create(800, 600); // Match window size for the light effect

    // Create a circle to simulate the light around the player
    CircleShape lightEffect(200.f); // Circle radius of 200 pixels
    lightEffect.setFillColor(Color(255, 255, 255, 255)); // Fully white circle
    lightEffect.setOrigin(200.f, 200.f); // Center the circle's origin for proper alignment

    // Dark overlay for the map (used for "darkness")
    RectangleShape darkness(Vector2f(800, 600));
    darkness.setFillColor(Color(0, 0, 0, 200)); // Semi-transparent black

    // Get the size of the map for boundary checks
    Vector2f mapSize(mapTexture.getSize().x, mapTexture.getSize().y);

    while (window.isOpen()) {
        // Event handling: Check for window close or other events
        Event event;
        while (window.pollEvent(event)) {
            if (event.type == Event::Closed)
                window.close(); // Close the window
        }

        // Handle player movement using WASD keys
        float speed = 200.f; // Player speed in pixels per second
        Vector2f movement(0.f, 0.f);
        if (Keyboard::isKeyPressed(Keyboard::W)) movement.y -= speed; // Move up
        if (Keyboard::isKeyPressed(Keyboard::S)) movement.y += speed; // Move down
        if (Keyboard::isKeyPressed(Keyboard::A)) movement.x -= speed; // Move left
        if (Keyboard::isKeyPressed(Keyboard::D)) movement.x += speed; // Move right

        // Update player position based on movement and time
        float deltaTime = 1.f / 60.f; // Fixed time step for smooth movement
        player.move(movement * deltaTime);

        // Clamp player position to map boundaries
        // Prevents the player from moving outside the visible map
        // https://en.cppreference.com/w/cpp/algorithm/min
        Vector2f playerPos = player.getPosition();
        playerPos.x = max(player.getRadius(), min(mapSize.x - player.getRadius(), playerPos.x));
        playerPos.y = max(player.getRadius(), min(mapSize.y - player.getRadius(), playerPos.y));
        player.setPosition(playerPos); // Update player's clamped position

        // Update view center to match player's position
        Vector2f viewCenter = player.getPosition();
        float halfWidth = view.getSize().x / 2; // Half of the view's width
        float halfHeight = view.getSize().y / 2; // Half of the view's height

        // Clamp view center to keep it within map boundaries
        viewCenter.x = max(halfWidth, min(mapSize.x - halfWidth, viewCenter.x));
        viewCenter.y = max(halfHeight, min(mapSize.y - halfHeight, viewCenter.y));
        view.setCenter(viewCenter); // Update view's position

        // Synchronize light effect with player's position
        lightEffect.setPosition(player.getPosition());

        // Render the scene
        window.clear(); // Clear the screen

        // Set the view for the map and player
        window.setView(view);
        window.draw(map);    // Draw the map
        window.draw(player); // Draw the player

        // Render light effect to the light texture
        lightTexture.clear(Color(0, 0, 0, 255)); // Clear with black
        lightTexture.setView(View(view));        // Synchronize light texture view with main view
        lightTexture.draw(lightEffect);          // Draw the light effect
        lightTexture.display();                  // Finalize the light texture

        // Overlay the light effect on the screen
        Sprite lightSprite(lightTexture.getTexture()); // Get the texture as a sprite
        lightSprite.setPosition(view.getCenter() - (view.getSize() / 2.f)); // Align to view
        window.draw(lightSprite, BlendMultiply); // Use blending mode for proper effect

        window.display(); // Display everything on the screen
    }

    return 0;
}

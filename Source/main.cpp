#define GATEWARE_ENABLE_CORE // Enables Gateware basics
#define GATEWARE_ENABLE_SYSTEM
#include "./DRAW/VulkanRenderer.cpp"
#define GATEWARE_ENABLE_GRAPHICS // Enables Vulkan Surface
#include "../gateware-main/Gateware.h"
#include "../entt-3.13.1/single_include/entt/entt.hpp"
#include <iostream>

int main() {
    // Initialize Gateware Window
    GW::SYSTEM::GWindow win;
    if (G_PASS(win.Create(0, 0, 800, 600, GW::SYSTEM::GWindowStyle::WINDOWEDBORDERED))) {
        win.SetWindowName("Vulkan 3D Shooter - Phase 1");

        entt::registry world; // Create entt registry for world data
        
        // Simple Loop to keep window open
        bool running = true;

        // Creating event responder to listen for window events
        GW::CORE::GEventResponder responder;
        responder.Create([&](const GW::GEvent& e) {
            GW::SYSTEM::GWindow::Events eventType;
            GW::SYSTEM::GWindow::EVENT_DATA eventData;

            // Checking if event is window event and if it is destroy event
            if (G_PASS(e.Read(eventType, eventData))) {
                if (eventType == GW::SYSTEM::GWindow::Events::DESTROY) {
                    running = false; // Exit loop on window close
                }
            }
        });

        // Register responder
        win.Register(responder);

        // Main Game Loop
        while (running) {

            /*
				Main game logic would go here. For this phase, we are just testing the window and event system, so we will keep it simple and just process window events.
            */

			win.ProcessWindowEvents(); // Process window events to trigger our responder
        }
    }
}
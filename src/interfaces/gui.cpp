#include "gui.h"

void gui::setup()
{
    ofSetBackgroundColor(16, 16, 16);
    _gui.setup(nullptr, false);
}

void gui::exit()
{
}

void gui::update()
{
}

void gui::draw()
{
    _gui.begin();
    ImGui::SetNextWindowPos(ImVec2(0, 0), ImGuiCond_FirstUseEver);
    ImGui::SetNextWindowSize(ImVec2(550, 680), ImGuiCond_FirstUseEver);
    ImGui::ShowDemoWindow();
    _gui.end();
    _gui.draw();
}

void gui::keyPressed(int key)
{
}
void gui::keyReleased(int key) {}
void gui::mouseMoved(int x, int y) {}
void gui::mouseDragged(int x, int y, int button) {}
void gui::mousePressed(int x, int y, int button) {}
void gui::mouseReleased(int x, int y, int button) {}
void gui::mouseEntered(int x, int y) {}
void gui::mouseExited(int x, int y) {}
void gui::windowResized(int w, int h) {}
void gui::dragEvent(ofDragInfo dragInfo) {}
void gui::gotMessage(ofMessage msg) {}
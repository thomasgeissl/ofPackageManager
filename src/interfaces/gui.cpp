#include "gui.h"

gui::gui(ofPackageManager app) : ofBaseApp(), _app(app),
             _welcomeState(ofxState::create("welcome")),
             _newState(ofxState::create("new")),
             _updateState(ofxState::create("update")),
             _updateMultipleState(ofxState::create("updateMultiple"))
{
    _stateMachine.setInitialState(_welcomeState);
    _stateMachine.addTransition(_welcomeState, "new", _newState);
    _stateMachine.addTransition(_welcomeState, "update", _updateState);
    _stateMachine.start();
}
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
    auto menuBarSize = drawMenuGui();
    ImGui::SetNextWindowPos(ImVec2(0, menuBarSize.y), ImGuiCond_FirstUseEver);
    ImGui::SetNextWindowSize(ImVec2(ofGetWidth(), ofGetHeight() - menuBarSize.y));
    // ImGui::ShowDemoWindow();
    bool open = true;
    if (ImGui::Begin("Main Window", &open, ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoDecoration))
    {

        if (_stateMachine.isCurrentState(_welcomeState))
        {

            ImGui::Button("install addons");
            ImGui::SameLine();
            if(ImGui::Button("new project")){
                _stateMachine.transit("new");
            }
            ImGui::SameLine();
            if(ImGui::Button("update project")){
                _stateMachine.transit("update");
            }
            ImGui::SameLine();
            ImGui::Button("update multiple projects");
            drawRecentProjects();
        }
        else if(_stateMachine.isCurrentState(_newState)){
            ImGui::Text("TODO: new");
            if(ImGui::Button("path")){
                auto result = ofSystemLoadDialog("where should your project live", true, "~");
            }
            static char name[128] = "Hello, world!";
            ImGui::InputText("input text", name, IM_ARRAYSIZE(name));
            if(ImGui::Button("next")){
                auto dataPath = ofFilePath::getAbsolutePath(".");
                ofLogNotice() << _app.findOfPathOutwardly(dataPath, 6);
                ofLogNotice() << _app.getOfPath();
                ofLogNotice() << ofFilePath::getAbsolutePath(".");
                ofLogNotice() << _app.getCwdPath();
            }
        }
        else if(_stateMachine.isCurrentState(_updateState)){
            ImGui::Text("TODO: update");
        }
        else if(_stateMachine.isCurrentState(_updateMultipleState)){
            ImGui::Text("TODO: update multiple");
        }

        ImGui::End();
    }

    _gui.end();
    _gui.draw();
}

ImVec2 gui::drawMenuGui()
{
    auto size = ImVec2(-1, -1);
    if (ImGui::BeginMainMenuBar())
    {
        if (ImGui::BeginMenu("ofPackageManager"))
        {
            if (ImGui::MenuItem("About"))
            {
            }
            if (ImGui::MenuItem("Preferences"))
            {
            }
            if (ImGui::MenuItem("Quit"))
            {
                ofExit();
            }
            ImGui::EndMenu();
        }
        if (ImGui::BeginMenu("File"))
        {
            if (ImGui::MenuItem("New"))
            {
                //Do something
            }
            if (ImGui::MenuItem("Open"))
            {
                //Do something
            }
            ImGui::EndMenu();
        }
        if (ImGui::BeginMenu("View"))
        {
            // if (ImGui::MenuItem("Fullscreen", NULL, _fullscreen))
            // {
            //     // _fullscreen = !_fullscreen;
            //     ofSetFullscreen(_fullscreen);
            // }
            ImGui::EndMenu();
        }
        size = ImGui::GetWindowSize();
        ImGui::EndMainMenuBar();
    }
    return size;
}
void gui::drawRecentProjects()
{
    if (ImGui::Begin("recent projects"))
    {
        ImGui::Text("recent projects");
        ImGui::End();
    }
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
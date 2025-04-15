#include <fstream>
#include <string>
#include <vector>
#include "headers/raylib.h"

#define RAYGUI_IMPLEMENTATION
#include "headers/raygui.h"

using namespace std;

void ReloadTasks(fstream &file, const char *filename, vector<string>& tasks)
{
    tasks.clear();
    file.open(filename, ios::in);
    if(file.is_open())
    {
        string line;
        while (getline(file, line))
        { 
            if(!line.empty())
            {
                tasks.push_back(line);
            }
        }
        
        file.close();
    }
}

void DrawTask(Rectangle bounds, string text, int fontSize, int index, fstream &file, const char *filename, vector<string>& tasks)
{
    const Vector2 delIconSize = {24, 24};
    DrawRectangle(bounds.x, bounds.y, bounds.width, bounds.height, Fade(LIGHTGRAY, 0.3f));
    DrawRectangleLines(bounds.x, bounds.y, bounds.width, bounds.height, Fade(DARKGRAY, 0.3f));
    DrawText(text.c_str(), bounds.x + 4,  bounds.y + bounds.height / 2 - MeasureTextEx(GetFontDefault(), text.c_str(), fontSize, 0).y / 2, fontSize, BLACK);
    
    Rectangle deleteBtn = {bounds.x + bounds.width - delIconSize.x - 3, bounds.y + (bounds.height - delIconSize.y) / 2, delIconSize.x, delIconSize.y};
    if (GuiButton(deleteBtn, "#143#")) // Delete icon
    {
        tasks.erase(tasks.begin() + index);
        std::ofstream outFile(filename); // Overwrites by default
        for (const std::string& task : tasks)
        {
            outFile << task << '\n';
        }
        outFile.close();
        ReloadTasks(file, filename, tasks);
    }
}

int main()
{
    Vector2 screenSize = {800, 480};
    
    SetConfigFlags(ConfigFlags::FLAG_WINDOW_RESIZABLE);
    InitWindow(screenSize.x, screenSize.y, "To Simply Do");
    SetTargetFPS(60);

    //-GENERIC
    const int defaultBuffer = 15;
    bool showMessageBox = false;
    fstream toDoFile;
    const char *toDoFilename = "todo.txt";
    
    //-TASKS
    vector<string> tasks;
    Vector2 taskSize = {screenSize.x - defaultBuffer * 2, defaultBuffer * 2};
    const int taskFontSize = 24;
    
    //-TITLE
    const int titleFontSize = 20;
    const char *titleText = "TO SIMPLY DO";
    Vector2 titlePos = {screenSize.x / 2 - MeasureText(titleText, titleFontSize) / 2, defaultBuffer};
    
    //-ADD A TASK
    
    //BUTTON
    const Vector2 addButtonSize = {120, 30};
    Vector2 addButtonPos = {screenSize.x - addButtonSize.x - defaultBuffer, (float) defaultBuffer};
    const char *addButtonText = "#65#Add New Task";
    
    //POPUP
    const int maxTextLength = 32;
    char inputText[maxTextLength] = "";       // This is where input will be stored
    bool secretActive = true;     // Whether to mask input (e.g., like password)
    const Vector2 addPopupSize = {250, 130};
    Vector2 addPopupPos = {(screenSize.x / 2) - (addPopupSize.x / 2), (screenSize.y / 2) - (addPopupSize.y / 2)};
    
    ReloadTasks(toDoFile, toDoFilename, tasks);
    
    while (!WindowShouldClose())
    {
        screenSize = {(float) GetScreenWidth(), (float) GetScreenHeight()};
        
        taskSize = {screenSize.x - defaultBuffer * 2, defaultBuffer * 2};
        titlePos = {screenSize.x / 2 - MeasureText(titleText, titleFontSize) / 2, defaultBuffer};
        addButtonPos = {screenSize.x - addButtonSize.x - defaultBuffer, (float) defaultBuffer};
        addPopupPos = {(screenSize.x / 2) - (addPopupSize.x / 2), (screenSize.y / 2) - (addPopupSize.y / 2)};
        
        BeginDrawing();
            ClearBackground(GetColor(GuiGetStyle(DEFAULT, BACKGROUND_COLOR)));
            
            int taskPosition = 0;
            for(string task : tasks)
            {
                //DrawText(task.c_str(), defaultBuffer, defaultBuffer + taskPosition * 16, 16, BLACK);
                DrawTask({defaultBuffer, (float) defaultBuffer * 4 + taskPosition * 32, taskSize.x, taskSize.y}, task, taskFontSize, taskPosition, toDoFile, toDoFilename, tasks);
                taskPosition++;
            }
            
            DrawText(titleText, titlePos.x, titlePos.y, titleFontSize, BLACK);

            if (GuiButton((Rectangle){addButtonPos.x, addButtonPos.y, addButtonSize.x, addButtonSize.y}, addButtonText)) 
            {
                showMessageBox = true;
            }

            if (showMessageBox)
            {
                int result = GuiTextInputBox((Rectangle){addPopupPos.x, addPopupPos.y, addPopupSize.x, addPopupSize.y},
                    "#191#Add New Task", "Please enter a name:", "Set;Reset",
                    inputText, maxTextLength, &secretActive);

                if(result == 0) showMessageBox = false;
                if (result == 1) { 
                    showMessageBox = false;
                    
                    toDoFile.open(toDoFilename, ios::app);
                    if(toDoFile.is_open())
                    {
                        toDoFile << (string(inputText) + "\n").c_str();
                        toDoFile.close();
                        ReloadTasks(toDoFile, toDoFilename, tasks);
                    }
                    
                    inputText[0] = '\0';
                }
                if (result == 2) inputText[0] = '\0';
            }

        EndDrawing();
    }

    CloseWindow();
    return 0;
}
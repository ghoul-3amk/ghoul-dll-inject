
#include <vector>
#include <string>
#include <Windows.h>
#include <iostream>
#include <fstream>
#include "imgui.h"
#include "imgui_internal.h"
#include <thread>
#include <chrono>
#include <random>

using namespace std::chrono;
using namespace std;

#include <filesystem>
namespace fs = std::filesystem;


namespace cfg {



    const char* label_notify = "";
}

static bool active_notify = false;
static float radius = 0.f;
static float timer = 0.f;

void notify_active(const char* label) {

    if (radius < 20.f) cfg::label_notify = label;
    active_notify = true;
    timer = 0.f;
}

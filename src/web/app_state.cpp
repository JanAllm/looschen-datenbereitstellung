#include "web/app_state.h"

#include <utility>

void AppState::setImage(std::vector<unsigned char> png)
{
    std::lock_guard<std::mutex> lock(mtx_);
    image_ = std::move(png);
}

std::vector<unsigned char> AppState::image() const
{
    std::lock_guard<std::mutex> lock(mtx_);
    return image_;
}

bool AppState::hasImage() const
{
    std::lock_guard<std::mutex> lock(mtx_);
    return !image_.empty();
}

void AppState::enqueue(PreviewCommand cmd)
{
    std::lock_guard<std::mutex> lock(mtx_);
    pending_ = std::move(cmd);
}

std::optional<PreviewCommand> AppState::takeCommand()
{
    std::lock_guard<std::mutex> lock(mtx_);
    auto c = pending_;
    pending_.reset();
    return c;
}

void AppState::requestStop()
{
    std::lock_guard<std::mutex> lock(mtx_);
    stop_ = true;
}

bool AppState::takeStop()
{
    std::lock_guard<std::mutex> lock(mtx_);
    const bool s = stop_;
    stop_ = false;
    return s;
}

void AppState::setStatus(bool running, int step, int total,
                         const std::string& project, const std::string& message)
{
    std::lock_guard<std::mutex> lock(mtx_);
    running_ = running;
    step_ = step;
    total_ = total;
    project_ = project;
    message_ = message;
}

nlohmann::json AppState::status() const
{
    std::lock_guard<std::mutex> lock(mtx_);
    return nlohmann::json{
        {"running", running_},
        {"step", step_},
        {"total", total_},
        {"project", project_},
        {"message", message_},
        {"gen", gen_.load()},
    };
}

void AppState::bumpGeneration()
{
    gen_.fetch_add(1);
}

void AppState::setProjects(const std::string& folder, std::vector<std::string> projects)
{
    std::lock_guard<std::mutex> lock(mtx_);
    folder_ = folder;
    projectList_ = std::move(projects);
}

nlohmann::json AppState::projects() const
{
    std::lock_guard<std::mutex> lock(mtx_);
    return nlohmann::json{
        {"folder", folder_},
        {"projects", projectList_},
    };
}

void AppState::setSpsMessage(const std::string& msg)
{
    std::lock_guard<std::mutex> lock(mtx_);
    spsMessage_ = msg;
}

std::string AppState::spsMessage() const
{
    std::lock_guard<std::mutex> lock(mtx_);
    return spsMessage_;
}

void AppState::setNodeDiagnostics(nlohmann::json diag)
{
    std::lock_guard<std::mutex> lock(mtx_);
    nodeDiag_ = std::move(diag);
}

nlohmann::json AppState::nodeDiagnostics() const
{
    std::lock_guard<std::mutex> lock(mtx_);
    return nodeDiag_;
}

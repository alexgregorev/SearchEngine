#include "JsonStorage.h"

#include "nlohmann/json.hpp"

#include <fstream>
#include <sstream>
#include <iomanip>
#include <filesystem>
#include <iostream>

using json = nlohmann::json;
namespace fs = std::filesystem;

JsonStorage::JsonStorage(
    const std::string& c,
    const std::string& r,
    const std::string& a)
    :configFile(c),requestFile(r),answersFile(a)
{
}

std::vector<std::string> JsonStorage::loadDocuments()
{
    std::vector<std::string> docs;

    std::ifstream f(configFile);

    if (!f)
    {
        std::cerr << "Cannot open config.json\n";
        return docs;
    }

    json j;
    f >> j;

    auto cfg = j["config"];

    for (auto& p : cfg["files"])
    {
        fs::path path = fs::absolute(p.get<std::string>());

        if (fs::is_directory(path))
        {
            for (auto& e : fs::recursive_directory_iterator(path))
                readFile(e.path().string(), docs);
        }
        else
        {
            readFile(path.string(), docs);
        }
    }

    return docs;
}

size_t JsonStorage::loadMaxResponses()
{
    std::ifstream f(configFile);

    if (!f)
        return 5;

    json j;
    f >> j;

    return j["config"]["max_responses"].get<size_t>();
}

std::vector<std::string> JsonStorage::loadRequests()
{
    std::vector<std::string> req;

    std::ifstream f(requestFile);

    if (!f)
    {
        std::cerr << "Cannot open requests.json\n";
        return req;
    }

    json j;
    f >> j;

    for (auto& r : j["requests"])
        req.push_back(r.get<std::string>());

    return req;
}

void JsonStorage::saveAnswers(
    const std::vector<std::vector<RelativeIndex>>& answers)
{
    std::lock_guard<std::mutex> lock(mtx);

    json out;

    int id = 1;

    for (auto& res : answers)
    {
        std::stringstream key;

        key << "request"
            << std::setw(3)
            << std::setfill('0')
            << id++;

        if (res.empty())
        {
            out["answers"][key.str()]["result"] = false;
        }
        else
        {
            out["answers"][key.str()]["result"] = true;

            for (auto& r : res)
            {
                out["answers"][key.str()]["relevance"].push_back({
                    {"docid", r.doc_id},
                    {"rank", r.rank}
                });
            }
        }
    }

    std::ofstream f(answersFile);
    f << std::setw(4) << out;
}

void JsonStorage::appendRequest(const std::string& q)
{
    std::lock_guard<std::mutex> lock(mtx);

    json j;

    std::ifstream in(requestFile);

    if (in)
        in >> j;

    if (!j.contains("requests"))
        j["requests"] = json::array();

    j["requests"].push_back(q);

    std::ofstream out(requestFile);
    out << std::setw(4) << j;
}

void JsonStorage::appendAnswer(
    const std::vector<RelativeIndex>& res)
{
    std::lock_guard<std::mutex> lock(mtx);

    json j;

    std::ifstream in(answersFile);

    if (in)
        in >> j;

    if (!j.contains("answers"))
        j["answers"] = json::object();

    int id = j["answers"].size() + 1;

    std::stringstream key;

    key << "request"
        << std::setw(3)
        << std::setfill('0')
        << id;

    if (res.empty())
    {
        j["answers"][key.str()]["result"] = false;
    }
    else
    {
        j["answers"][key.str()]["result"] = true;

        j["answers"][key.str()]["relevance"] = json::array();

        for (auto& r : res)
        {
            j["answers"][key.str()]["relevance"].push_back({
                {"docid", r.doc_id},
                {"rank", r.rank}
            });
        }
    }

    std::ofstream out(answersFile);
    out << std::setw(4) << j;
}

void JsonStorage::readFile(
    const std::string& p,
    std::vector<std::string>& docs)
{
    std::ifstream f(p);

    if (!f)
        return;

    std::stringstream buf;

    buf << f.rdbuf();

    docs.push_back(buf.str());

    docPaths.push_back(p);
}
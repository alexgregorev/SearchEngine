#pragma once

#include <string>
#include <vector>
#include <mutex>

#include "SearchServer.h"

class JsonStorage
{
public:

    JsonStorage(
        const std::string& config,
        const std::string& requests,
        const std::string& answers);

    std::vector<std::string> loadDocuments();

    size_t loadMaxResponses();

    std::vector<std::string> loadRequests();

    void saveAnswers(
        const std::vector<std::vector<RelativeIndex>>& answers);

    void appendRequest(const std::string& q);

    void appendAnswer(const std::vector<RelativeIndex>& res);

    std::vector<std::string> docPaths;

private:

    void readFile(
        const std::string& p,
        std::vector<std::string>& docs);

    std::string configFile;
    std::string requestFile;
    std::string answersFile;

    std::mutex mtx;
};
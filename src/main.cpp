#include "JsonStorage.h"
#include "InvertedIndex.h"
#include "SearchServer.h"
#include "WebServer.h"

#include <iostream>
#include <thread>

int main()
{
    try
    {
        std::cout << "Starting SearchEngine\n";

        JsonStorage storage(
            "data/config.json",
            "data/requests.json",
            "data/answers.json"
        );

        auto docs = storage.loadDocuments();
        auto paths = storage.docPaths;

        std::cout << "Loaded docs: "
                  << docs.size()
                  << std::endl;

        InvertedIndex index;

        index.indexDocumentsParallel(
            docs,
            paths);

        size_t maxResponses =
            storage.loadMaxResponses();

        SearchServer server(
            index,
            maxResponses);

        std::thread web(
            startServer,
            std::ref(server),
            std::ref(storage));

        web.detach();

        auto requests =
            storage.loadRequests();

        std::vector<
            std::vector<RelativeIndex>> answers;

        for (auto& q : requests)
        {
            answers.push_back(
                server.searchQuery(q));
        }

        storage.saveAnswers(answers);

        std::cout
            << "Press ENTER to exit\n";

        std::cin.get();
    }
    catch (std::exception& e)
    {
        std::cerr
            << "Error: "
            << e.what()
            << "\n";
    }
}
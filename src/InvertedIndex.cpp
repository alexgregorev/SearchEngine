#include "InvertedIndex.h"
#include "TextUtils.h"

#include <thread>
#include <mutex>
#include <algorithm>

void InvertedIndex::indexDocumentsParallel(
    const std::vector<std::string>& docs,
    const std::vector<std::string>& paths)
{
    docPaths = paths;

    size_t threads =
        std::thread::hardware_concurrency();

    if (threads == 0)
        threads = 4;

    size_t chunk =
        docs.size() / threads + 1;

    std::mutex mergeMutex;

    std::vector<std::thread> workers;

    docLength.reserve(docs.size());

    for (size_t t = 0; t < threads; t++)
    {
        workers.emplace_back([&, t]()
        {
            size_t begin = t * chunk;

            size_t end =
                std::min(begin + chunk, docs.size());

            std::unordered_map<
                std::string,
                std::vector<Entry>> localIndex;

            std::vector<size_t> localLengths;

            for (size_t id = begin; id < end; id++)
            {
                const auto& doc = docs[id];

                auto words =
                    TextUtils::tokenize(doc);

                localLengths.push_back(words.size());

                std::unordered_map<std::string,int> local;

                for (auto& w : words)
                    local[w]++;

                for (auto& [w,c] : local)
                    localIndex[w].push_back({id,(size_t)c});
            }

            std::lock_guard<std::mutex> lock(mergeMutex);

            for (auto& [word,entries] : localIndex)
            {
                auto& vec = freq[word];

                vec.insert(
                    vec.end(),
                    entries.begin(),
                    entries.end());
            }

            docLength.insert(
                docLength.end(),
                localLengths.begin(),
                localLengths.end());
        });
    }

    for (auto& w : workers)
        w.join();

    computeAvg();
}

const std::vector<Entry>&
InvertedIndex::get(const std::string& word) const
{
    static const std::vector<Entry> empty;

    auto it = freq.find(word);

    if (it == freq.end())
        return empty;

    return it->second;
}

const std::string&
InvertedIndex::getPath(size_t id) const
{
    return docPaths[id];
}

size_t InvertedIndex::docsCount() const
{
    return docPaths.size();
}

size_t InvertedIndex::length(size_t id) const
{
    return docLength[id];
}

void InvertedIndex::computeAvg()
{
    double sum = 0;

    for (auto l : docLength)
        sum += l;

    avgLen =
        docLength.empty()
        ? 0
        : sum / docLength.size();
}
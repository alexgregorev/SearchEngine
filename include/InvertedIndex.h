#pragma once

#include <string>
#include <vector>
#include <unordered_map>

struct Entry
{
    size_t doc_id;
    size_t count;
};

class InvertedIndex
{
public:

    void indexDocumentsParallel(
        const std::vector<std::string>& docs,
        const std::vector<std::string>& paths);

    const std::vector<Entry>& get(
        const std::string& word) const;

    const std::string& getPath(size_t id) const;

    size_t docsCount() const;

    size_t length(size_t id) const;

    double avgLen = 0;

private:

    void computeAvg();

    std::unordered_map<std::string,
        std::vector<Entry>> freq;

    std::vector<size_t> docLength;

    std::vector<std::string> docPaths;
};
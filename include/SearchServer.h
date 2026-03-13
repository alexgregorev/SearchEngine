#pragma once

#include "InvertedIndex.h"

#include <vector>
#include <string>
#include <cmath>

struct RelativeIndex
{
    size_t doc_id;
    float rank;

    bool operator<(const RelativeIndex& other) const
    {
        if (std::fabs(rank - other.rank) < 1e-6)
            return doc_id < other.doc_id;

        return rank > other.rank;
    }
};

class SearchServer
{
public:

    SearchServer(InvertedIndex& i, size_t maxRes);

    std::vector<RelativeIndex> searchQuery(const std::string& q);

    const std::string& getPath(size_t id) const;

    size_t docsCount() const;

private:

    InvertedIndex& index;

    size_t maxResponses;
};
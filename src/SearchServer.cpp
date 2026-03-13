#include "SearchServer.h"
#include "TextUtils.h"

#include <map>
#include <algorithm>
#include <cmath>

const float BM25_K = 1.6f;
const float BM25_B = 0.75f;

SearchServer::SearchServer(
    InvertedIndex& i,
    size_t maxRes)
    :index(i),maxResponses(maxRes)
{
}

std::vector<RelativeIndex>
SearchServer::searchQuery(const std::string& q)
{
    std::vector<std::string> words =
        TextUtils::tokenize(q);

    std::map<size_t,double> scores;

    double N = index.docsCount();

    for (auto& term : words)
    {
        const auto& entries = index.get(term);

        double df = entries.size();

        if (df == 0)
            continue;

        double idf =
            log((N - df + 0.5)/(df + 0.5) + 1);

        for (auto& e : entries)
        {
            double norm = 1;

            double tf = e.count;

            double dl = index.length(e.doc_id);

            if (index.avgLen > 0)
                norm = dl / index.avgLen;

            double denom =
                tf + BM25_K * (1 - BM25_B + BM25_B * norm);

            if (denom == 0)
                denom = 1e-9;

            if (index.avgLen > 0)
                norm = dl/index.avgLen;

            double score =
                idf *
                ((tf*(BM25_K+1))/denom);

            scores[e.doc_id] += score;
        }
    }

    if (scores.empty())
        return {};

    double maxScore = 0.0;

    for (const auto& [d, s] : scores)
        if (s > maxScore)
            maxScore = s;
    
    std::vector<RelativeIndex> result;

    for (auto& [doc, score] : scores)
    {
        float rank = maxScore > 1e-12
            ? static_cast<float>(score / maxScore)
            : 0.f;

        result.push_back({ doc,rank });
    }

    std::sort(result.begin(),result.end());

    if (result.size() > maxResponses)
        result.resize(maxResponses);

    return result;
}

const std::string& SearchServer::getPath(size_t id) const
{
    return index.getPath(id);
}

size_t SearchServer::docsCount() const
{
    return index.docsCount();
}
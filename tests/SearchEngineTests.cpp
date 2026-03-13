#include <gtest/gtest.h>
#include "InvertedIndex.h"
#include "SearchServer.h"
#include "TextUtils.h"
#include "external_libs.h"

#include <random>
#include <set>

//////////////////////////////////////////////////////////////
// Global vocabulary (safe for parameterized tests)
//////////////////////////////////////////////////////////////

const std::vector<std::string> vocabulary =
{
    "tiger","cruel","youth","man","salt","river",
    "stag","water","hunter","fox","goat",
    "cowboy","villagers","farmer","sons",
    "lion","mouse","net","fisherman",
    "crow","stones","jackal","animals"
};

//////////////////////////////////////////////////////////////
// Shared Environment
//////////////////////////////////////////////////////////////

class SearchEnvironment : public ::testing::Environment
{
public:

    InvertedIndex index;
    std::unique_ptr<SearchServer> server;

    std::vector<std::string> docs;

    void SetUp() override
    {
        docs =
        {
            "tiger cruel youth man eater gold bangle forest",
            "man dealt salt ass carry loads salt river",
            "stag thirsty drink water horns hunter",
            "fox passing well goat thirsty fox",
            "cowboy cried tiger villagers",
            "farmer sons bundle sticks united",
            "lion mouse hunter net escaped",
            "fisherman river monkey net",
            "crow thirsty water pot stones",
            "jackal blue animals king"
        };

        index.indexDocumentsParallel(docs, {});

        server = std::make_unique<SearchServer>(index, 5);
    }
};

SearchEnvironment* env;

//////////////////////////////////////////////////////////////
// Test main
//////////////////////////////////////////////////////////////

int main(int argc, char** argv)
{
    ::testing::InitGoogleTest(&argc, argv);

    env = static_cast<SearchEnvironment*>(
        ::testing::AddGlobalTestEnvironment(
            new SearchEnvironment)
        );

    return RUN_ALL_TESTS();
}

//////////////////////////////////////////////////////////////
// Random query helper
//////////////////////////////////////////////////////////////

std::string randomQuery()
{
    static std::mt19937 rng(42);

    std::uniform_int_distribution<int>
        dist(0, vocabulary.size() - 1);

    return vocabulary[dist(rng)];
}

//////////////////////////////////////////////////////////////
// Parameterized Tests
//////////////////////////////////////////////////////////////

class QueryTest :
    public ::testing::TestWithParam<std::string> {
};

TEST_P(QueryTest, QueryReturnsFiniteRanks)
{
    auto q = GetParam();

    auto result = env->server->searchQuery(q);

    for (const auto& r : result)
        ASSERT_TRUE(std::isfinite(r.rank));
}

INSTANTIATE_TEST_SUITE_P(
    VocabularyQueries,
    QueryTest,
    ::testing::ValuesIn(vocabulary)
);

//////////////////////////////////////////////////////////////
// Basic tests
//////////////////////////////////////////////////////////////

TEST(SearchBasic, SearchReturnsResults)
{
    auto r = env->server->searchQuery("tiger");

    ASSERT_FALSE(r.empty());
}

TEST(SearchBasic, UnknownWord)
{
    auto r = env->server->searchQuery("banana");

    EXPECT_TRUE(r.empty());
}

//////////////////////////////////////////////////////////////
// Ranking tests
//////////////////////////////////////////////////////////////

TEST(SearchRanking, SortedResults)
{
    auto r = env->server->searchQuery("water");

    for (size_t i = 1; i < r.size(); i++)
        EXPECT_GE(r[i - 1].rank, r[i].rank);
}

TEST(SearchRanking, RankRange)
{
    auto r = env->server->searchQuery("water");

    for (auto& e : r)
    {
        EXPECT_GE(e.rank, 0);
        EXPECT_LE(e.rank, 1);
    }
}

//////////////////////////////////////////////////////////////
// Stress Tests
//////////////////////////////////////////////////////////////

TEST(SearchStress, RandomQueries)
{
    const int iterations = 1000;

    for (int i = 0; i < iterations; i++)
    {
        auto q = randomQuery();

        auto r = env->server->searchQuery(q);

        for (auto& e : r)
            ASSERT_TRUE(std::isfinite(e.rank));
    }
}

//////////////////////////////////////////////////////////////
// Property Tests
//////////////////////////////////////////////////////////////

TEST(SearchProperty, SortedRandomQueries)
{
    for (int i = 0; i < 200; i++)
    {
        auto query = randomQuery();

        auto result = env->server->searchQuery(query);

        for (size_t i = 1; i < result.size(); i++)
            EXPECT_GE(result[i - 1].rank, result[i].rank);
    }
}

TEST(SearchProperty, DeterministicSearch)
{
    for (int i = 0; i < 100; i++)
    {
        auto query = randomQuery();

        auto r1 = env->server->searchQuery(query);
        auto r2 = env->server->searchQuery(query);

        ASSERT_EQ(r1.size(), r2.size());

        for (size_t i = 0; i < r1.size(); i++)
            EXPECT_EQ(r1[i].doc_id, r2[i].doc_id);
    }
}
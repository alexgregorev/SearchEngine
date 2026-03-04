#pragma once

#include "ConverterJSON.h"
#include "InvertedIndex.h"

#include <fstream>
#include <string>
#include <vector>

class SearchServer
{
public:

    SearchServer() = default;

    SearchServer( InvertedIndex& idx );

    vector<vector<RelativeIndex>>& search( vector<string> queries_input );

    void requestParsing( const vector<string>& input_words );

    void distribution( vector<vector<size_t>>& document );

    vector<vector<RelativeIndex>>& getAnswers();

private:
    InvertedIndex _index;
    vector<vector<RelativeIndex>> relative_index;
};
#pragma once

#include <fstream>
#include <iostream>
#include <map>
#include <string>
#include <vector>

#include "nlohmann/json.hpp"
using json = nlohmann::json;
using namespace std;

struct RelativeIndex
{
    size_t doc_id;
    float rank;

    bool operator == ( const RelativeIndex& other ) const
    {
        return ( doc_id == other.doc_id && rank == other.rank );
    }
};

class ConverterJSON
{
public:

    ConverterJSON() = default;

    static json configJSON();

    static void checkConfig();

    void setDocuments();

    vector<string>& getDocuments();

    void setRequest();

    vector<string>& getRequests();

    static int getResponsesLimit() ;

    bool getDBUpdate() const;

    static bool checkDocBase();

    static void setDocBaseJSON( map<string, vector<RelativeIndex>>& doc_base );

    static map<string, vector<RelativeIndex>> getDocBase();

    static void putAnswers( const vector<vector<RelativeIndex>>& answers ) ;

private:
    vector<string> requests;
    vector<string> documents;
};

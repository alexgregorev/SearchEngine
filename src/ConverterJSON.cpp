#include "ConverterJSON.h"

json ConverterJSON::configJSON()
{
    ifstream Config( "config.json", ios::in );
    json dict; Config >> dict;
    return dict;
}

void ConverterJSON::checkConfig()
{
        ifstream Config( "resources/config.json", ios::in );
        json dict; Config >> dict;

        if (  ifstream Check( "config.json", ios::in ); ! Check )
            throw invalid_argument( "Config file is missing" );
        if ( configJSON()[ "config" ].empty())
            throw invalid_argument( "Config file is empty" );
        if ( configJSON()[ "config" ][ "version" ] != dict[ "config" ][ "version" ])
            throw invalid_argument( "config.json has incorrect file version" );
}

int ConverterJSON::getResponsesLimit()
{
    int max_responses = 5;
    if ( const ifstream Check( "config.json", ios::in ); Check )
        max_responses = configJSON()[ "config" ][ "max_responses" ];
    return max_responses;
}

void ConverterJSON::setDocuments()
{
    for ( json dict = configJSON()[ "files" ]; auto& file : dict )
        documents.push_back( file );
}

vector<string>& ConverterJSON::getDocuments()
{
    return documents;
}

void ConverterJSON::setRequest()
{
    if (  ifstream Check( "requests.json", ios::in ); ! Check )
        throw invalid_argument( "Requests file is missing" );

    ifstream Requests( "requests.json", ios::in );
    json dict; Requests >> dict;

    if ( dict[ "requests" ].empty() )
        cout << "Nothing a search for" << endl;
    else if ( dict[ "requests" ].size() > 1000 )
        cerr << "1000+ requests" << endl;
    else
        for ( auto& request : dict[ "requests" ])
            requests.push_back( request );
}

vector<string>& ConverterJSON::getRequests()
{
    return requests;
}

bool ConverterJSON::getDBUpdate() const
{
    if ( const ifstream Check( "config.json", ios::in ); Check )
    {
        int db_update = configJSON()[ "config" ][ "db_update" ];
        if ( requests.empty() || requests.size() % db_update != 0 )
            return false;
        cout << "DocumentBase update" << endl;
    }
    return true;
}

bool ConverterJSON::checkDocBase()
{
    if ( const ifstream Check( "dbase.json", ios::in ); ! Check ) return false;
    return true;
}

void ConverterJSON::setDocBaseJSON( map<string, vector<RelativeIndex>>& doc_base )
{
    if ( ifstream Check( "dbase.json", ios::in ); ! Check )
    {
        ifstream dBase( "resources/dbase.json", ios::in );
        json dict; dBase >> dict;
        ofstream inFile( "dbase.json" );
        inFile << dict;
        cout << "Create dbase.json" << endl;
        inFile.close();
    }

    ofstream inFile( "dbase.json" );
    json dict; int cnt = 0;
    map<string, vector<RelativeIndex>>::iterator it;
    for ( auto& it : doc_base )
    {
        ++cnt;
        json temp = dict[ "doc_base" ][ "word_" + to_string( cnt )];
        temp[ "value" ] = it.first;

        if ( it.second.size() == 1 )
            for ( auto& [ doc_id, count ] : it.second )
            {
                temp[ "doc_id" ] = doc_id;
                temp[ "count" ] = count;
            }
        else
            for ( auto& [ doc_id,count ] : it.second )
            {
                temp[ "docs" ][ "doc_id" ].push_back( doc_id );
                temp[ "docs" ][ "count" ].push_back( count );
            }
        dict[ "doc_base" ][ "word_" + to_string( cnt )] = temp;
    }
    inFile << dict;
}

map<string, vector<RelativeIndex>> ConverterJSON::getDocBase()
{
    ifstream dBase( "dbase.json" );
    json dict; dBase >> dict;
    map<string, vector<RelativeIndex>> doc_base;

    for( auto word = 1; word <= dict[ "doc_base" ].size(); ++word )
    {
        vector<RelativeIndex> index;
        json temp = dict[ "doc_base" ][ "word_" + to_string( word )];
        string value = temp[ "value" ];

        if ( temp[ "docs" ].empty())
            index.emplace_back( temp[ "doc_id" ], temp[ "count" ]);
        else
            for ( auto j = 0; j < temp[ "docs" ][ "doc_id" ].size(); ++j )
                index.emplace_back(
                    temp[ "docs" ][ "doc_id" ][ j ],
                    temp[ "docs" ][ "count" ][ j ]);

        doc_base.insert({ value, index });
    }
    return doc_base;
}

void ConverterJSON::putAnswers( const vector<vector<RelativeIndex>>& answers )
{
    if ( ifstream Check( "answers.json", ios::in ); ! Check )
    {
        ifstream Answers( "resources/answers.json", ios::in );
        json dict; Answers >> dict;
        ofstream inFile( "answers.json" );
        inFile << dict;
        cout << "Create answers.json" << endl;
    }

    json dict;
    ofstream inFile( "answers.json" );
    for ( auto request = 0; request < answers.size(); ++request )
    {
        json temp;
        if ( answers[ request ].empty())
            temp[ "result" ] = "false";
        else
        {
            temp[ "result" ] = "true";
            for ( auto answer = 0; answer < answers[ request ].size(); ++answer )
            {
                size_t doc_id = answers[ request ][ answer ].doc_id;
                float rank = answers[ request ][ answer ].rank;

                answers[ request ].size() == 1
                    ? temp[ "doc_id" ].push_back( doc_id ),
                      temp[ "rank" ].push_back( rank )
                    : temp[ "relevance" ].push_back({
                           { "doc_id", doc_id },
                           { "rank", rank }});
            }
        }
        dict[ "answers" ][ "request_" + to_string( request + 1 )] = temp;
    }
    if ( ! answers.empty()) cout << "Search complete" << endl;
    inFile << dict.dump(4);
}
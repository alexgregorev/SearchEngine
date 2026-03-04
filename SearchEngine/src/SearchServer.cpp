#include "ConverterJSON.h"
#include "InvertedIndex.h"
#include "SearchServer.h"

SearchServer::SearchServer( InvertedIndex& idx ) : _index( idx ) {};

vector<vector<RelativeIndex>>& SearchServer::search( vector<string> queries_input )
{
    relative_index.clear();

    for ( auto& request : queries_input )
    {
        string word;
        vector<string> input_words;

        for( auto simbol = 0; simbol < request.length(); ++simbol )
        {
            if ( request[ simbol ] != ' ' )
            {
                word += request[ simbol ];
                if ( simbol != request.length() - 1 )
                    continue;
            }
            input_words.push_back( word );
            word.clear();
        }

        if ( input_words.size() > 10 )
        {
            input_words.clear();
            cerr << "Overlong request" << endl;
        }
        else
            requestParsing( input_words );
    }
    return relative_index;
}

void SearchServer::requestParsing( const vector<string>& input_words )
{
    vector<Entry> entry;
    vector<vector<size_t>> document;

    for ( auto& word : input_words )
    {
        entry = _index.getWordCount( word );
        for ( auto& [ doc_id, count ] : entry )
        {
            bool check = true;
            for ( auto& it : document )
            {
                if ( it[ 1 ] ==  doc_id )
                {
                    it[ 0 ] += count;
                    check = false;
                    break;
                }
            }
            if ( check ) document.push_back({ count, doc_id });
        }
    }
    distribution( document );
}

void SearchServer::distribution( vector<vector<size_t>>& document )
{
    float max = 0;
    if ( ! document.empty())
    {
        sort( document.begin(), document.end(),
              []( const vector<size_t>& a, const vector<size_t>& b )
              {
                  return a[ 0 ] > b[ 0 ];
			});
        max = document[ 0 ][ 0 ];
    }
	
    RelativeIndex rlv{};
    vector<RelativeIndex> input_relative;

    int responsesLimit = ConverterJSON::getResponsesLimit();
    for ( auto& it : document )
    {
        if ( input_relative.size() == responsesLimit ) break;
        rlv.doc_id = it[ 1 ];
        rlv.rank = static_cast<float>( it[ 0 ] ) / max;
        input_relative.push_back({ rlv.doc_id, rlv.rank });
    }
    relative_index.push_back( input_relative );
}

vector<vector<RelativeIndex>>& SearchServer::getAnswers()
{
    return relative_index;
}
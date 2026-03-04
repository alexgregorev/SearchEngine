#include "ConverterJSON.h"
#include "InvertedIndex.h"

mutex dict_access;

InvertedIndex::InvertedIndex( const ConverterJSON&  cvr ) : _convert( cvr ) {};

void InvertedIndex::indexDocument( TextIndex texts_input )
{
    unordered_map<string, size_t> words;

    for ( auto& doc : texts_input.doc_text )
        words.insert( pair( doc, 0 ));

    for( auto& word : words )
        word.second = ranges::count( texts_input.doc_text, word.first );

    {
        lock_guard<mutex> lock( dict_access );
        for ( auto& word : words )
        {
            bool check = true;
            for ( auto& it : freq_dictionary )
            {
                if ( it.first == word.first )
                {
                    it.second.push_back({ texts_input.doc_id, word.second });
                    check = false;
                }
            }
            if ( check || freq_dictionary.empty())
            {
                vector<Entry>entry{{ texts_input.doc_id, word.second }};
                freq_dictionary.insert( pair( word.first, entry ));
            }
        }
    }
}

void InvertedIndex::updateDocumentBase( vector<string> input_docs )
{
    auto thread_Dict = [ this ]( const string& doc_page, const size_t& doc_id )
    {
        _convert.getDocuments().empty() 
        ? indexDocument( docOutVector( doc_page, doc_id ))
        : indexDocument( docOutFile( doc_page, doc_id ));
    };

    if ( ! ConverterJSON::checkDocBase() || _convert.getDBUpdate() || _convert.getDocuments().empty())
    {
        vector<thread> thread_vec;
        if ( input_docs.size() >= 1000 ) cerr << "1000+ files" << endl;
        for ( auto doc_id = 0; doc_id < input_docs.size() && doc_id < 1000; ++doc_id )
            thread_vec.emplace_back ( thread_Dict, input_docs[ doc_id ], doc_id );
        for ( auto & thread : thread_vec ) thread.join();

        if ( ! _convert.getDocuments().empty()) setDocBase();
    }
    else getDocBase();
}

TextIndex InvertedIndex::docOutVector( const string& doc_page, const size_t& doc_id )
{
    string word;
    vector<string> texts_input;

    for ( auto simbol = 0; simbol < doc_page.length(); ++simbol )
    {
        if ( doc_page[ simbol ] != ' ' )
        {
            if ( ! validSimbol( doc_page[ simbol ]))
            {
                char temp = doc_page[ simbol ];
                letterCase( temp );
                word += temp;
            }
            if ( simbol != doc_page.length() - 1 )
                continue;
        }
        texts_input.push_back( word );
        word.clear();
    }
    return TextIndex{ texts_input, doc_id };
}

TextIndex InvertedIndex::docOutFile( const string& doc_page, const size_t& doc_id )
{
    string word;
    vector<string> texts_input;

    if ( ifstream Check( doc_page.c_str(), ios::in ); ! Check )
        cerr << "file_" << to_string( doc_id ) << " not found" << endl;

    ifstream Document( doc_page.c_str(), ios::in );
    while ( Document >> word )
    {
        string buffer;
        auto upload = [ &texts_input ]( string& buffer )
        {
            if ( ! buffer.empty()) texts_input.push_back( buffer );
            buffer.clear();
        };

        for ( auto simbol = 0; simbol < word.size(); ++simbol )
        {
            letterCase( word[ simbol ]);

            if ( word[ simbol ] == '\'' && word[ simbol + 1 ] == 's' )
            {
                ++simbol;
                upload( buffer );
            }
            else if ( validSimbol( word[ simbol ]))
                upload( buffer );
            else
                buffer += word[ simbol ];
        }
        if ( texts_input.size() > 1000 || buffer.length() > 100 )
        {
            cerr << "file_" << to_string( doc_id );
            buffer.length() > 100
            ? cerr << " wideword: " << buffer << " \n"
            : cerr << ": lots of words" << " \n";
            texts_input.clear();
            break;
        }
        upload( buffer );
    }
    return TextIndex{ texts_input, doc_id };
}

void InvertedIndex::letterCase( char& value )
{
    const string caps = "ABCDEFGHIJKLMNOPQRSTUVWXYZ";
    for ( auto& simbol : caps )
        if ( value == simbol )
        {
            value = tolower( value );
            break;
        }
}

bool InvertedIndex::validSimbol(const char& value )
{
    const string simbols = ".,!?-:;()'\"\\|/{}[]<>_+=№@#$%^&*~`1234567890";
    for ( auto& simbol : simbols )
        if ( value == simbol ) return true;
    return false;
}

void InvertedIndex::setDocBase()
{
    map<string, vector<RelativeIndex>> doc_base;
    for ( auto& it : freq_dictionary )
    {
        vector<RelativeIndex> word_cnt;
        for ( auto& entry : it.second )
            word_cnt.emplace_back( RelativeIndex{ entry.doc_id, static_cast<float>( entry.count )});
        doc_base[ it.first ] = word_cnt;
    }
    ConverterJSON::setDocBaseJSON( doc_base );
}

void InvertedIndex::getDocBase()
{
    map doc_base( ConverterJSON::getDocBase());
    map<string, vector<RelativeIndex>>::iterator it;
    for ( it = doc_base.begin(); it != doc_base.end(); ++it)
    {
        vector<Entry> entry;
        for ( auto& [ doc_id, count ] : it->second )
            entry.push_back({  doc_id, static_cast<size_t>( count )});
        freq_dictionary.insert( pair( it->first, entry ));
    }
}

vector<Entry> InvertedIndex::getWordCount( const string& word )
{
    vector<Entry> entry;
    for ( auto& it : freq_dictionary )
    {
        if ( word == it.first )
            for ( auto& [ doc_id, count ] : it.second )
                entry.push_back({  doc_id, static_cast<size_t>( count )});
    }
    return entry;
}
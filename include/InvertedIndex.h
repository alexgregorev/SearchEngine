#pragma once

#include "ConverterJSON.h"

#include <fstream>
#include <iostream>
#include <map>
#include <mutex>
#include <string>
#include <thread>
#include <vector>
#include <utility>

struct Entry
{
    size_t doc_id;
    size_t count;

    bool operator == ( const Entry& other ) const
    {
        return ( doc_id == other.doc_id && count == other.count );
    }
};

struct TextIndex
{
    vector<string> doc_text;
    size_t doc_id;

    bool operator == ( const TextIndex& other ) const
    {
        return ( doc_text == other.doc_text && doc_id == other.doc_id );
    }
};

class InvertedIndex
{
public:
    InvertedIndex() = default;

    InvertedIndex( const ConverterJSON& cvr );

    void indexDocument( TextIndex texts_input );

    void updateDocumentBase( vector<string> input_docs );

    static TextIndex docOutVector( const string& doc_page, const size_t& doc_id );

    static TextIndex docOutFile( const string& doc_page, const size_t& doc_id );

    static void letterCase( char& value );

    static bool validSimbol (const char& value );

    void setDocBase();

    void getDocBase();

    vector<Entry> getWordCount( const string& word );

private:
    ConverterJSON _convert;
    map<string, vector<Entry>> freq_dictionary;
};
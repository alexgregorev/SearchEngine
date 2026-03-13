#pragma once

class SearchServer;
class JsonStorage;

void startServer(
    SearchServer& search,
    JsonStorage& storage);
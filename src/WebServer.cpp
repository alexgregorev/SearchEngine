#include "SearchServer.h"
#include "JsonStorage.h"

#include "httplib.h"
#include "nlohmann/json.hpp"

#include <filesystem>
#include <fstream>
#include <sstream>

using json = nlohmann::json;
using namespace httplib;
namespace fs = std::filesystem;

void startServer(
    SearchServer& search,
    JsonStorage& storage)
{
    Server svr;

    svr.Get("/search",
    [&](const Request& req, Response& res)
    {
        std::string q;

        if (req.has_param("q"))
            q = req.get_param_value("q");

        auto ranked = search.searchQuery(q);

        storage.appendRequest(q);
        storage.appendAnswer(ranked);

        json r;

        r["results"] = json::array();

        for (auto& d : ranked)
        {
            r["results"].push_back({
                {"docid",d.doc_id},
                {"score",d.rank},
				{"path",search.getPath(d.doc_id)}
                });
        }

        res.set_content(r.dump(4),"application/json");
    });

    svr.Get("/doc",
    [&](const Request& req, Response& res)
    {
        if (!req.has_param("id"))
        {
            res.status = 400;
            res.set_content("Missing id","text/plain");
            return;
        }

        int id = std::stoi(req.get_param_value("id"));

        if (id < 0 || id >= search.docsCount())
        {
            res.status = 404;
            res.set_content("Document not found","text/plain");
            return;
        }

        std::string path =
            search.getPath(id);

        std::ifstream file(path);

        if (!file)
        {
            res.status = 404;
            res.set_content("Cannot open file","text/plain");
            return;
        }

        std::stringstream buffer;

        buffer << file.rdbuf();

        res.set_content(buffer.str(),"text/plain");
    });

    fs::path web = fs::current_path() / "web";

    svr.set_mount_point("/", web.string());

    svr.listen("0.0.0.0",8080);
}
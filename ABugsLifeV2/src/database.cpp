#include <iostream>
#include <stdlib.h>
#include <string>
#include <vector>
#include <stdexcept>

#include "../includes/data_models.hpp"
#include "../includes/sqlite_orm.hpp"
#include "../includes/httplib.hpp"
#include "../includes/json.hpp"

using namespace httplib;
using namespace sqlite_orm;
using namespace std;
using json = nlohmann::json;

////////////////////////////////////////////////////////////

void to_json(json& j, const Filter& f) {
    j = json{{"id", f.id}, {"name", f.name}, {"x", f.x}, {"y", f.y}};
}

void to_json(json& j, const Interaction& i) {
    int filterID = *i.filter_id;
    j = json{{"id", i.id}, {"start_frame", i.start_frame}, {"end_frame", i.end_frame}, {"duration", i.duration}, {"filter", filterID}};
}

void Database() {
    //This makes the Database
    auto storage = make_storage(
            database_name_,
            make_table("filter",
                       make_column("id", &Filter::id, autoincrement(), primary_key()),
                       make_column("name", &Filter::name),
                       make_column("x", &Filter::x),
                       make_column("y", &Filter::y)),
            make_table("interaction",
                       make_column("id", &Interaction::id, autoincrement(), primary_key()),
                       make_column("start", &Interaction::start_frame),
                       make_column("end", &Interaction::end_frame),
                       make_column("duration", &Interaction::duration),
                       make_column("filter_id", &Interaction::filter_id),
                       foreign_key(&Interaction::filter_id).references(&Filter::id))
    );
    storage.sync_schema();

    //Server Functionality
    Server svr;

    //Working
    svr.Get("/test", [&](const Request &req, Response &res) {
        res.set_content("The server works", "text/plain");
    });

    //Post Filters
    //
    // request params - "name", "x", "y".
    //
    svr.Post("/filters", [&](const Request& req, Response& res) {
        string name;
        int x = 0;
        int y = 0;
        //Set Name, x and y
        if (req.has_param("name")) {
            name = req.get_param_value("name");
        }
        if (req.has_param("x")) {
            x = std::stoi(req.get_param_value("x"));
        }
        if (req.has_param("y")) {
            y = std::stoi(req.get_param_value("y"));
        }
        Filter f {-1, name, x, y};
        auto insertedId = storage.insert(f);
        std::cout << insertedId << std::endl;
        f.id = insertedId;
        std::cout << f.id << " " << f.name << " " << f.x << " " << f.y << std::endl;
    });

    //Get Filters
    svr.Get("/filters", [&](const Request& req, Response& res) {
        json j_array = {json::array()};
        auto allFilters = storage.get_all<Filter>();
        std::cout << allFilters.size() << std::endl;
        for (size_t i = 0; i < allFilters.size(); ++i) {
            std::cout << allFilters.at(i).id << " " << allFilters.at(i).name << " " << allFilters.at(i).x << " " << allFilters.at(i).y << std::endl;
            json j_object(allFilters.at(i));
            std::cout << j_object << std::endl;
            j_array.push_back(j_object);
        }
        std::cout << j_array << std::endl;
        res.set_content(j_array.dump(), "application/json");
    });

    //Delete Filters
    svr.Delete("/filters", [&](const Request& req, Response& res) {
        storage.remove_all<Filter>();
    });

    //Post Interaction
    //
    // Request params - "StartFrame", "EndFrame", "filterID"
    //
    svr.Post("/interactions", [&](const Request& req, Response& res) {
        int start_frame;
        int end_frame;
        int filter_id;
        //Start Frame, End Frame
        if (req.has_param("StartFrame")) {
            start_frame = std::stoi(req.get_param_value("StartFrame"));
        } //If you fail tests insert 400 status here?
        if (req.has_param("EndFrame")) {
            end_frame = std::stoi(req.get_param_value("EndFrame"));
        }


        if (req.has_param("filterID")) {
            filter_id = std::stoi(req.get_param_value("filterID"));
            auto filterwithID = storage.get_all<Filter>(where(filter_id == c(&Filter::id)));
            if (filterwithID.size()) {
                Filter filter = filterwithID.at(0);
                auto filterID = std::make_unique<int>(filter.id);
                Interaction interaction;
                interaction.id = -1;
                interaction.start_frame = start_frame;
                interaction.end_frame = end_frame;
                interaction.duration = end_frame - start_frame;
                interaction.filter_id = std::move(filterID);
                auto insertedId = storage.insert(interaction);
                interaction.id = insertedId;
            }
        }
    });

    //Get Interactions
    svr.Get("/interactions", [&](const Request& req, Response& res) {
        json j_array = {json::array()};
        auto allInteractions = storage.get_all<Interaction>();
        for (size_t i = 0; i < allInteractions.size(); ++i) {
            json j_object = allInteractions.at(i);
            j_array.push_back(j_object);
        }
        std::cout << j_array << std::endl;
        res.set_content(j_array.dump(), "application/json");
    });

    //Delete Interactions
    svr.Delete("/interactions", [&](const Request& req, Response& res) {
        storage.remove_all<Interaction>();
    });

    svr.listen("0.0.0.0", 1234);
}
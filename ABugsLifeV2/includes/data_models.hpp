#ifndef DB_DATA_MODELS_HPP
#define DB_DATA_MODELS_HPP

#include <string>
#include <utility>

struct Filter {
    int id;
    std::string name;
    float x;
    float y;
};

struct Interaction {
    size_t id;
    float start_frame;
    float end_frame;
    float duration;
    std::unique_ptr<int> filter_id;
    Interaction() = default;
    Interaction(const Interaction& inter) {
        this->id = inter.id;
        this->start_frame = inter.start_frame;
        this->end_frame = inter.end_frame;
        this->duration = inter.duration;
    }
    Interaction(size_t id, size_t start_frame, size_t end_frame) : id(id),
                                                          start_frame(start_frame),
                                                          end_frame(end_frame),
                                                          duration(end_frame - start_frame) {}
};



#endif /* DB_MODELS_HPP */
#include "langnes_json/json.hpp"
#include <iostream>

int main() {
    try {
        using namespace langnes::json;
        auto document{load(R"({"color":"red"})")};
        auto& object{document.as_object()};
        std::cout << object["color"].as_string() << "\n"; // red
        object["color"] = "blue";
        object["size"] = 10;
        std::cout << save(document) << "\n"; // {"size":10,"color":"blue"}
        return 0;
    } catch (const std::exception& e) {
        std::cerr << "Exception: " << e.what() << '\n';
        return 1;
    }
}

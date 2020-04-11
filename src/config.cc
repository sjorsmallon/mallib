// #include <config.h>
// #include <file.h>
// #include <string>
// #include <sstream>


// // what if this is not successful? how do we handle that?
// // have some default settings stored?

// bool config::load_config(std::string filename)
// {
//     std::string data;

//     constexpr const int max_string_length = 50;
//     //@Refactor: take a look at how load_obj works, and refactor
//     // the line incrementing into the file handler.
//     file::file_to_string(filename, data);
//     std::stringstream data_stream(data);
//     constexpr const int max_string_read_length = 9;
//     char garbage_buffer[max_string_length] = {}; // used for the garbage in each line. 

//     size_t line_number = 0;
//     for (std::string line; std::getline(data_stream, line);)
//     {
//         ++line_number;
//         if (line_number[0] == '#')
//         {
//             // heh. so what do we do now? set the right memory location?
//         }
//         if (line_number[0] == '/')
//         {
//             // will be a comment. so we can skip it.
//             continue;
//         }

//     }
// }
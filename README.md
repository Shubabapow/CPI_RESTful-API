# CPI_RESTful-API

This is a git repository for a senior project for the implementation of a RESTful API

# How to get crow setup with a project:

Need:
__________
•	git  
•	visual studio (2019 recommended – should be installed to create C++ programs and use CMake)
•	CMake (should come with VS)


Create folder
Navigate to folder in terminal
Clone repo:	 git clone https://github.com/Microsoft/vcpkg.git ( will need git on computer )
Enter vcpkg:	 cd .\vcpkg
Run:		.\bootstrap-vcpkg.bat
Run:		.\vcpkg integrate install

Install crow
./vcpkg install crow (terminal)
Vcpkg install crow (cmd)

Create project:
Open Visual Studio open file location where crow was installed
Create new empty project
Create new C++ file
(in the file put this example code to test) 
If you are unable to use the include crow.h you might need a fresh install of visual studio code











#include "crow.h"

#include <sstream>
#include <fstream>
#include <string>

class ExampleLogHandler : public crow::ILogHandler {
public:
    void log(std::string /*message*/, crow::LogLevel /*level*/) override {
        //            cerr << "ExampleLogHandler -> " << message;
    }
};

struct ExampleMiddleware
{
    std::string message;

    ExampleMiddleware()
    {
        message = "foo";
    }

    void setMessage(std::string newMsg)
    {
        message = newMsg;
    }

    struct context
    {
    };

    void before_handle(crow::request& /*req*/, crow::response& /*res*/, context& /*ctx*/)
    {
        CROW_LOG_DEBUG << " - MESSAGE: " << message;
    }

    void after_handle(crow::request& /*req*/, crow::response& /*res*/, context& /*ctx*/)
    {
        // no-op
    }
};

int main()
{
    crow::App<ExampleMiddleware> app;

    app.get_middleware<ExampleMiddleware>().setMessage("hello");

    CROW_ROUTE(app, "/")
        .name("hello")
        ([] {
        return "Hello World!";
            });

    CROW_ROUTE(app, "/about")
        ([]() {
        return "About Crow example.";
            });

    // a request to /path should be forwarded to /path/
    CROW_ROUTE(app, "/path/")
        ([]() {
        return "Trailing slash test case..";
            });


    // simple json response
    // To see it in action enter {ip}:18080/json
    CROW_ROUTE(app, "/json")
        ([] {
        crow::json::wvalue x;
        x["message"] = "Hello, World!";
        return x;
            });

    // To see it in action enter {ip}:18080/hello/{integer_between -2^32 and 100} and you should receive
    // {integer_between -2^31 and 100} bottles of beer!
    CROW_ROUTE(app, "/hello/<int>")
        ([](int count) {
        if (count > 100 || count < 0)
            return crow::response(400);
        std::ostringstream os;
        os << count << " bottles of beer!";
        return crow::response(os.str());
            });

    // To see it in action submit {ip}:18080/add/1/2 and you should receive 3 (exciting, isn't it)
    CROW_ROUTE(app, "/add/<int>/<int>")
        ([](const crow::request& /*req*/, crow::response& res, int a, int b) {
        std::ostringstream os;
        os << a + b;
        res.write(os.str());
        res.end();
            });

    // Compile error with message "Handler type is mismatched with URL paramters"
    //CROW_ROUTE(app,"/another/<int>")
    //([](int a, int b){
        //return crow::response(500);
    //});

    // more json example

    // To see it in action, I recommend to use the Postman Chrome extension:
    //      * Set the address to {ip}:18080/add_json
    //      * Set the method to post
    //      * Select 'raw' and then JSON
    //      * Add {"a": 1, "b": 1}
    //      * Send and you should receive 2

    // A simpler way for json example:
    //      * curl -d '{"a":1,"b":2}' {localhost}:18080/add_json
    CROW_ROUTE(app, "/add_json")
        .methods("POST"_method)
        ([](const crow::request& req) {
        auto x = crow::json::load(req.body);
        if (!x)
            return crow::response(400);
        int sum = x["a"].i();
        std::ostringstream os;
        os << sum;
        return crow::response{ os.str() };
            });

    CROW_ROUTE(app, "/GET/filecontent")
        .methods("GET"_method)
        ([](const crow::request& req) {
        std::ifstream myfile; myfile.open("hellofriend.txt");
        std::string mystring;
        if (myfile.is_open())
            myfile >> mystring; // pipe file's content into stream
            //std::cout << mystring; // pipe stream's content to standard output
        return crow::response{mystring};
            });

    // Example of a request taking URL parameters
    // If you want to activate all the functions just query
    // {ip}:18080/params?foo='blabla'&pew=32&count[]=a&count[]=b
    CROW_ROUTE(app, "/params")
        ([](const crow::request& req) {
        std::ostringstream os;

        // To get a simple string from the url params
        // To see it in action /params?foo='blabla'
        os << "Params: " << req.url_params << "\n\n";
        os << "The key 'foo' was " << (req.url_params.get("foo") == nullptr ? "not " : "") << "found.\n";

        // To get a double from the request
        // To see in action submit something like '/params?pew=42'
        if (req.url_params.get("pew") != nullptr) {
            double countD = boost::lexical_cast<double>(req.url_params.get("pew"));
            os << "The value of 'pew' is " << countD << '\n';
        }

        // To get a list from the request
        // You have to submit something like '/params?count[]=a&count[]=b' to have a list with two values (a and b)
        auto count = req.url_params.get_list("count");
        os << "The key 'count' contains " << count.size() << " value(s).\n";
        for (const auto& countVal : count) {
            os << " - " << countVal << '\n';
        }

        // To get a dictionary from the request
        // You have to submit something like '/params?mydict[a]=b&mydict[abcd]=42' to have a list of pairs ((a, b) and (abcd, 42))
        auto mydict = req.url_params.get_dict("mydict");
        os << "The key 'dict' contains " << mydict.size() << " value(s).\n";
        for (const auto& mydictVal : mydict) {
            os << " - " << mydictVal.first << " -> " << mydictVal.second << '\n';
        }

        return crow::response{ os.str() };
            });

    CROW_ROUTE(app, "/large")
        ([] {
        return std::string(512 * 1024, ' ');
            });

    app.port(18080)
        .multithreaded()
        .run();
}




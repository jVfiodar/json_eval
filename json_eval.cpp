#include <string>
#include <iostream>
#include <vector>
#include <fstream>
// #include <map>
#include <cassert>
// #include <stdexcept>
// #include <unordered_map>
#include <chrono>
#include <ext/pb_ds/assoc_container.hpp>
#include <ext/pb_ds/tree_policy.hpp>

using namespace __gnu_pbds;
using namespace std;

#define flag bool

enum typeobj
{
    BOOL,
    INT,
    DOUBLE,
    CHAR,
    STRING,
    ARRAY,
    OBJECT,
    NIL
};

string to_string(typeobj type)
{
    switch (type)
    {
    case BOOL:
        return "boolean";
    case INT:
        return "integer";
    case DOUBLE:
        return "floating point number";
    case STRING:
        return "string";
    case ARRAY:
        return "array";
    case OBJECT:
        return "object";
    case NIL:
        return "null";
    case CHAR:
        return "char";
    }
}

enum func
{
    UNDEF,
    SIZE,
    MIN,
    MAX
};

class jsonObj
{
public:
    typeobj type = NIL;
    string name = "";
    int int_value = 0;
    long double double_value = 0;
    string string_value = "";
    vector<jsonObj *> arr;
    gp_hash_table<string, jsonObj *> contents;
    flag test_call_string_array = 0;
    int stack_trace_sz = 0;

public:
    // for objects or nulls
    jsonObj(string name, typeobj type)
    {
        this->name = name;
        this->type = type;
    }

    // for ints
    jsonObj(string name, int val)
    {
        this->name = name;
        this->type = INT;
        this->int_value = val;
    }

    // for booleans
    jsonObj(string name, bool val)
    {
        this->name = name;
        this->type = BOOL;
        this->int_value = val;
    }

    // for chars
    jsonObj(string name, char val)
    {
        this->name = name;
        this->type = CHAR;
        this->int_value = val;
    }

    // for doubles
    jsonObj(string name, double val)
    {
        this->name = name;
        this->type = DOUBLE;
        this->double_value = val;
    }

    // for strings
    jsonObj(string name, string val)
    {
        this->name = name;
        this->type = STRING;
        this->string_value = val;
    }

    static void utility_skip_whitespaces(string &input_file, int &ptr)
    {
        while (ptr != input_file.length() && (input_file[ptr] == ' ' || input_file[ptr] == '\r' || input_file[ptr] == '\n' || input_file[ptr] == '\t'))
            ptr++;
        if (ptr == input_file.length())
            throw invalid_argument("Unexpected end of file\n");
    }

    static jsonObj *size_obj(vector<jsonObj *> args)
    {
        if (args.size() == 0)
        {
            cout << "WARNING: size got 0 arguments, sending 0 as a result";
            return new jsonObj("", 0);
        }
        if (args.size() > 1)
            throw invalid_argument("ERROR: size got more than 1 argument");
        jsonObj *obj = args[0];
        switch (obj->type)
        {
        case ARRAY:
            return new jsonObj("", (int)((obj->arr).size()));
        case OBJECT:
            return new jsonObj("", (int)((obj->contents).size()));
        case STRING:
            return new jsonObj("", (int)((obj->string_value).length()));
        default:
            throw invalid_argument("ERROR: size doesn't accept non-container classes");
        }
    }

    static jsonObj *max_obj(vector<jsonObj *> args)
    {
        int mx = INT_MIN;
        if (args[0]->type == ARRAY)
        {
            if (args.size() != 1)
                throw invalid_argument("ERROR: array is not the only argument of max()");
            args = args[0]->arr;
        }
        for (int i = 0; i < args.size(); i++)
        {
            switch (args[i]->type)
            {
            case STRING:
                throw invalid_argument("ERROR: strings not supported in max() operation");
            case OBJECT:
                throw invalid_argument("ERROR: objects not supported in max() operation");
            case NIL:
                throw invalid_argument("ERROR: nulls not supported in max() operation");
            case ARRAY:
                throw invalid_argument("ERROR: arrays not supported in max() operation");
            case DOUBLE:
                throw logic_error("ERROR: for now, max() is only implemented for integers");
            case BOOL:
                throw logic_error("ERROR: for now, max() is only implemented for integers");
            case CHAR:
                throw logic_error("ERROR: for now, max() is only implemented for integers");
            case INT:
                mx = max(mx, args[i]->int_value);
            }
        }
        return new jsonObj("", mx);
    }

    static jsonObj *min_obj(vector<jsonObj *> args)
    {
        int mn = INT_MAX;
        if (args[0]->type == ARRAY)
        {
            if (args.size() != 1)
                throw invalid_argument("ERROR: array is not the only argument of max()");
            args = args[0]->arr;
        }
        for (int i = 0; i < args.size(); i++)
        {
            switch (args[i]->type)
            {
            case STRING:
                throw invalid_argument("ERROR: strings not supported in min() operation");
            case OBJECT:
                throw invalid_argument("ERROR: objects not supported in min() operation");
            case NIL:
                throw invalid_argument("ERROR: nulls not supported in min() operation");
            case ARRAY:
                throw invalid_argument("ERROR: arrays not supported in min() operation");
            case DOUBLE:
                throw logic_error("ERROR: for now, min() is only implemented for integers");
            case BOOL:
                throw logic_error("ERROR: for now, min() is only implemented for integers");
            case CHAR:
                throw logic_error("ERROR: for now, min() is only implemented for integers");
            case INT:
                mn = min(mn, args[i]->int_value);
            }
        }
        return new jsonObj("", mn);
    }

    static string obj_to_string(jsonObj *obj)
    {
        string res = "";
        switch (obj->type)
        {
        case INT:
            return to_string(obj->int_value);
        case DOUBLE:
            return to_string(obj->double_value);
        case NIL:
            return "null";
        case BOOL:
            if (obj->int_value)
                return "true";
            return "false";
        case STRING:
            res = "\"";
            for (int i = 0; i < obj->string_value.length(); i++)
            {
                flag temp = 0;
                if (obj->string_value[i] == '\\' && i + 1 != obj->string_value.length())
                    switch (obj->string_value[i + 1])
                    {
                    case 'r':
                        res += '\r';
                        temp = 1;
                        i++;
                        break;
                    case 't':
                        res += '\t';
                        i++;
                        temp = 1;
                        break;
                    case 'n':
                        res += '\n';
                        i++;
                        temp = 1;
                        break;
                    case 'b':
                        res += '\b';
                        i++;
                        temp = 1;
                        break;
                    case '"':
                        res += '\"';
                        i++;
                        temp = 1;
                        break;
                    case '\\':
                        res += '\\';
                        temp = 1;
                        break;
                    // escape sequence unrecognised by GCC, therefore not implemented
                    case '/':
                        break;
                    // a hex notation with u. I had no idea it existed before.
                    case 'u':
                        if (i + 5 != obj->string_value.length() && isxdigit(obj->string_value[i + 2]) && isxdigit(obj->string_value[i + 3]) && isxdigit(obj->string_value[i + 4]) && isxdigit(obj->string_value[i + 5]))
                        {
                            res += (char)(stoi(obj->string_value.substr(i + 2, 4), nullptr, 16));
                            temp = 1;
                            i += 5;
                        }
                        break;
                    default:
                        break;
                    }
                if (!temp)
                    res += obj->string_value[i];
                temp = 0;
            }
            res += '\"';
            return res;
        case ARRAY:
            res = "[";
            cout<<obj->name<<'\n';
            for (int i = 0; i < (obj->arr).size(); i++)
            {
                res += obj_to_string((obj->arr)[i]);
                if (i + 1 != (obj->arr).size())
                    res += ", ";
                else
                    res += "]";
            }
            return res;
        case OBJECT:
            res = "{";
            for (auto i : obj->contents)
            {
                res += "\"" + i.first + "\": ";
                //cout<<i.first<<"\n";
                res += obj_to_string(i.second);
                res += ", ";
            }
            res.pop_back(), res.pop_back();
            res.push_back('}');
            return res;
        case CHAR:
            res = "\'";
            res.push_back((char)(obj->int_value));
            res.push_back('\'');
            return res;
        }
    }

    /// @brief Parses an expression on obj.
    /// @param expression
    /// @param obj The root json object. It is always expected to be of type object (otherwise the evaluation
    ///            would not make sense).
    /// @param ptr A saved pointer to the expression string. Shared and changed between recursion calls.
    /// @return A result object. To be human-readable, it needs to be passed through to_string method.
    static jsonObj *parse_expression(string &expression, jsonObj *obj, int &ptr)
    {
        // we will need it when descending into arrays
        jsonObj *root = obj;

        // args for functions
        vector<jsonObj *> args;

        string str = "";
        func mode = UNDEF;

        // if the array element (or a function argument) is a simple number or not
        flag array_digging_down = 0;

        // we assume there are no whitespaces in expression
        while (ptr != expression.length())
        {

            if (expression[ptr] == '.')
            {
                if (ptr == 0)
                    throw invalid_argument("ERROR: illegal start of expression");
                // if expression[ptr-1]==']' or ')', the type is already prepared (we dealt with an array previously)
                if (expression[ptr - 1] != ']' && expression[ptr - 1] != ')')
                {
                    auto it = (obj->contents).find(str);
                    if (it == (obj->contents).end())
                        throw invalid_argument("ERROR: " + str + " not found in " + obj->name);
                    obj = it->second;
                }
                array_digging_down = 1;
                str = "";
            }
            else if (expression[ptr] == '[')
            {
                if (ptr == 0)
                    throw invalid_argument("ERROR: array call in the beginning of the file impossible (root structure is an object)");
                // if expression[ptr-1]==']' or '), the type is already prepared (we dealt with an array previously)
                if (expression[ptr - 1] != ']' && expression[ptr - 1] != ')')
                {
                    auto it = (obj->contents).find(str);
                    if (it == (obj->contents).end())
                        throw invalid_argument("ERROR: " + str + " not found in " + obj->name);
                    obj = it->second;
                }
                if (obj->type == STRING)
                {
                    obj->test_call_string_array = 1;
                    throw invalid_argument("ERROR: addressing char's does not work for now");
                }
                // no extra preparations
                else if (obj->type == ARRAY)
                {
                }
                else
                    throw invalid_argument("ERROR: Non-array value cannot get an element with array notation");
                array_digging_down = 1;
                ptr++;
                jsonObj *index = parse_expression(expression, root, ptr);
                try
                {
                    if(obj->type==ARRAY)
                        obj = (obj->arr)[index->int_value];
                    
                }
                catch (out_of_range e)
                {
                    throw out_of_range("ERROR: element specified (" + to_string(index->int_value) + ") is out of range");
                }
            }
            else if (expression[ptr] == ']')
            {
                if (!ptr)
                    throw invalid_argument("ERROR: illegal start of an expression");
                // then we should get a simple number
                if (!array_digging_down)
                {
                    try
                    {
                        int val = stoi(str);
                        array_digging_down = 0;
                        return new jsonObj("", val);
                    }
                    catch (invalid_argument e)
                    {
                        throw invalid_argument("ERROR: an integer expected as an index, but " + str + " found");
                    }
                }
                else
                {
                    // if expression[ptr-1]==']' or '), the type is already prepared (we dealt with an array previously)
                    if (expression[ptr - 1] != ']' && expression[ptr - 1] != ')')
                    {
                        auto it = (obj->contents).find(str);
                        if (it == (obj->contents).end())
                            throw invalid_argument("ERROR: " + str + " not found in " + obj->name);
                        obj = it->second;
                    }
                    if (obj->type != INT)
                        throw invalid_argument("ERROR: an integer expected as an index, but " + obj->name + " is not an integer");
                    array_digging_down = 0;
                    return obj;
                }
            }
            else if (expression[ptr] == '(')
            {
                if (str == "max")
                    mode = MAX;
                else if (str == "min")
                    mode = MIN;
                else if (str == "size")
                    mode = SIZE;
                else
                    throw invalid_argument("ERROR: unrecognised function: \"" + str + "\"");
                str = "";

                ptr++;
                args.push_back(parse_expression(expression, root, ptr));
                while (expression[ptr] == ',')
                {
                    ptr++;
                    args.push_back(parse_expression(expression, root, ptr));
                }
                if (expression[ptr] != ')')
                    throw invalid_argument("ERROR: \")\" expected");
                switch (mode)
                {
                case SIZE:
                    obj = size_obj(args);
                    break;
                case MAX:
                    obj = max_obj(args);
                    break;
                case MIN:
                    obj = min_obj(args);
                    break;
                }

                mode = UNDEF;
                args.clear();
            }
            // only designated for sub-calls of parse_expression
            else if (expression[ptr] == ')' || expression[ptr] == ',')
            {
                if (!ptr)
                    throw invalid_argument("ERROR: illegal start of an expression");
                try
                {
                    // if expression[ptr-1]==']', the type is already prepared (we dealt with an array previously)
                    if (expression[ptr - 1] != ']' && expression[ptr - 1] != ')')
                    {
                        auto it = (obj->contents).find(str);
                        if (it == (obj->contents).end())
                            throw invalid_argument("ERROR: " + str + " not found in " + obj->name);
                        obj = it->second;
                    }
                    array_digging_down = 0;
                    return obj;
                    
                    
                }
                // order reversed from ']' procedure as a workaround for unexplainable bug.
                catch (invalid_argument e)
                {
                    try{
                        int val = stoi(str);
                        array_digging_down = 0;
                        return new jsonObj("", val);
                    }
                    catch(invalid_argument g){
                        throw e;
                    } 
                }
            }
            else
                str.push_back(expression[ptr]);
            ptr++;
        }
        if (!ptr)
            throw invalid_argument("ERROR: empty expression");
        if (expression[ptr - 1] != ']' && expression[ptr-1]!=')')
        {
            auto it = (obj->contents).find(str);
            if (it == (obj->contents).end())
                throw invalid_argument("ERROR: " + str + " not found in " + obj->name);
            obj = it->second;
        }
        return obj;
    }

    static jsonObj *parse_obj(string &input_file, string name_obj, int &ptr)
    {
        jsonObj *root = new jsonObj("", OBJECT);
        // it is only possible to fail this condition when entering for the first time.
        // by logic, all other calls of parse_obj have a pointer at the opening bracket
        if (input_file[ptr] != '{')
            throw invalid_argument("ERROR: For the purposes of further evaluation, the root structure must be an object");
        string obj = "";
        string name_sub = "";
        while (ptr != input_file.length())
        {
            ptr++;
            utility_skip_whitespaces(input_file, ptr);

            // every attribute of an object must start with the name
            if (input_file[ptr] != '\"')
                throw invalid_argument("ERROR: Expected the name of the object");
            assert(input_file[ptr] == '\"');
            // name getter
            while (ptr + 1 != input_file.length())
            {
                ptr++;
                // accounting for the situation where \" (escape char) is used
                if (input_file[ptr] == '\"' && input_file[ptr - 1] != '\\')
                {
                    ptr++;
                    utility_skip_whitespaces(input_file, ptr);
                    // assert the name of the attribute is correct
                    if (input_file[ptr] != ':')
                        throw invalid_argument("ERROR: the name of the attribute \"" + obj + "\" must be followed by a semicolon");
                    break;
                }
                name_sub.push_back(input_file[ptr]);
            }
            ptr++;
            utility_skip_whitespaces(input_file, ptr);
            try
            {
                if (input_file[ptr] == '[')
                {
                    (root->contents)[name_sub] = jsonObj::parse_arr(input_file, name_sub, ptr);
                    ptr++;
                    name_sub.clear();
                }
                else if (input_file[ptr] == '{')
                {
                    (root->contents)[name_sub] = jsonObj::parse_obj(input_file, name_sub, ptr);
                    ptr++;
                    name_sub.clear();
                }
                else
                {
                    // for strings ("...",)
                    if (input_file[ptr] == '\"')
                    {
                        while (ptr + 1 != input_file.length())
                        {
                            obj.push_back(input_file[ptr]);
                            ptr++;
                            // account for in-string ", situation
                            if (input_file[ptr - 1] == '\"' && input_file[ptr - 2] != '\\' && (input_file[ptr] == ',' || input_file[ptr] == ';' || input_file[ptr] == ' ' || input_file[ptr] == '\r' || input_file[ptr] == '\n' || input_file[ptr] == '\t'))
                            {
                                utility_skip_whitespaces(input_file, ptr);
                                break;
                            }
                        }
                    }
                    // for numbers, booleans, nulls (obj...,)
                    else
                    {
                        while (ptr + 1 != input_file.length())
                        {
                            if (input_file[ptr] == '}' || input_file[ptr] == ',' || input_file[ptr] == ' ' || input_file[ptr] == '\r' || input_file[ptr] == '\n' || input_file[ptr] == '\t')
                            {
                                utility_skip_whitespaces(input_file, ptr);
                                break;
                            }
                            obj.push_back(input_file[ptr]);
                            ptr++;
                        }
                    }

                    try
                    {
                        int val = stoi(obj);
                        (root->contents)[name_sub] = new jsonObj(name_sub, (int)val);
                        obj.clear();
                        name_sub.clear();
                    }
                    catch (invalid_argument e)
                    {
                        try
                        {
                            double val = stod(obj);
                            (root->contents)[name_sub] = new jsonObj(name_sub, (double)val);
                            obj.clear();
                            name_sub.clear();
                        }
                        catch (invalid_argument e)
                        {
                            // no value
                            if (obj.empty())
                                throw invalid_argument("ERROR: attribute \"" + name_sub + "\" does not have a value");
                            // null
                            if (obj == "null")
                                (root->contents)[name_sub] = new jsonObj(name_sub, NIL);
                            // bool true
                            else if (obj == "true")
                                (root->contents)[name_sub] = new jsonObj(name_sub, true);
                            // bool false
                            else if (obj == "false")
                                (root->contents)[name_sub] = new jsonObj(name_sub, false);
                            // string
                            else if (obj[0] == '\"' && obj[obj.length() - 1] == '\"')
                                (root->contents)[name_sub] = new jsonObj(name_sub, obj.substr(1, obj.length() - 2));
                            else
                            {
                                throw invalid_argument("ERROR: unrecognised token \"" + obj + "\"\n");
                            }
                            obj.clear();
                            name_sub.clear();
                        }
                        catch (out_of_range e)
                        {
                            throw invalid_argument("ERROR: floating point number " + obj + " out of range\n");
                        }
                    }
                    catch (out_of_range &e)
                    {
                        throw invalid_argument("ERROR: integer " + obj + "out of range\n");
                    }
                }

                utility_skip_whitespaces(input_file, ptr);
                // end of object
                if (input_file[ptr] == '}')
                    return root;
            }
            catch (out_of_range oor)
            {
                throw invalid_argument("ERROR: unexpected end of file\n");
            }
        }
        return root;
    }

    static jsonObj *parse_arr(string &input_file, string name_arr, int &ptr)
    {
        jsonObj *root = new jsonObj("", ARRAY);
        string obj = "";
        int arr_size = 0;
        while (ptr != input_file.length())
        {
            ptr++;
            utility_skip_whitespaces(input_file, ptr);
            try
            {
                if (input_file[ptr] == '[')
                {
                    (root->arr).push_back(jsonObj::parse_arr(input_file, "", ptr));
                    ptr++;
                }
                else if (input_file[ptr] == '{')
                {
                    (root->arr).push_back(jsonObj::parse_obj(input_file, "", ptr));
                    ptr++;
                }
                else
                {
                    // for strings
                    if (input_file[ptr] == '\"')
                    {
                        while (ptr + 1 != input_file.length())
                        {
                            obj.push_back(input_file[ptr]);
                            ptr++;
                            // account for in-string ", situation
                            if (input_file[ptr - 1] == '\"' && input_file[ptr - 2] != '\\' && (input_file[ptr] == ',' || input_file[ptr] == ';' || input_file[ptr] == ' ' || input_file[ptr] == '\r' || input_file[ptr] == '\n' || input_file[ptr] == '\t'))
                            {
                                utility_skip_whitespaces(input_file, ptr);
                                break;
                            }
                        }
                    }
                    // for bools, numbers, nulls
                    else
                        while (ptr != input_file.length())
                        {
                            if (input_file[ptr] == ']' || input_file[ptr] == ',' || input_file[ptr] == ' ' || input_file[ptr] == '\r' || input_file[ptr] == '\n' || input_file[ptr] == '\t')
                            {
                                utility_skip_whitespaces(input_file, ptr);
                                break;
                            }
                            obj.push_back(input_file[ptr]);
                            ptr++;
                        }
                    try
                    {
                        int val = stoi(obj);
                        (root->arr).push_back(new jsonObj("", val));
                        obj.clear();
                    }
                    catch (invalid_argument e)
                    {
                        try
                        {
                            double val = stod(obj);
                            (root->arr).push_back(new jsonObj("", val));
                            obj.clear();
                        }
                        catch (invalid_argument e)
                        {
                            // empty string
                            if (obj.empty())
                                throw invalid_argument("ERROR: element " + to_string(arr_size) + " of the array does not have a value");
                            if (obj == "null")
                                (root->arr).push_back(new jsonObj("", NIL));
                            else if (obj == "true")
                                (root->arr).push_back(new jsonObj("", true));
                            else if (obj == "false")
                                (root->arr).push_back(new jsonObj("", false));
                            else if (obj[0] == '\"' && obj[obj.length() - 1] == '\"')
                                (root->arr).push_back(new jsonObj("", obj.substr(1, obj.length() - 2)));
                            else
                            {
                                throw invalid_argument("ERROR: unrecognised token \"" + obj + "\"\n");
                            }
                            obj.clear();
                        }
                        catch (const out_of_range e)
                        {
                            throw invalid_argument("ERROR: floating point number " + obj + "out of range\n");
                        }
                    }
                    catch (out_of_range e)
                    {
                        throw invalid_argument("ERROR: integer " + obj + "out of range\n");
                    }
                }
                arr_size++;
                utility_skip_whitespaces(input_file, ptr);
                // end of array
                if (input_file[ptr] == ']')
                    return root;
            }
            catch (out_of_range oor)
            {
                throw invalid_argument("ERROR: Unexpected end of file\n");
            }
        }
        return root;
    }

    ~jsonObj()
    {
    }
};

signed main(int argc, char *argv[])
{
    // there are some symbols in ChinookData.json that char alone cannot cope with. 
    setlocale(LC_ALL, "en_US.UTF-8");
    ifstream fin;
    string json = "";
    string inter = "";
    try
    {
        fin.open(argv[1]);
    }
    catch (const char *msg)
    {
        cout << msg << '\n';
    }
    if (fin.is_open())
    {
        while (fin.good())
        {
            getline(fin, inter);
            json.append(inter);
        }
    }
    int ptr = 0;
    int clk = clock();
    try
    {
        jsonObj *parsed = jsonObj::parse_obj(json, "", ptr);
        jsonObj::obj_to_string(parsed);
        cout << "Execution time (parsing stage): " << clock() - clk << '\n';
        string expression = argv[2];
        ptr = 0;
        int clk_inter = clock();
        jsonObj *query = jsonObj::parse_expression(expression, parsed, ptr);
        cout << "Execution time (evaluation stage): " << clock() - clk_inter << '\n';
        _sleep(3000);
        cout << jsonObj::obj_to_string(query) << '\n';
        
    }
    catch (const char *msg)
    {
        cout << msg << '\n';
    }
}

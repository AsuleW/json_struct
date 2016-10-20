/*
 * Copyright © 2016 Jorgen Lind
 *
 * Permission to use, copy, modify, distribute, and sell this software and its
 * documentation for any purpose is hereby granted without fee, provided that
 * the above copyright notice appear in all copies and that both that copyright
 * notice and this permission notice appear in supporting documentation, and
 * that the name of the copyright holders not be used in advertising or
 * publicity pertaining to distribution of the software without specific,
 * written prior permission.  The copyright holders make no representations
 * about the suitability of this software for any purpose.  It is provided "as
 * is" without express or implied warranty.
 *
 * THE COPYRIGHT HOLDERS DISCLAIM ALL WARRANTIES WITH REGARD TO THIS SOFTWARE,
 * INCLUDING ALL IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS, IN NO
 * EVENT SHALL THE COPYRIGHT HOLDERS BE LIABLE FOR ANY SPECIAL, INDIRECT OR
 * CONSEQUENTIAL DAMAGES OR ANY DAMAGES WHATSOEVER RESULTING FROM LOSS OF USE,
 * DATA OR PROFITS, WHETHER IN AN ACTION OF CONTRACT, NEGLIGENCE OR OTHER
 * TORTIOUS ACTION, ARISING OUT OF OR IN CONNECTION WITH THE USE OR PERFORMANCE
 * OF THIS SOFTWARE.
 */

#include "json_tools.h"

#include "assert.h"

const char json_data1[] = u8R"({
    "StringNode" : "Some test data",
    "NumberNode" : 4676.4,
    "BooleanTrue" : true,
    "BooleanFalse" : false,
    "TestStruct" : {
        "SubString" : "Some other string",
        "SubNumber" : 500,
        "Array" : [
            5,	
            6,
            3,
            6
        ],
        "optional_float" : 300,
        "this_property_does_not_exist" : true
    },
    "OptionalButWithData" : [ 17.5 ],
    "subStruct2" : {
        "Field1" : 4,
        "Field2" : true
    }
    })";

struct SubStruct
{
    std::string SubString;
    int SubNumber;
    std::vector<double> Array;
    JT::OptionalChecked<float> optional_float;
    JT::OptionalChecked<double> optional_double;
    JT::Optional<double> optional_with_value = 4.5;
    JT_STRUCT(JT_MEMBER(SubString),
              JT_MEMBER(SubNumber),
              JT_MEMBER(Array),
              JT_MEMBER(optional_float),
              JT_MEMBER(optional_with_value));
};

struct SubStruct2
{
    float Field1;
    bool Field2;
    JT_STRUCT(JT_MEMBER(Field1),
              JT_MEMBER(Field2));
};

struct JsonData1
{
    std::string StringNode;
    double NumberNode;
    bool BooleanTrue;
    bool BooleanFalse;
    JT::Optional<int> OptionalInt;
    SubStruct TestStruct;
    JT::Optional<std::vector<double>> OptionalButWithData;
    float unassigned_value;
    std::unique_ptr<SubStruct2> subStruct2;

    JT_STRUCT(JT_MEMBER(StringNode),
              JT_MEMBER(NumberNode),
              JT_MEMBER(BooleanTrue),
              JT_MEMBER(BooleanFalse),
              JT_MEMBER(OptionalInt),
              JT_MEMBER(TestStruct),
              JT_MEMBER(OptionalButWithData),
              JT_MEMBER(unassigned_value),
              JT_MEMBER(subStruct2));

};

struct SimpleData
{
    std::string member1 = "Foobar";
    float member2 = 45;
    JT_STRUCT(JT_MEMBER(member1)
             ,JT_MEMBER(member2));
};


static int check_json_tree_nodes()
{
    JT::ParseContext context = JT::makeParseContextForData(json_data1, sizeof(json_data1));
    JsonData1 data = JT::parseData<JsonData1>(context);

    for (double x : data.TestStruct.Array)
        fprintf(stderr, "x is %f\n", x);

    fprintf(stderr, "optional with default value %f\n", data.TestStruct.optional_with_value());
    data.TestStruct.optional_with_value = 5;
    fprintf(stderr, "optional with default value second %f\n", data.TestStruct.optional_with_value());
    JT_ASSERT(data.StringNode == "Some test data");
    JT_ASSERT(context.error == JT::Error::NoError);

    SimpleData simpleData;
    std::string json = JT::serializeStruct(data);
    fprintf(stderr, "%s\n", json.c_str());
    return 0;
}

const char json_data2[] = u8R"({
    "some_int" : 4,
    "sub_object" : {
        "more_data" : "some text",
        "a_float" : 1.2,
        "boolean_member" : false
    }
})";

template<typename T>
struct OuterStruct
{
    int some_int;
    T sub_object;

    JT_STRUCT(JT_MEMBER(some_int),
              JT_MEMBER(sub_object));
};

struct SubObject
{
    std::string more_data;
    float a_float;
    bool boolean_member;

    JT_STRUCT(JT_MEMBER(more_data),
              JT_MEMBER(a_float),
              JT_MEMBER(boolean_member));
};

static int check_json_tree_template()
{
    JT::ParseContext context = JT::makeParseContextForData(json_data2, sizeof(json_data2));
    OuterStruct<SubObject> data = JT::parseData<OuterStruct<SubObject>>(context);
    JT_ASSERT(data.sub_object.more_data == "some text");
    return 0;
};

int main(int, char **)
{
    check_json_tree_nodes();
    check_json_tree_template();
    return 0;
}
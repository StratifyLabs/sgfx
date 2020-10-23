
#include <cstdio>

#include "chrono.hpp"
#include "fs.hpp"
#include "printer.hpp"
#include "sys.hpp"
#include "var.hpp"

#include "json.hpp"

#include "test/Test.hpp"

class UnitTest : public test::Test {
public:

  UnitTest(var::StringView name) : test::Test(name) {}

  bool execute_class_api_case() {

    if (!array_case()) {
      return false;
    }

    if (!object_case()) {
      return false;
    }

    if (!value_case()) {
      return false;
    }

    if (!document_case()) {
      return false;
    }

    return true;
  }

  bool array_case() {

    JsonArray array = JsonArray()
                          .append(JsonString("string"))
                          .append(JsonInteger(5))
                          .append(JsonReal(2.5f))
                          .append(JsonTrue())
                          .append(JsonFalse())
                          .append(JsonNull());

    TEST_ASSERT(array.insert(1, JsonString("at1")).at(1).to_string() == "at1");

    return true;
  }

  bool document_case() {

    {
      JsonObject object = JsonObject()
                              .insert("string", JsonString("string"))
                              .insert("integerString", JsonString("100"))
                              .insert("integer", JsonInteger(10))
                              .insert("integerZero", JsonInteger(0))
                              .insert("real", JsonReal(2.2f))
                              .insert("realString", JsonString("2.2f"))
                              .insert("realZero", JsonReal(0.0f))
                              .insert("true", JsonTrue())
                              .insert("false", JsonFalse())
                              .insert("null", JsonNull())
                              .insert("trueString", JsonString("true"))
                              .insert("array", JsonArray()
                                                   .append(JsonString("string"))
                                                   .append(JsonInteger(5))
                                                   .append(JsonReal(2.5f))
                                                   .append(JsonTrue())
                                                   .append(JsonFalse())
                                                   .append(JsonNull()));

      TEST_ASSERT(JsonDocument()
                      .save(object, File(File::IsOverwrite::yes, "test.json"))
                      .is_success());

      {
        JsonObject load_object =
            JsonDocument().load(File("test.json", OpenMode::read_only()));

        TEST_ASSERT(is_success());
        TEST_ASSERT(load_object.is_object());
      }

      {
        JsonObject load_object =
            JsonDocument()
                .load(File("test2.json", OpenMode::read_only()))
                .to_object();
        API_RESET_ERROR();
        TEST_ASSERT(load_object.is_valid() == false);
        TEST_ASSERT(load_object.is_object() == false);
        TEST_ASSERT(load_object.is_empty() == true);
      }
    }
    return true;
  }

  bool value_case() {
    TEST_ASSERT(JsonString().assign("test").is_string());
    TEST_ASSERT(JsonInteger().assign("10").to_integer() == 10);
    TEST_ASSERT(JsonReal().assign("2.5").to_real() == 2.5f);
    TEST_ASSERT(JsonReal().assign("2.5f").to_real() == 2.5f);

    {
      TEST_ASSERT(JsonValue().is_valid() == false);
      TEST_ASSERT(JsonObject().is_valid() == true);
      TEST_ASSERT(JsonObject().is_object() == true);
      TEST_ASSERT(JsonArray().is_valid() == true);
      TEST_ASSERT(JsonArray().is_array() == true);
      TEST_ASSERT(JsonString().is_valid() == true);
      TEST_ASSERT(JsonString().is_string() == true);
      TEST_ASSERT(JsonInteger().is_valid() == true);
      TEST_ASSERT(JsonInteger().is_integer() == true);
      TEST_ASSERT(JsonReal().is_valid() == true);
      TEST_ASSERT(JsonReal().is_real() == true);
      TEST_ASSERT(JsonTrue().is_valid() == true);
      TEST_ASSERT(JsonTrue().is_true() == true);
      TEST_ASSERT(JsonFalse().is_valid() == true);
      TEST_ASSERT(JsonFalse().is_false() == true);
      TEST_ASSERT(JsonNull().is_valid() == true);
      TEST_ASSERT(JsonNull().is_null() == true);
    }

    return true;
  }

  bool object_case() {

    PrinterObject po(printer(), "object");
    {
      JsonObject object = JsonObject()
                              .insert("string", JsonString("string"))
                              .insert("integerString", JsonString("100"))
                              .insert("integer", JsonInteger(10))
                              .insert("integerZero", JsonInteger(0))
                              .insert("real", JsonReal(2.2f))
                              .insert("realString", JsonString("2.2f"))
                              .insert("realZero", JsonReal(0.0f))
                              .insert("true", JsonTrue())
                              .insert("false", JsonFalse())
                              .insert("null", JsonNull())
                              .insert("trueString", JsonString("true"))
                              .insert("array", JsonArray()
                                                   .append(JsonString("string"))
                                                   .append(JsonInteger(5))
                                                   .append(JsonReal(2.5f))
                                                   .append(JsonTrue())
                                                   .append(JsonFalse())
                                                   .append(JsonNull()));

      JsonObject::KeyList key_list = object.key_list();

      TEST_ASSERT(key_list.count() == 12);
      TEST_ASSERT(key_list.find(CString("string")) == CString("string"));
      TEST_ASSERT(key_list.find(CString("integerString")) ==
                  CString("integerString"));
      TEST_ASSERT(key_list.find(CString("integer")) == CString("integer"));
      TEST_ASSERT(key_list.find(CString("integerZero")) ==
                  CString("integerZero"));
      TEST_ASSERT(key_list.find(CString("real")) == CString("real"));

      TEST_ASSERT(key_list.find(CString("realString")) ==
                  CString("realString"));
      TEST_ASSERT(key_list.find(CString("realZero")) == CString("realZero"));
      TEST_ASSERT(key_list.find(CString("true")) == CString("true"));
      TEST_ASSERT(key_list.find(CString("false")) == CString("false"));
      TEST_ASSERT(key_list.find(CString("null")) == CString("null"));
      TEST_ASSERT(key_list.find(CString("trueString")) ==
                  CString("trueString"));
      TEST_ASSERT(key_list.find(CString("array")) == CString("array"));

      printer().object("object", object);

      TEST_ASSERT(object.at("string").to_cstring() == StringView("string"));
      TEST_ASSERT(object.at("string").to_string() == StringView("string"));
      TEST_ASSERT(object.at("string").to_integer() == 0);
      TEST_ASSERT(object.at("string").to_bool() == false);
      TEST_ASSERT(object.at("string").to_real() == 0.0f);
      TEST_ASSERT(object.at("integerString").to_integer() == 100);
      TEST_ASSERT(object.at("integerString").to_real() == 100.0f);
      TEST_ASSERT(object.at("integerString").to_cstring() == StringView("100"));
      TEST_ASSERT(object.at("integerString").to_string() == StringView("100"));
      TEST_ASSERT(object.at("integer").to_integer() == 10);
      TEST_ASSERT(object.at("integer").to_real() == 10.0f);
      TEST_ASSERT(object.at("real").to_real() == 2.2f);
      TEST_ASSERT(object.at("real").to_integer() == 2);
      TEST_ASSERT(object.at("realString").to_real() == 2.2f);
      TEST_ASSERT(object.at("realString").to_cstring() == StringView("2.2f"));
      TEST_ASSERT(object.at("realString").to_integer() == 2);
      TEST_ASSERT(object.at("true").to_bool() == true);
      TEST_ASSERT(object.at("false").to_bool() == false);
      TEST_ASSERT(object.at("null").to_bool() == false);
      TEST_ASSERT(object.at("trueString").to_bool() == true);
      TEST_ASSERT(object.at("string").to_bool() == false);
      TEST_ASSERT(object.at("array").to_array().at(0).to_cstring() ==
                  StringView("string"));
      TEST_ASSERT(object.at("array").to_array().at(1).to_integer() == 5);
      TEST_ASSERT(object.at("array").to_array().at(2).to_real() == 2.5f);
      TEST_ASSERT(object.at("array").to_array().at(3).to_bool() == true);
      TEST_ASSERT(object.at("array").to_array().at(4).to_bool() == false);
      TEST_ASSERT(object.at("array").to_array().at(5).is_null() == true);
    }
    return true;
  }

private:
};

#include <fstream>
#include <gtest/gtest.h>

#include "csv.h"


// #################### io::LineReader ####################

TEST(LineReaderTest, ReadsLinesCorrectly) {
    // Подготовка тестового файла
    auto fname = "test_lines.txt";
    std::ofstream test_file(fname, std::ios::trunc | std::ios::out);
    test_file << "Line 1\nLine 2\nLine 3";
    test_file.close();

    // Тестирование
    io::LineReader reader(fname); 
    EXPECT_STREQ(reader.next_line(), "Line 1");
    EXPECT_STREQ(reader.next_line(), "Line 2");
    EXPECT_STREQ(reader.next_line(), "Line 3");
    EXPECT_EQ(reader.next_line(), nullptr);  // Конец файла
}

TEST(LineReaderTest, HandlesFileOpenError) {
    EXPECT_THROW({
        io::LineReader reader("nonexistent_file.txt");
    }, io::error::can_not_open_file);
}


TEST(LineReaderTest, HandlesDifferentLineEndings) {
    auto fname = "mixed_endings.txt";
    std::ofstream test_file(fname, std::ios::trunc | std::ios::out);
    test_file << "Unix\nLine\r\nWindows\r\nLine";
    test_file.close();

    io::LineReader reader(fname);
    EXPECT_STREQ(reader.next_line(), "Unix");
    EXPECT_STREQ(reader.next_line(), "Line");
    EXPECT_STREQ(reader.next_line(), "Windows");
    EXPECT_STREQ(reader.next_line(), "Line");
}


// #################### io::CSVReader ####################

TEST(CSVReaderTest, ParsesHeaderAndRows) {
    auto fname = "test_data.csv";
    std::ofstream csv_file(fname, std::ios::trunc | std::ios::out);
    csv_file << "Name,Age\nAlice,25\nBob,30";
    csv_file.close();

    io::CSVReader<2> reader(fname);
    reader.read_header(io::ignore_extra_column, "Name", "Age");

    std::string name;
    int age;
    ASSERT_TRUE(reader.read_row(name, age));
    EXPECT_EQ(name, "Alice");
    EXPECT_EQ(age, 25);

    ASSERT_TRUE(reader.read_row(name, age));
    EXPECT_EQ(name, "Bob");
    EXPECT_EQ(age, 30);
}

TEST(CSVReaderTest, HandlesQuotedFields) {
    auto fname = "quoted.csv";
    std::ofstream csv_file(fname, std::ios::trunc | std::ios::out);
    csv_file << "Text\n" << R"("""World""")";
    csv_file.close();

    io::CSVReader<1, io::trim_chars<' '>, io::double_quote_escape<' ', '"'>> reader(fname);
    reader.read_header(io::ignore_missing_column, "Text");
    std::string text;
    ASSERT_TRUE(reader.read_row(text));
    EXPECT_EQ(text, "\"World\"");
}

TEST(CSVReaderTest, DetectsColumnMismatch) {
    std::ofstream csv_file("invalid.csv", std::ios::trunc | std::ios::out);
    csv_file << "A,B\n1,2,3";
    csv_file.close();

    io::CSVReader<2> reader("invalid.csv");
    reader.read_header(io::ignore_no_column, "A", "B");
    int a, b;
    EXPECT_THROW({
        reader.read_row(a, b);
    }, io::error::too_many_columns);
}


int main(int argc, char** argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
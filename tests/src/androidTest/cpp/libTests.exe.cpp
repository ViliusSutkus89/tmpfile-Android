/*
 * libTests.exe.cpp
 *
 * Copyright (C) 2020 Vilius Sutkus'89
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <https://www.gnu.org/licenses/>.
*/

#include <cerrno>
#include <cstdio>
#include <memory>
#include <random>
#include <string_view>

const std::string_view sampleData = "Hello World";

struct TmpFileAutoClose {
    FILE * fh;

    TmpFileAutoClose() {
        fh = tmpfile();
    }

    bool close() {
        bool retVal = true;
        if (nullptr != fh) {
            retVal = (0 == fclose(fh));
        }
        fh = nullptr;
        return retVal;
    }

    ~TmpFileAutoClose() {
        (void) close();
    }
};

int CASE_open_and_close_tmpfile() {
    TmpFileAutoClose openedFile;

    if (nullptr == openedFile.fh) {
        fprintf(stderr, "Failed to open tmpfile. errno: %d", errno);
        return 1;
    }

    if (!openedFile.close()) {
        fprintf(stderr, "Failed to close previously opened tmpfile. errno: %d", errno);
        return 2;
    }

    return 0;
}

int CASE_write_to_tmpfile() {
    TmpFileAutoClose openedFile;

    size_t written = 0;
    while (written < sampleData.length()) {
        int writtenThisChunk = fwrite(&sampleData.at(written), sizeof(sampleData.at(0)), sampleData.length() - written, openedFile.fh);
        if (-1 == writtenThisChunk) {
            fprintf(stderr, "fwrite failed!");
            return 1;
        }
        written += writtenThisChunk;
    }

    return 0;
}

size_t write_in_chunks(FILE * fh, const char * data, size_t length) {
    size_t written = 0;
    while (written < length) {
        size_t writtenThisChunk = fwrite(&data[written], sizeof(char), length - written, fh);
        if (0 == writtenThisChunk) {
            fprintf(stderr, "fwrite failed!");
            break;
        }
        written += writtenThisChunk;
    }
    return written;
}

size_t read_in_chunks(FILE * fh, char * buffer, size_t expectedLen) {
    size_t haveRead = 0;
    while (haveRead < expectedLen) {
        size_t leftToRead = expectedLen - haveRead;
        int haveReadThisChunk = fread(&buffer[haveRead], sizeof(char), leftToRead, fh);
        if (0 == haveReadThisChunk) {
            fprintf(stderr, "fread failed!");
            break;
        }
        haveRead += haveReadThisChunk;
    }
    return haveRead;
}

int CASE_write_and_readback_from_tmpfile() {
    TmpFileAutoClose openedFile;
    if (sampleData.length() != write_in_chunks(openedFile.fh, sampleData.cbegin(), sampleData.length())) {
        return 1;
    }

    if (0 != fseek(openedFile.fh, 0L, SEEK_SET)) {
        fprintf(stderr, "fseek failed! errno: %d", errno);
        return 2;
    }

    std::unique_ptr<char[]> readBuffer(new char[sampleData.length()]);

    if (sampleData.length() != read_in_chunks(openedFile.fh, readBuffer.get(), sampleData.length())) {
        return 3;
    }

    if (0 != memcmp(sampleData.cbegin(), readBuffer.get(), sampleData.length())) {
        fprintf(stderr, "Data read from tmpfile does not match previously written sample data!");
        return 4;
    }

    return 0;
}

int CASE_write_and_readback_one_Meg() {
    TmpFileAutoClose openedFile;

    constexpr size_t length = 1024 * 1024;

    std::unique_ptr<char[]> randomData(new char[length]);
    std::unique_ptr<char[]> readBuffer(new char[length]);

    std::default_random_engine randomGenerator;
    std::uniform_int_distribution<uint8_t> distribution(0, std::numeric_limits<uint8_t>::max());

    for (size_t i = 0; i < length; i++) {
        randomData[i] = distribution(randomGenerator);
    }

    if (length != write_in_chunks(openedFile.fh, randomData.get(), length)) {
        return 1;
    }

    if (0 != fseek(openedFile.fh, 0L, SEEK_SET)) {
        fprintf(stderr, "fseek failed! errno: %d", errno);
        return 2;
    }

    if (length != read_in_chunks(openedFile.fh, readBuffer.get(), length)) {
        return 3;
    }

    if (0 != memcmp(randomData.get(), readBuffer.get(), length)) {
        fprintf(stderr, "Data read from tmpfile does not match previously written sample data!");
        return 4;
    }

    return 0;
}

int CASE_write_and_readback_sixty_four_Megs() {
    TmpFileAutoClose openedFile;

    constexpr size_t length = 64 * 1024 * 1024;

    std::unique_ptr<char[]> randomData(new char[length]);
    std::unique_ptr<char[]> readBuffer(new char[length]);

    std::default_random_engine randomGenerator;
    std::uniform_int_distribution<uint8_t> distribution(0, std::numeric_limits<uint8_t>::max());

    for (size_t i = 0; i < length; i++) {
        randomData[i] = distribution(randomGenerator);
    }

    if (length != write_in_chunks(openedFile.fh, randomData.get(), length)) {
        return 1;
    }

    if (0 != fseek(openedFile.fh, 0L, SEEK_SET)) {
        fprintf(stderr, "fseek failed! errno: %d", errno);
        return 2;
    }

    if (length != read_in_chunks(openedFile.fh, readBuffer.get(), length)) {
        return 3;
    }

    if (0 != memcmp(randomData.get(), readBuffer.get(), length)) {
        fprintf(stderr, "Data read from tmpfile does not match previously written sample data!");
        return 4;
    }

    return 0;
}

int CASE_open_and_not_close_tmpfile() {
    FILE * fh = tmpfile();

    if (nullptr == fh) {
        fprintf(stderr, "Failed to open tmpfile. errno: %d", errno);
        return 1;
    }

    return 0;
}


int main(int argc, const char * argv[]) {
    if (argc != 2) {
        return -1;
    }
    std::string_view testCase = argv[1];

    if ("1" == testCase) {
        return CASE_open_and_close_tmpfile();
    } else if ("2" == testCase) {
        return CASE_write_to_tmpfile();
    } else if ("3" == testCase) {
        return CASE_write_and_readback_from_tmpfile();
    } else if ("4" == testCase) {
        return CASE_write_and_readback_one_Meg();
    } else if ("5" == testCase) {
        return CASE_write_and_readback_sixty_four_Megs();
    } else if ("6" == testCase) {
        return CASE_open_and_not_close_tmpfile();
    }

    return -2;
}

#include "cmdlineparser.h"
#include <assert.h>
#include <boost/lexical_cast.hpp>
#include <boost/filesystem.hpp>
#include <chrono>
#include <fstream>
#include <iomanip>
#include <iostream>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/wait.h>
#include <zlib.h>
extern int def(FILE* src, FILE* dest, int level);
extern int inf(FILE* src, FILE* dest);
extern void zerr(int ret);

// Data Size definitions
constexpr std::size_t KiB = 1024;
constexpr std::size_t MiB = KiB * 1024;
constexpr std::size_t GiB = MiB * 1024;

// CLI Option global params
constexpr std::uint16_t NUM_ITER = 1;
constexpr std::uint8_t MAX_CR = 20;
std::uint8_t verbosity = 0;
uint16_t num_iter = NUM_ITER;
bool chunk_mode = false;
int compressLevel = Z_DEFAULT_COMPRESSION;
#define ERRCHCK(call)                                        \
    try {                                                    \
        call;                                                \
    } catch (std::exception const& e) {                      \
        std::cerr << "\n";                                   \
        std::cerr << "EXCEPTION: " << e.what() << std::endl; \
        std::cerr << "TEST FAILED\n";                        \
        return EXIT_FAILURE;                                 \
    }

static uint64_t get_file_size(std::ifstream& file) {
    file.seekg(0, file.end);
    uint64_t file_size = file.tellg();
    file.seekg(0, file.beg);
    return file_size;
}

void fileSanityCheck(const std::string& inFile_name) {
    boost::filesystem::path p(inFile_name.c_str());
    if (boost::filesystem::exists(p)) {
        if (boost::filesystem::is_directory(p)) {
            throw std::runtime_error("Directories are not supported ");
        }
    } else {
        throw std::runtime_error("Unable to Open Input File: " + inFile_name);
    }
}

bool get_list_filenames(std::string& filelist, std::vector<std::string>& fname_vec) {
    std::ifstream infilelist(filelist.c_str());
    if (!infilelist) throw std::runtime_error("Unable to Open File List: " + filelist);

    std::string line;

    // Pick File names
    while (std::getline(infilelist, line)) fname_vec.push_back(line);

    infilelist.close();

    if (fname_vec.empty()) throw std::runtime_error("Failed to find files under " + filelist);

    return EXIT_SUCCESS;
}

int chunkDeflate(std::string& inFile, std::string& outFile, int level) {
    FILE* source;
    FILE* dest;

    fileSanityCheck(inFile);

    // Open input file
    source = fopen(inFile.c_str(), "rb");
    if (source == NULL) throw std::runtime_error("Unable to open input file ");

    // Open output file
    dest = fopen(outFile.c_str(), "wb");
    if (dest == NULL) throw std::runtime_error("Unable to open output file ");

    // zpipe.c based deflate called in chunks
    int ret = def(source, dest, compressLevel);

    fclose(source);
    fclose(dest);
    return ret;
}

void zlib_compress(std::string& inFile_name) {
    std::string outFile_name = inFile_name;
    outFile_name = outFile_name + ".zlib";

    fileSanityCheck(inFile_name);

    // Open input file
    std::ifstream inFile(inFile_name, std::ifstream::binary);
    if (!inFile) throw std::runtime_error("Unable to Open Input File: " + inFile_name);
    size_t input_size = get_file_size(inFile);

    std::vector<uint8_t> in;
    std::vector<uint8_t> compress_out;
    uint64_t compress_len = 0;

    if (!chunk_mode) {
        // File operations
        // Input and Output buffers
        in.resize(input_size);
        compress_out.resize(2 * input_size);
        compress_len = 2 * input_size;

        // Read input data from file
        inFile.read((char*)in.data(), input_size);

        // Close file
        inFile.close();
    }

    int ret = 0;
    std::chrono::duration<double, std::nano> compress_API_time_ns_1(0);
    std::chrono::duration<double, std::milli> compress_API_time_ms_1(0);
    auto compress_API_start = std::chrono::high_resolution_clock::now();
    for (int i = 0; i < num_iter; i++) {
        if (chunk_mode) {
            ret = chunkDeflate(inFile_name, outFile_name, Z_DEFAULT_COMPRESSION);
        } else {
            // Call to libz (compress API) located in
            // thirdParty/zlib-1.2.7/compress.c
            ret = compress2(compress_out.data(), &compress_len, in.data(), input_size, compressLevel);
        }
        if (ret != Z_OK) zerr(ret);
    }

    auto compress_API_end = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration<double, std::nano>(compress_API_end - compress_API_start);
    auto duration_ms = std::chrono::duration<double, std::milli>(compress_API_end - compress_API_start);
    compress_API_time_ns_1 = duration / num_iter;
    compress_API_time_ms_1 = duration_ms / num_iter;
    float throughput_in_mbps_1 = (float)input_size * 1000 / compress_API_time_ns_1.count();

    if (verbosity >= 1) {
        if (chunk_mode) {
            std::ifstream outFile(outFile_name.c_str(), std::ifstream::binary);
            if (!outFile) throw std::runtime_error("Unable to Open Output File: " + outFile_name);
            compress_len = get_file_size(outFile);
            outFile.close();
        }
        std::cout << "#Iterations " << num_iter;
        std::cout << ", Input File: " << inFile_name;
        std::cout << ", Output File: " << outFile_name << std::endl;
        std::cout << "Input Size: " << input_size / MiB << "MiB ";
        std::cout << " Compressed Size: " << compress_len / MiB << "MiB ";
        std::cout << " CR: " << std::fixed << std::setprecision(2) << (float)input_size / compress_len;
        std::cout << " PID: " << getpid();
        std::cout << " PPID: " << getppid();
        std::cout << " API: " << std::fixed << std::setprecision(2) << throughput_in_mbps_1 << "MB/s";
        std::cout << " Time: " << std::fixed << std::setprecision(2) << compress_API_time_ms_1.count() << "ms"
                  << std::endl;
        std::cout << "\n";
    }
    if (!chunk_mode) {
        std::ofstream outFile(outFile_name.c_str(), std::ofstream::binary);
        if (!outFile) throw std::runtime_error("Unable to Open Output File: " + outFile_name);
        outFile.write((char*)compress_out.data(), compress_len);
        outFile.close();
    }
}

void zlib_uncompress(std::string& inFile_name) {
    std::string dec_in = inFile_name;
    std::string dec_out = dec_in + ".raw";

    fileSanityCheck(inFile_name);

    // Open input file
    std::ifstream inFile(dec_in.c_str(), std::ifstream::binary);
    if (!inFile) throw std::runtime_error("Unable to Open Input File: " + dec_in);
    size_t input_size = get_file_size(inFile);
    size_t output_size = input_size * MAX_CR;

    // Input and Output buffers
    std::vector<uint8_t> in(input_size);
    std::vector<uint8_t> out(output_size);
    uint64_t uncompress_len = output_size;

    in.resize(input_size);
    out.resize(output_size);

    // Read data from input file
    inFile.read((char*)in.data(), input_size);

    // Close file
    inFile.close();

    int ret = 0;
    std::chrono::duration<double, std::milli> compress_API_time_ms_1(0);
    std::chrono::duration<double, std::nano> compress_API_time_ns_1(0);
    auto compress_API_start = std::chrono::high_resolution_clock::now();

    for (int i = 0; i < num_iter; i++) {
        // Call to libz (uncompress API) located in
        // thirdParty/zlib-1.2.7/uncompress.c
        ret = uncompress(out.data(), &uncompress_len, in.data(), input_size);
        if (ret != Z_OK) {
            if (ret == Z_BUF_ERROR) throw std::runtime_error("Output Buffer Size Exceeds limits ... ");
            zerr(ret);
        }
    }

    auto compress_API_end = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration<double, std::nano>(compress_API_end - compress_API_start);
    auto duration_ms = std::chrono::duration<double, std::milli>(compress_API_end - compress_API_start);
    compress_API_time_ns_1 = duration / num_iter;
    compress_API_time_ms_1 = duration_ms / num_iter;

    float throughput_in_mbps_1 = (float)uncompress_len * 1000 / compress_API_time_ns_1.count();

    if (verbosity >= 1) {
        std::cout << "#Iterations " << num_iter;
        std::cout << ", Input File: " << inFile_name;
        std::cout << ", Output File: " << dec_out << std::endl;
        std::cout << "Input Size: " << input_size / MiB << "MiB ";
        std::cout << " UnCompressed Size: " << uncompress_len / MiB << "MiB ";
        std::cout << " PID: " << getpid();
        std::cout << " PPID: " << getppid();
        std::cout << " API: " << std::fixed << std::setprecision(2) << throughput_in_mbps_1 << "MB/s";
        std::cout << " Time: " << std::fixed << std::setprecision(2) << compress_API_time_ms_1.count() << "ms"
                  << std::endl;
        std::cout << "\n";
    }

    // Open output file
    std::ofstream outFile(dec_out.c_str(), std::ofstream::binary);
    if (!outFile) throw std::runtime_error("Unable to Open Output File: " + dec_out);

    // Write data into output file
    outFile.write((char*)out.data(), uncompress_len);

    // Close files
    outFile.close();
}

bool verify(std::string& verify) {
    printf("\n");
    printf("ZLIB Compression - LIBZSO \n\n");

    // Compress
    ERRCHCK(zlib_compress(verify));

    printf("\n");
    printf("ZLIB DeCompression - LIBZSO \n\n");

    std::string dec_in = verify + ".zlib";

    // Decompress
    ERRCHCK(zlib_uncompress(dec_in));

    std::string in = verify.c_str();
    std::string out = dec_in + ".raw";
    std::string command = "cmp " + in + " " + out;

    // Compare input/output
    int ret = system(command.c_str());

    printf("\n");
    if (ret == 0) {
        std::cout << "TEST PASSED" << std::endl;
        return ret;
    } else {
        std::cout << "TEST FAILED" << std::endl;
        return ret;
    }
}

int main(int argc, char* argv[]) {
    sda::utils::CmdLineParser parser;
    parser.addSwitch("--decompress", "-d", "Decompress the specified files", "");
    parser.addSwitch("--test", "-t",
                     "Compress followed by Decompress to test that decompression produces original files", "");
    parser.addSwitch("--verbosity", "-v", "Verbose Mode [0|1]", "0");
    parser.addSwitch(
        "--no_acceleration", "-n",
        "Do not use Xilinx Alveo, use CPU cores instead. By default Alveo acceleration is used[FPGA=1|CPU=0]", "1");
#ifdef XILINX_DEBUG
    parser.addSwitch("--compression_level", "-cl",
                     "Compression Level Settings [No Compression=0|Best Speed=1|Best Compression=9|Default=-1]"
                     "-1");
    parser.addSwitch("--chunk_mode", "-cm", "Chunk Mode: Input file divided and deflate/inflate called in a loop [0|1]",
                     "0");
    parser.addSwitch("--cu_id", "-cu", "Compute Unit", "1");
    parser.addSwitch("--c_file_list", "-cfl", "Compression List of Files", "");
    parser.addSwitch("--d_file_list", "-dfl", "Decompression List of Files", "");
    parser.addSwitch("--file_list", "-l", "File List (Compress, Decompress and Validation)", "");
    parser.addSwitch("--num_iter", "-nitr", "Number of Iterations", "1");
#endif
    if (argc == 1) {
        parser.printHelp();
        exit(EXIT_FAILURE);
    }
    parser.parse(argc, argv);
    std::string decompress_mod = parser.value("decompress");
    std::string verify_mod = parser.value("test");
    std::string verbosity_mod = parser.value("verbosity");
    std::string noaccel = parser.value("no_acceleration");
    if (!verbosity_mod.empty()) ERRCHCK(verbosity = boost::lexical_cast<uint16_t>(verbosity_mod));

    // Choose between CPU/FPGA
    if (!noaccel.empty()) setenv("XILINX_NO_ACCEL", noaccel.c_str(), 1);

#ifdef XILINX_DEBUG
    std::string compLevel = parser.value("compression_level");
    std::string chunkmode = parser.value("chunk_mode");
    std::string cfilelist = parser.value("c_file_list");
    std::string dfilelist = parser.value("d_file_list");
    std::string cuid = parser.value("cu_id");
    std::string filelist = parser.value("file_list");
    std::string numitr = parser.value("num_iter");
    if (!chunkmode.empty()) ERRCHCK(chunk_mode = boost::lexical_cast<bool>(chunkmode));
    if (!compLevel.empty()) ERRCHCK(compressLevel = boost::lexical_cast<int>(compLevel));
#ifndef RANDCU
    setenv("XILINX_CU_ID", cuid.c_str(), 1);
#endif
    if (!numitr.empty()) num_iter = boost::lexical_cast<uint16_t>(numitr);
#endif

    if (!decompress_mod.empty()) {
        // Compression with no option for it
        std::vector<std::string> fname_vec;
        uint8_t sIdx = 0;
        if (!(strcmp(argv[1], "-d")))
            sIdx = 2;
        else
            sIdx = 4;
        for (auto i = sIdx; i < argc; i++) {
            if (strcmp(argv[i], "-v"))
                fname_vec.push_back(argv[i]);
            else {
                if (!verbosity_mod.empty()) verbosity = boost::lexical_cast<uint16_t>(verbosity_mod);
                break;
            }
        }
        for (auto fr : fname_vec) ERRCHCK(zlib_uncompress(fr));
    } else if (!verify_mod.empty()) {
        verbosity = 1;
        ERRCHCK(verify(verify_mod));
#ifdef XILINX_DEBUG
    } else if (!cfilelist.empty() || !dfilelist.empty()) {
        // By default verbosity enabled
        verbosity = 1;
        // Compression
        if (!cfilelist.empty()) {
            std::vector<std::string> fname_vec;
            ERRCHCK(get_list_filenames(cfilelist, fname_vec));
            for (auto fr : fname_vec) {
                if (fork() == 0) {
                    ERRCHCK(zlib_compress(fr));
                    exit(0);
                }
            }
            for (auto fr : fname_vec) wait(NULL);
        }

        if (!dfilelist.empty()) {
            std::vector<std::string> fname_vec;
            ERRCHCK(get_list_filenames(dfilelist, fname_vec));
            std::string cu_id = "0";
            uint8_t cntr = 0;
            for (auto fr : fname_vec) {
                cu_id = std::to_string(cntr++);
                if (fork() == 0) {
                    std::string temp = fr + ".zlib";
                    ERRCHCK(zlib_uncompress(temp));
                    exit(0);
                }
            }
            for (auto fr : fname_vec) wait(NULL);
        }
    } else if (!filelist.empty()) {
        // By default verbosity enabled
        verbosity = 1;

        std::vector<std::string> fname_vec;
        ERRCHCK(get_list_filenames(filelist, fname_vec));
        std::cout << "======================================" << std::endl;
        std::cout << "           ZLIB Compression           " << std::endl;
        std::cout << "======================================" << std::endl;
        // Compress list of iles
        for (auto file : fname_vec) ERRCHCK(zlib_compress(file));

        std::cout << "======================================" << std::endl;
        std::cout << "           ZLIB DeCompression         " << std::endl;
        std::cout << "======================================" << std::endl;

        // DeCompress list of iles
        for (auto file : fname_vec) {
            std::string temp = file + ".zlib";
            ERRCHCK(zlib_uncompress(temp));
        }

        std::cout << "======================================" << std::endl;
        std::cout << "             Validation               " << std::endl;
        std::cout << "======================================" << std::endl;

        std::cout << "Test Result\t\t"
                  << "File Name" << std::endl;
        // Validation
        for (auto file : fname_vec) {
            std::string outfile = file + ".zlib" + ".raw";
            std::string command = "cmp " + file + " " + outfile;

            // Compare input/output
            int ret = system(command.c_str());

            if (ret == 0) {
                std::cout << "PASSED"
                          << "\t\t" << file << std::endl;
            } else {
                std::cout << "FAILED"
                          << "\t\t" << file << std::endl;
                return 1;
            }
        }
#endif
    } else {
        uint8_t sIdx = 0;
        if ((strcmp(argv[1], "-v")))
            sIdx = 1;
        else
            sIdx = 3;

        // Compression no switch
        std::vector<std::string> fname_vec;
        for (auto i = sIdx; i < argc; i++) {
            if (strcmp(argv[i], "-v"))
                fname_vec.push_back(argv[i]);
            else {
                if (!verbosity_mod.empty()) ERRCHCK(verbosity = boost::lexical_cast<uint16_t>(verbosity_mod));
                break;
            }
        }
        for (auto fr : fname_vec) ERRCHCK(zlib_compress(fr));
    }
    return EXIT_SUCCESS;
}

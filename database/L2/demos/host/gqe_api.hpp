/*
 * Copyright 2019 Xilinx, Inc.
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#ifndef GQE_CFG_H
#define GQE_CFG_H
//#include "ap_int.h"
#include <ap_int.h>
#include "meta_table.hpp"
#include "kernel_command.hpp" 
#include "xf_database/dynamic_alu_host.hpp"
#include "xf_database/enums.hpp"
#include <fstream>
#endif
#ifndef _GQE_API_
#define _GQE_API_

#include <CL/cl_ext_xilinx.h>
#include <xcl2.hpp>

#define XCL_BANK(n) (((unsigned int)(n)) | XCL_MEM_TOPOLOGY)

#define XCL_BANK0 XCL_BANK(0)
#define XCL_BANK1 XCL_BANK(1)
#define XCL_BANK2 XCL_BANK(2)
#define XCL_BANK3 XCL_BANK(3)
#define XCL_BANK4 XCL_BANK(4)
#define XCL_BANK5 XCL_BANK(5)
#define XCL_BANK6 XCL_BANK(6)
#define XCL_BANK7 XCL_BANK(7)
#define XCL_BANK8 XCL_BANK(8)
#define XCL_BANK9 XCL_BANK(9)
#define XCL_BANK10 XCL_BANK(10)
#define XCL_BANK11 XCL_BANK(11)
#define XCL_BANK12 XCL_BANK(12)
#define XCL_BANK13 XCL_BANK(13)
#define XCL_BANK14 XCL_BANK(14)
#define XCL_BANK15 XCL_BANK(15)

long getkrltime(cl::Event e1, cl::Event e2) {
    cl_ulong start, end;
    e1.getProfilingInfo(CL_PROFILING_COMMAND_START, &start);
    e2.getProfilingInfo(CL_PROFILING_COMMAND_START, &end);
    long kerneltime = (end - start) / 1000000;
    return kerneltime;
}
long getkrltime(cl::Event e1_0, cl::Event e1_1, cl::Event e2) {
    cl_ulong start_0, start_1, start, end;
    e1_0.getProfilingInfo(CL_PROFILING_COMMAND_START, &start_0);
    e1_1.getProfilingInfo(CL_PROFILING_COMMAND_START, &start_1);
    e2.getProfilingInfo(CL_PROFILING_COMMAND_START, &end);
    start = start_0 > start_1 ? start_1 : start_0;
    long kerneltime = (end - start) / 1000000;
    return kerneltime;
}
long getkrltime(std::vector<cl::Event> e1, std::vector<cl::Event> e2) {
    cl_ulong start, end;
    e1[0].getProfilingInfo(CL_PROFILING_COMMAND_START, &start);
    e2[0].getProfilingInfo(CL_PROFILING_COMMAND_END, &end);
    for (unsigned int i = 1; i < e1.size(); i++) {
        cl_ulong start1;
        e1[i].getProfilingInfo(CL_PROFILING_COMMAND_START, &start1);
        if (start1 < start) start = start1;
    }
    for (unsigned int i = 1; i < e2.size(); i++) {
        cl_ulong end1;
        e2[i].getProfilingInfo(CL_PROFILING_COMMAND_END, &end1);
        if (end1 > end) end = end1;
    }
    long kerneltime = (end - start) / 1000000;
    return kerneltime;
}
void printkrlTime(cl::Event e1, cl::Event e2, const std::string kinfo = "kernel") {
    std::cout << std::dec << kinfo << " execution time " << getkrltime(e1, e2) << " ms" << std::endl;
}
void printkrlTime(cl::Event e1, cl::Event e2, cl::Event e3, const std::string kinfo = "kernel") {
    std::cout << std::dec << kinfo << " execution time " << getkrltime(e1, e2, e3) << " ms" << std::endl;
}
void printkrlTime(std::vector<cl::Event> e1, std::vector<cl::Event> e2, std::string kinfo = "kernel") {
    std::cout << std::dec << kinfo << " execution time " << getkrltime(e1, e2) << " ms" << std::endl;
}
int64_t gettimestamp() {
    //  std::chrono::steady_clock::duration d = std::chrono::steady_clock::now().time_since_epoch();
    //  std::chrono::microseconds mic = std::chrono::duration_cast<std::chrono::microseconds>(d);
    //  return mic.count();
    std::chrono::time_point<std::chrono::system_clock, std::chrono::milliseconds> tp =
        std::chrono::time_point_cast<std::chrono::milliseconds>(std::chrono::system_clock::now());
    auto tmp = std::chrono::duration_cast<std::chrono::milliseconds>(tp.time_since_epoch());
    int64_t timestamp = tmp.count();
    return timestamp;
}
void print_h_time(struct timeval tv_r_s, struct timeval tv_r_0, struct timeval tv_r_1, const std::string hinfo) {
    std::cout << hinfo << " start time of Host " << tvdiff(&tv_r_s, &tv_r_0) / 1000.0 << " ms" << std::endl;
    std::cout << hinfo << " end time of Host " << tvdiff(&tv_r_s, &tv_r_1) / 1000.0 << " ms" << std::endl;
    std::cout << hinfo << " duration time of Host " << tvdiff(&tv_r_0, &tv_r_1) / 1000.0 << " ms" << std::endl;
}
void print_d_time(cl::Event s, cl::Event e, int64_t base, const std::string kinfo = "kernel", int64_t offset = 0) {
    cl_ulong start, end;
    s.getProfilingInfo(CL_PROFILING_COMMAND_START, &start);
    e.getProfilingInfo(CL_PROFILING_COMMAND_END, &end);
    long stime = (start - base) / 1000000 + offset;
    long etime = (end - base) / 1000000 + offset;
    long duration = (end - start) / 1000000;
    std::cout << std::dec << kinfo << " start time of Device " << stime << " ms" << std::endl;
    std::cout << std::dec << kinfo << " end time of Device " << etime << " ms" << std::endl;
    std::cout << std::dec << kinfo << " duration time of Device " << duration << " ms" << std::endl;
}
template <typename T>
int load_dat(T* data, const std::string& name, const std::string& dir, size_t n, size_t sizeT) {
    if (!data) {
        return -1;
    }

    std::string fn = dir + "/" + name + ".dat";
    FILE* f = fopen(fn.c_str(), "rb");
    if (!f) {
        std::cerr << "ERROR: " << fn << " cannot be opened for binary read." << std::endl;
    }
    // size_t cnt = fread(data, sizeof(T), n, f);
    //std::cout << "SIZE: " << sizeT << std::endl;
    size_t cnt = fread((void*)data, sizeT, n, f);
    fclose(f);
    if (cnt != n) {
        std::cerr << "ERROR: " << cnt << " entries read from " << fn << ", " << n << " entries required." << std::endl;
        return -1;
    }
    return 0;
}

/*template <typename T>
void load_dat(T* data, const std::string& name, const std::string& dir, size_t n) {
    load_dat(data, name, dir, n, sizeof(T));
};*/

class Table {
   public:
    std::string name;
    size_t nrow;
    size_t ncol;
    int npart;
    std::vector<std::string> colsname;
    std::vector<size_t> colswidth;
    std::vector<size_t> isrowid;
    std::vector<size_t> iskdata;

    std::string dir;

    std::vector<size_t> size512;
    cl_mem_ext_ptr_t mext;

    ap_uint<512>* data;
    ap_uint<512>* datak;
    cl::Buffer buffer;

    // Column format -> 64-bit data
    std::vector<cl_mem_ext_ptr_t> mext_col;
    std::vector<ap_uint<512>* > data_col;
    std::vector<cl::Buffer> buffer_col;

    int overlayVersion; //0 being 2020; 1 being 2022

    int partition_num_rows;
    int partition_col_size;

    Table(){};

    Table(std::string name_, size_t nrow_, size_t ncol_, std::string dir_, int num_rows = 0, int col_size = 0) {
        name = name_;
        nrow = nrow_;
        ncol = ncol_;
        npart = 1;
        dir = dir_;
        size512.push_back(0);
        mode = 2;
        overlayVersion = 0;
        partition_num_rows = num_rows;
        partition_col_size = col_size;
        if(partition_num_rows != 0){ // Partitioned Table Columns (BF+Part)
            mode = 4;
        }
    };

    Table(size_t size) {
        size512.push_back(size / 64);
        mode = 3;
        overlayVersion = 0;
    };

    void selectOverlayVersion(int version){
        if (version == 0 || version == 1) {
            overlayVersion = version;
        } else {
            std::cout << "Error: overlay version not supported\n";
        }
    };

    // Add column
    void addCol(std::string columname, size_t width, int isrid = 0, int iskda = 1, int flag = 0) {
        size_t depth = nrow + VEC_LEN * 2 - 1;
        size_t sizeonecol = size_t((width * depth + 64 - 1) / 64);
        size512.push_back(size512.back() + sizeonecol);
        colsname.push_back(columname);
        isrowid.push_back(isrid);
        iskdata.push_back(iskda);
        colswidth.push_back(width);
        if(flag==0){
            mode = 1;
        }
        else if(flag==1){ //BF+Part
            std::cout << name << ": " << mode << std::endl;
            mode = 4;
        }
    };

    //! Load table to CPU memory
    void loadHost() {
        // std::cout<<"load host"<<std::endl;
        for (size_t i = 0; i < ncol; i++) {
            // std::cout<<isrowid[i]<<std::endl;
            if (isrowid[i] == 0) {
                // std::cout<<colsname[i]<<dir<<nrow<<colswidth[i]<<std::endl;
                // std::cout << "???????????" << std::endl;
                int err = load_dat(data + size512[i] + 1, colsname[i], dir, nrow, colswidth[i]);
                if (err) {
                    std::cout << "ERROR loading table from disk" << std::endl;
                };
            } else {
                for (size_t j = 0; j < nrow; j++) {
                    setInt32(j, i, j);
                }
            }
            memcpy(data + size512[i], &nrow, 4);
        };
    };

    //! CPU memory allocation
    void allocateHost() { // col added manually
        // std::cout << name << ": " << mode << std::endl;
        if (mode == 1) {
            data = aligned_alloc<ap_uint<512> >(size512.back());
            data[0] = get_table_header(size512[1], nrow); // TO CHECK
            for (size_t j = 1; j < ncol; j++) {
                data[size512[j]] = 0;
            };
            if (overlayVersion == 1) {
                //allocate column memory in 64-bit
                // resize of the column vector
                // size_t col_depth = (nrow + VEC_LEN - 1)/VEC_LEN;
                size_t depth_overlay2022 = (nrow);
                size_t sizeonecol_overlay2022 = size_t((8 * depth_overlay2022 + 64 - 1) / 64);
                data_col.resize(ncol);
                for (size_t j = 0; j < ncol; j++) {
                    // do each column memory allocation
                    ap_uint<512>* col_data = aligned_alloc<ap_uint<512> >(sizeonecol_overlay2022);
                    memset(col_data, 2, 64 * sizeonecol_overlay2022);
                    data_col[j] = col_data;
                }
                // std::cout << std::endl;
            }
        } else if (mode == 2) { // col added automatically
            size_t depth = (nrow + VEC_LEN);
            size_t sizeonecol = size_t((4 * depth + 64 - 1) / 64);
            for (size_t i = 0; i < ncol; i++) {
                size512.push_back(size512.back() + sizeonecol);
                iskdata.push_back(1);
            };
            data = aligned_alloc<ap_uint<512> >(size512.back());
            data[0] = get_table_header(size512[1], 0); // TO CHECK
            if (overlayVersion == 1) {
                //DONE: allocate column memory in 64-bit
                // alec-added
                // size_t col_depth = (nrow + VEC_LEN - 1)/VEC_LEN;
                size_t depth_overlay2022 = (nrow);
                size_t sizeonecol_overlay2022 = size_t((8 * depth_overlay2022 + 64 - 1) / 64);
                // std::cout << "depth_overlay2022 " << depth_overlay2022 << std::endl;
                // std::cout << "sizeonecol_overlay2022 " << sizeonecol_overlay2022 << std::endl;

                // resize of the column vector
                data_col.resize(ncol);
                // std::cout << "num col: " << ncol << std::endl;
                for (size_t j = 0; j < ncol; j++) {
                    // do each column memory allocation 
                    ap_uint<512>* col_data = aligned_alloc<ap_uint<512> >(sizeonecol_overlay2022);
                    memset(col_data, -1, 64 * sizeonecol_overlay2022);
                    data_col[j] = col_data;
                    // std::cout << "start -->" << data_col[j] << std::endl;
                }
                // std::cout << std::endl;
            }            
        } else if (mode == 3) {
            data = aligned_alloc<ap_uint<512> >(size512.back());
        } else if (mode == 4) { // Partitioned Table Columns (BF+Part)
            size_t depth = (nrow + VEC_LEN);
            size_t sizeonecol = size_t((4 * depth + 64 - 1) / 64);
            for (size_t i = 0; i < ncol; i++) {
                size512.push_back(size512.back() + sizeonecol);
                iskdata.push_back(1);
            };
            data = aligned_alloc<ap_uint<512> >(size512.back());
            data[0] = get_table_header(size512[1], 0); // TO CHECK
            if (overlayVersion == 1) {
                // resize of the column vector
                data_col.resize(ncol);
                // std::cout << "num col: " << ncol << std::endl;
                for (size_t j = 0; j < ncol; j++) {
                    // do each column memory allocation 
                    ap_uint<512>* col_data = aligned_alloc<ap_uint<512> >(partition_num_rows);
                    memset(col_data, 2, 64 * partition_num_rows);
                    data_col[j] = col_data;
                }
            }     
        } 
        else {
            std::cout << "ERROR: Table mode not supported" << std::endl;
        }
    };

    void allocateHost(float f, int p_num) { // col added manually
        if ((f == 0) || (p_num == 0))
            std::cout << "ERROR: p_num (" << p_num << ")should be bigger than 1,"
                      << "f(" << f << ")should be bigger than 1" << std::endl;
        if (mode == 2) { // col added automatically
            npart = p_num;
            size_t depth = nrow + VEC_LEN;
            size_t sizeonecol = size_t((4 * depth + 64 - 1) / 64);
            size_t alignedSizeOneCol = (f * sizeonecol + p_num - 1) / p_num;
            for (int j = 0; j < p_num; j++) {
                for (size_t i = 0; i < ncol; i++) {
                    size512.push_back(size512.back() + alignedSizeOneCol);
                    iskdata.push_back(1);
                };
            }
            data = aligned_alloc<ap_uint<512> >(size512.back());
            data[0] = get_table_header(size512[ncol], alignedSizeOneCol, 0); // TO CHECK
            std::cout << nrow << " " 
                      << sizeonecol << " "
                      << alignedSizeOneCol << " "
                      << alignedSizeOneCol * 64 / 4 << " "
                      << data[0].range(31, 0) << std::endl;
            partition_num_rows = alignedSizeOneCol * 64 / 4;
            for (int j = 1; j < p_num; j++) data[size512[ncol] * j] = get_table_header(alignedSizeOneCol, 0);
        } else {
            std::cout << "ERROR: Table mode not supported" << std::endl;
        }
    };

    int getNumRow() {
        int nm = *(data);
        return nm;
    };

    void setNumRow(int n) {
        data[0].range(31, 0) = n; // TO CHECK
        nrow = n;
    };

    template <class T, int N>
    void setcharN(int r, int l, std::array<T, N> array_) {
        long offset = (long)r * N * sizeof(T);
        memcpy((char*)(data + size512[l] + 1) + offset, array_.data(), N * sizeof(T));
    }
    void setInt32(int r, int l, int d) { memcpy((char*)(data + size512[l] + 1) + r * sizeof(int), &d, sizeof(int)); };
    void setInt64(int r, int l, int64_t d) {
        long offset = (long)r * sizeof(int64_t);
        memcpy((char*)(data + size512[l] + 1) + offset, &d, sizeof(int64_t));
    };
    void setInt64_col(int row_index, int col_index, int64_t d) {
        long offset = row_index * sizeof(int64_t);
        // std::cout << data_col[col_index] << " -> +=" << offset << std::endl;
        ap_uint<512>* base_addr = data_col[col_index];
        memcpy((char*)(base_addr) + offset, &d, sizeof(int64_t));
    };

    void setInt64_l(int r, int l, int64_t d) { setInt32(r, l, d & 0x000000007fffffff); };
    void setInt64_h(int r, int l, int64_t d) { setInt32(r, l, (int32_t)(d >> 31)); };
    template <class T, int N>
    std::array<T, N> getcharN(int r, int l) {
        std::array<T, N> arr;
        long offset = (long)r * N * sizeof(T);
        memcpy(arr.data(), (char*)(data + size512[l] + 1) + offset, N * sizeof(T));
        return arr;
    };
    int getInt32K(int r, int l) {
        int d;
        memcpy(&d, (char*)(datak + size512[l] + 1) + r * sizeof(int), sizeof(int));
        return d;
    };
    int getInt32(int r, int l) {
        int d;
        memcpy(&d, (char*)(data + size512[l] + 1) + r * sizeof(int), sizeof(int));
        return d;
    };
    int64_t getInt64(int r, int l) {
        int64_t d;
        long offset = (long)r * sizeof(int64_t);
        memcpy(&d, (char*)(data + size512[l] + 1) + offset, sizeof(int64_t));
        return d;
    };
    int64_t getInt64_col(int row_index, int col_index) {
        //DONE: get Int64 data from Col 
        int64_t d;
        long offset = row_index * sizeof(int64_t);
        ap_uint<512>* base_addr = data_col[col_index];
        memcpy(&d, (char*)(base_addr) + offset, sizeof(int64_t));
        return d;
    };
    int64_t getInt64_part_col(int offset, int col_index) {
        //DONE: get Int64 data from Col 
        int64_t d;
        // long offset = row_index * sizeof(int64_t);
        ap_uint<512>* base_addr = data_col[col_index];
        memcpy(&d, (char*)(base_addr) + offset, sizeof(int64_t));
        return d;
    };
    uint32_t getUint32(int r, int l) {
        uint32_t d;
        memcpy(&d, (char*)(data + size512[l] + 1) + r * sizeof(uint32_t), sizeof(uint32_t));
        return d;
    };
    int64_t combineInt64(int r, int l0, int l1) {
        ap_uint<32> h; // h
        ap_uint<32> l; // l
        int64_t d;

        memcpy(&h, (char*)(data + size512[l0] + 1) + r * sizeof(uint32_t), sizeof(uint32_t));
        memcpy(&l, (char*)(data + size512[l1] + 1) + r * sizeof(uint32_t), sizeof(uint32_t));
        d = (h, l);

        return d;
    };

    int64_t mergeInt64(int32_t l, int32_t h) {
        int64_t h_ = (int64_t)h;
        int64_t h_l = (h_ << 31) + l;
        return h_l;
    }

    bool getKdata() {
        bool datacp = 0;
        int cpNum = 0;
        for (size_t i = 0; i < ncol; i++) {
            if (iskdata[i] == 0) {
                datacp = 1;
            } else {
                cpNum++;
            }
        }
        if (datacp) {
            size_t depth = nrow + VEC_LEN * 2 - 1;
            size_t sizeonecol = size_t((4 * depth + 64 - 1) / 64);
            datak = aligned_alloc<ap_uint<512> >(sizeonecol * cpNum);
            int ki = 0;
            for (size_t i = 0; i < ncol; i++) {
                if (iskdata[i] == 1) {
                    memcpy(datak + ki, data + size512[i], sizeonecol * 64);
                    ki += sizeonecol;
                }
            }
            datak[0] = get_table_header(sizeonecol, nrow); // TO CHECK
        }

        return datacp;
    }

    void tableToCol() {
        // if (overlayVersion == 1) {
            for (int i = 0; i < ncol; i++) {
                for (int j = 0; j < nrow; j++) {
                    int d = getInt32(j,i);
                    if (j < 10) std::cout << d << " ";
                    int64_t d_64 = d;
                    setInt64_col(j, i, d_64);
                }
            }
        // }
    };

    void colToTable() {
        // if (overlayVersion == 1) {
            for (int i = 0; i < ncol; i++) {
                for (int j = 0; j < nrow; j++) {
                    int64_t d = getInt64_col(j,i);
                    if (j < 10) std::cout << d << " ";
                    int32_t d_32 = static_cast<int32_t>(d);
                    setInt32(j, i, d_32);
                }
            }
        // }
    };

    void allocateDevBuffer_col(cl::Context& context, int bank, int index){

        if (mode != 4) {
            size_t depth_overlay2022 = nrow; // getNumRow();
            if (depth_overlay2022 == 0 && index < ncol && index != -1) {
                depth_overlay2022 = partition_num_rows;
            }
            size_t sizeonecol_overlay2022 = size_t((8 * depth_overlay2022 + 64 - 1) / 64);
            // std::cout << "hel1\n";
            // std::cout << depth_overlay2022 << "\n";
            // std::cout << sizeonecol_overlay2022 << "\n\n";

            if (index == -1) {
                ap_uint<512>* extra_col = aligned_alloc<ap_uint<512> >(1); // default to 1
                memset(extra_col, 1, 64 * 1);
                cl_mem_ext_ptr_t temp_cl_mem_col = {XCL_MEM_TOPOLOGY | (unsigned int)(bank), extra_col, 0};
                mext_col.push_back(temp_cl_mem_col);
                cl::Buffer temp_cl_buff_col = cl::Buffer(context, CL_MEM_EXT_PTR_XILINX | CL_MEM_USE_HOST_PTR | CL_MEM_READ_WRITE, (size_t)(64 * 1), &(temp_cl_mem_col));
                buffer_col.push_back(temp_cl_buff_col);
                // std::cout << "-> " << extra_col << std::endl;
            } else {
                if (index < ncol) {
                    cl_mem_ext_ptr_t temp_cl_mem_col = {XCL_MEM_TOPOLOGY | (unsigned int)(bank), data_col[index], 0};
                    // std::cout << "hel2\n";
                    // std::cout << data_col.size() << "\n";
                    // std::cout << mext_col.size() << "\n\n";
                    mext_col.push_back(temp_cl_mem_col);
                    cl::Buffer temp_cl_buff_col = cl::Buffer(context, CL_MEM_EXT_PTR_XILINX | CL_MEM_USE_HOST_PTR | CL_MEM_READ_WRITE, (size_t)(64 * sizeonecol_overlay2022), &(temp_cl_mem_col));
                    buffer_col.push_back(temp_cl_buff_col);
                    // std::cout << "-> " << data_col[index] << std::endl;
                } else {
                    ap_uint<512>* extra_col = aligned_alloc<ap_uint<512> >(1); // default to 1
                    memset(extra_col, 1, 64 * 1);
                    cl_mem_ext_ptr_t temp_cl_mem_col = {XCL_MEM_TOPOLOGY | (unsigned int)(bank), extra_col, 0};
                    mext_col.push_back(temp_cl_mem_col);
                    cl::Buffer temp_cl_buff_col = cl::Buffer(context, CL_MEM_EXT_PTR_XILINX | CL_MEM_USE_HOST_PTR | CL_MEM_READ_WRITE, (size_t)(64 * 1), &(temp_cl_mem_col));
                    buffer_col.push_back(temp_cl_buff_col);
                }
            }
        }
        else if(mode == 4 && partition_col_size != 0) { // For BF+Part
            // std::cout << index << std::endl;
            if (index == -1) {
                ap_uint<512>* extra_col = aligned_alloc<ap_uint<512> >(1); // default to 1
                memset(extra_col, 1, 64 * 1);
                cl_mem_ext_ptr_t temp_cl_mem_col = {XCL_MEM_TOPOLOGY | (unsigned int)(bank), extra_col, 0};
                mext_col.push_back(temp_cl_mem_col);
                cl::Buffer temp_cl_buff_col = cl::Buffer(context, CL_MEM_EXT_PTR_XILINX | CL_MEM_USE_HOST_PTR | CL_MEM_READ_WRITE, (size_t)(64 * 1), &(temp_cl_mem_col));
                buffer_col.push_back(temp_cl_buff_col);
                std::cout << "-> " << extra_col << std::endl;
            }
            else{
                if(index < ncol){
                    cl_mem_ext_ptr_t temp_cl_mem_col = {XCL_MEM_TOPOLOGY | (unsigned int)(bank), data_col[index], 0};
                    mext_col.push_back(temp_cl_mem_col);
                    cl::Buffer temp_cl_buff_col = cl::Buffer(context, CL_MEM_EXT_PTR_XILINX | CL_MEM_USE_HOST_PTR | CL_MEM_READ_WRITE, partition_col_size, &(temp_cl_mem_col));
                    buffer_col.push_back(temp_cl_buff_col);
                    std::cout << "-> " << data_col[index] << std::endl;
                }
                else
                {
                    ap_uint<512>* extra_col = aligned_alloc<ap_uint<512> >(1); // default to 1
                    memset(extra_col, 1, 64 * 1);
                    cl_mem_ext_ptr_t temp_cl_mem_col = {XCL_MEM_TOPOLOGY | (unsigned int)(bank), extra_col, 0};
                    mext_col.push_back(temp_cl_mem_col);
                    cl::Buffer temp_cl_buff_col = cl::Buffer(context, CL_MEM_EXT_PTR_XILINX | CL_MEM_USE_HOST_PTR | CL_MEM_READ_WRITE, (size_t)(64 * 1), &(temp_cl_mem_col));
                    buffer_col.push_back(temp_cl_buff_col);
                }
            }   
        }
        
    }

    //! Buffer allocation
    void allocateDevBuffer(cl::Context& context, int bank, int num_cols = 0) {
        // getKdata();
        int cpNum = 0;
        for (size_t i = 0; i < ncol; i++) {
            if (iskdata[i] == 1) {
                cpNum++;
            }
        }
        size_t depth = nrow + VEC_LEN * 2 - 1;
        size_t sizeonecol = size_t((4 * depth + 64 - 1) / 64);

        if (getKdata()) {
            mext = {XCL_MEM_TOPOLOGY | (unsigned int)(bank), datak, 0};
            buffer = cl::Buffer(context, CL_MEM_EXT_PTR_XILINX | CL_MEM_USE_HOST_PTR | CL_MEM_READ_WRITE,
                                (size_t)(64 * sizeonecol * cpNum), &mext);
            std::cout << name << " Buffer size: " << (64 * sizeonecol * cpNum) / (1024 * 1024) << " MByte " << std::endl;
        } else {
            if (overlayVersion == 0) {
                mext = {XCL_MEM_TOPOLOGY | (unsigned int)(bank), data, 0};
                buffer = cl::Buffer(context, CL_MEM_EXT_PTR_XILINX | CL_MEM_USE_HOST_PTR | CL_MEM_READ_WRITE,
                                    (size_t)(64 * size512.back()), &mext);
                std::cout << name << " XBuffer size: " << (64 * size512.back() / (1024 * 1024)) << " MByte " << std::endl;
            } else if (overlayVersion == 1) {
                mext = {XCL_MEM_TOPOLOGY | (unsigned int)(bank), data, 0};
                buffer = cl::Buffer(context, CL_MEM_EXT_PTR_XILINX | CL_MEM_USE_HOST_PTR | CL_MEM_READ_WRITE,
                                    (size_t)(64 * size512.back()), &mext);
                std::cout << name << " XBuffer size: " << (64 * size512.back() / (1024 * 1024)) << " MByte " << std::endl;                
                // // DONE: allocate device buffers for each column, using the bank from 2022 U50.ini
                // data_col.resize(num_cols);
                // // size_t col_depth = (nrow + VEC_LEN - 1)/VEC_LEN;
                // size_t depth_overlay2022 = (nrow);
                // size_t sizeonecol_overlay2022 = size_t((8 * depth_overlay2022 + 64 - 1) / 64);
                // // set non-valid columns in CPU
                // for(int i=0; i < (num_cols - ncol); i++){
                //     ap_uint<512>* extra_col = aligned_alloc<ap_uint<512> >(1); // default to 1
                //     memset(extra_col, 0, 64 * 1);
                //     data_col[ncol + i] = extra_col;
                // }
                // mext_col.resize(num_cols);
                // buffer_col.resize(num_cols);
                // // set valid columns
                // for(int i = 0; i < ncol; i++){
                //     cl_mem_ext_ptr_t temp_cl_mem_col = {XCL_MEM_TOPOLOGY | (unsigned int)(bank), data_col[i], 0};
                //     mext_col[i] = temp_cl_mem_col;
                //     cl::Buffer temp_cl_buff_col = cl::Buffer(context, CL_MEM_EXT_PTR_XILINX | CL_MEM_USE_HOST_PTR | CL_MEM_READ_WRITE, 
                //                                             (size_t)(64 * sizeonecol_overlay2022), &(mext_col[i]));
                //     buffer_col[i] = temp_cl_buff_col;
                //     // std::cout << "---Valid Columns----\n";
                //     std::cout << "NumRows: " << depth_overlay2022 << std::endl;
                //     std::cout << name << " XBuffer size: " << (64 * sizeonecol_overlay2022) << " Bytes " << std::endl;

                // }
                // // set non-valid columns
                // for(int i = 0; i < (num_cols - ncol); i++){
                //     // std::cout << "fin --> " << data_col[i] << std::endl;
                //     cl_mem_ext_ptr_t temp_cl_mem_col = {XCL_MEM_TOPOLOGY | (unsigned int)(bank), data_col[ncol + i], 0};
                //     mext_col[ncol + i] = temp_cl_mem_col;
                //     cl::Buffer temp_cl_buff_col = cl::Buffer(context, CL_MEM_EXT_PTR_XILINX | CL_MEM_USE_HOST_PTR | CL_MEM_READ_WRITE, 
                //                                             (size_t)(64 * 1), &(mext_col[ncol + i]));
                //     buffer_col[ncol + i] = temp_cl_buff_col;
                //     // std::cout << name << " XBuffer size: " << (64 * size512.back() / (1024 * 1024)) << " MByte " << std::endl;
                //     std::cout << name << " XBuffer size: " << (64 * sizeonecol / (1024 * 1024)) << " MByte " << std::endl;
                //     // std::cout << "---Non Valid Columns----\n";
                //     // std::cout << "NumRows: " << depth_overlay2022 << std::endl;
                //     // std::cout << name << " XBuffer size: " << (64 * 1) << " Bytes " << std::endl;
                // }   
            }
        }
    };


    void copyTableData(Table* dest_table) {
        int cpNum = 0;
        for (size_t i = 0; i < ncol; i++) {
            if (iskdata[i] == 1) {
                cpNum++;
            }
        }
        size_t depth = nrow + VEC_LEN * 2 - 1;
        size_t sizeonecol = size_t((4 * depth + 64 - 1) / 64);

        size_t numBytes = 0;
        if (getKdata()) {
            numBytes = 64 * sizeonecol * cpNum;
            memcpy((char*)dest_table->datak, (char*)datak, numBytes);
        } else {
            numBytes = 64 * size512.back();
            memcpy((char*)dest_table->data, (char*)data, numBytes);
        }
    };

    void initBuffer(cl::CommandQueue clq) {
        std::vector<cl::Memory> tb;
        tb.push_back(buffer);
        clq.enqueueMigrateMemObjects(tb, CL_MIGRATE_MEM_OBJECT_CONTENT_UNDEFINED, nullptr, nullptr);
        clq.enqueueWriteBuffer(buffer, CL_TRUE, 0, 64, data, nullptr, nullptr);
    }

    void getPartDevBuffer(cl::Buffer* subBuf, int p_num, size_t size) {
        cl_buffer_region sub_region[2];
        for (int i = 0; i < p_num; i++) {
            sub_region[i] = {i * size, size};
            subBuf[i] = buffer.createSubBuffer(CL_MEM_READ_WRITE | CL_MEM_EXT_PTR_XILINX, CL_BUFFER_CREATE_TYPE_REGION,
                                               &sub_region[i]);
        }
    };

    Table createSubTable(int index) {
        Table tout;
        if (index < npart) {
            cl_buffer_region region{64 * index * size512[ncol], 64 * size512[ncol]};
            tout.buffer = buffer.createSubBuffer(CL_MEM_READ_WRITE | CL_MEM_EXT_PTR_XILINX,
                                                 CL_BUFFER_CREATE_TYPE_REGION, &region);
            tout.data = data + index * size512[ncol];
            tout.nrow = tout.data[0].range(31, 0);
            tout.ncol = ncol;
            tout.data_col.resize(ncol);
            tout.partition_num_rows = partition_num_rows;
            for (size_t j = 0; j < ncol; j++) {
                // do each column memory allocation 
                ap_uint<512>* col_data = aligned_alloc<ap_uint<512> >(partition_num_rows);
                memset(col_data, 2, 64 * partition_num_rows);
                tout.data_col[j] = col_data;
            }
            std::cout << tout.partition_num_rows << std::endl;
            int size512Base = size512[index * ncol];
            for (size_t i = 0; i < ncol; i++) {
                tout.size512.push_back(size512[index * ncol + i] - size512Base);
            }
            // return tout;
        } else {
            std::cout << "index (" << index << ") out of part range (" << npart << ")." << std::endl;
            // return NULL;
        }
        return tout;
    }
    void mergeSubTable(Table* tables, int num) {
        ap_uint<512>* datam = aligned_alloc<ap_uint<512> >(size512.back());
        int rownum = 0;
        for (int i = 0; i < num; i++) {
            rownum += tables[i].data[0].range(31, 0).to_int();
        }
        size_t depth = rownum + VEC_LEN;
        size_t sizeonecol = size_t((4 * depth + 64 - 1) / 64);
        std::cout << "rownum in data" << rownum << std::endl;
        std::cout << "sizeonecol in data" << sizeonecol << std::endl;
        int offsets0 = 0;

        for (size_t k = 0; k < ncol; k++) {
            int offsets1 = 0;
            size512[k + 1] = (size512[k] + sizeonecol);
            for (int i = 0; i < num; i++) {
                ap_uint<512>* subdata = tables[i].data;
                int nnrow = subdata[0].range(31, 0).to_int();
                int size512_one_col = subdata[0].range(63, 32).to_int();
                // int size512_one_tb = subdata[0].range(95, 64).to_int();
                if (nnrow > 0) {
                    // if(size512_one_col!=24) std::cout<<i<<" : "<<size512_one_col<<std::endl<<std::endl;
                    ap_uint<512>* subdata_k = subdata + size512_one_col * k + 1;
                    memcpy(((char*)(datam + offsets0 + 1)) + offsets1, (char*)subdata_k, 4 * nnrow);
                    offsets1 += nnrow * 4;
                }
            }
            offsets0 += sizeonecol;
        }
        // data = datam;
        int64_t sizeofbytes = 64 * size512.back();
        memcpy(data, datam, sizeofbytes);
        data[0] = get_table_header(sizeonecol, rownum); // TO CHECK
        std::cout << "The check" << data[0].range(63, 32).to_int() << " " << data[0].range(31, 0).to_int() << std::endl;
        std::cout << "The check" << data[1].range(95, 64).to_int() << " " << data[1].range(63, 32).to_int() << " "
                  << data[1].range(31, 0).to_int() << std::endl;
        std::cout << "The check" << data[1 + sizeonecol * 7].range(95, 64).to_int() << " "
                  << data[1 + sizeonecol * 7].range(63, 32).to_int() << " "
                  << data[1 + sizeonecol * 7].range(31, 0).to_int() << std::endl;
        std::cout << "The check" << datam[1].range(95, 64).to_int() << " " << datam[1].range(63, 32).to_int() << " "
                  << datam[1].range(31, 0).to_int() << std::endl;
        std::cout << "The check" << datam[1 + sizeonecol * 7].range(95, 64).to_int() << " "
                  << datam[1 + sizeonecol * 7].range(63, 32).to_int() << " "
                  << datam[1 + sizeonecol * 7].range(31, 0).to_int() << std::endl;
    }

   private:
    ap_uint<512> get_table_header(int n512b, int nrow) {
        ap_uint<512> th = 0;
        th.range(31, 0) = nrow;
        th.range(63, 32) = n512b;
        return th;
    };

    ap_uint<512> get_table_header(int hp_size, int blk_size, int nrow) {
        ap_uint<512> th = 0;
        th.range(31, 0) = nrow;
        th.range(63, 32) = blk_size;
        th.range(95, 64) = hp_size;
        return th;
    }

    int mode;
};

void gatherTable_col(Table& tin1, Table& tin2, Table& tout) {
    int nrow = tin1.getNumRow();
    if (nrow != tin2.getNumRow()) {
        std::cout << "colums gathering must tin1.row == tin2.row" << std::endl;
        return;
    }
    for (int r = 0; r < nrow; r++) {
        int tin1_0 = tin1.getInt32(r, 0);
        int tin1_1 = tin1.getInt32(r, 1);
        int tin2_0 = tin2.getInt32(r, 2);
        int tin2_1 = tin2.getInt32(r, 3);
        tout.setInt32(r, 0, tin1_0);
        tout.setInt32(r, 1, tin1_1);
        tout.setInt32(r, 2, tin2_0);
        tout.setInt32(r, 3, tin2_1);
    }
    tout.setNumRow(nrow);
}
void gatherTable_row(Table& tin1, Table& tin2, Table& tout) {
    int nrow1 = tin1.getNumRow();
    int nrow2 = tin2.getNumRow();
    /*for (int r = 0; r < nrow1; r++) {
        int tin1_0 = tin1.getInt32(r, 0);
        int tin1_1 = tin1.getInt32(r, 1);
    }
    for (int r = nrow1; r < nrow1 + nrow2; r++) {
        int tin2_0 = tin2.getInt32(r, 0);
        int tin2_1 = tin2.getInt32(r, 1);
    }*/
    tout.setNumRow(nrow1 + nrow2);
}

class cfgCmd {
   public:
    ap_uint<512>* cmd;
    cl::Buffer buffer;

    MetaTable* meta_in;
    MetaTable* meta_out;
    cl::Buffer buffer_meta_in;
    cl::Buffer buffer_meta_out;
    cl::Buffer buffer_valid_in_col;

    int overlayVersion = 0; // 0 - 2020; 1 - 2022

    std::vector<int> input_key_columns;
    std::vector<int> output_key_columns; 
    ap_uint<64>* din_val;

    cfgCmd(){};

    void allocateHost() {

        if(overlayVersion == 0){
            cmd = aligned_alloc<ap_uint<512> >(9);
            memset(cmd, 0, 64 * 9);
        }
        else if(overlayVersion == 1){
            cmd = aligned_alloc<ap_uint<512> >(14);
            memset(cmd, 0, 64 * 14);
        }
        
        meta_in = new MetaTable();
        meta_out = new MetaTable();
    };

    void allocateDevBuffer(cl::Context& context, int bank) {

        //Config-Data according to overlayVersion
        if (overlayVersion == 0) {
            cl_mem_ext_ptr_t mext = {XCL_MEM_TOPOLOGY | (unsigned int)(bank), cmd, 0};
            buffer = cl::Buffer(context, CL_MEM_EXT_PTR_XILINX | CL_MEM_USE_HOST_PTR | CL_MEM_READ_ONLY, (size_t)(64 * 9),
                            &mext);
        }
        else if (overlayVersion == 1) {
#ifdef USE_U50            
            cl_mem_ext_ptr_t mext = {XCL_MEM_TOPOLOGY | (unsigned int)(3), cmd, 0};
            buffer = cl::Buffer(context, CL_MEM_EXT_PTR_XILINX | CL_MEM_USE_HOST_PTR | CL_MEM_READ_ONLY, (size_t)(64 * 14),
                            &mext);

            // std::cout << "Config: Device Buffers set" << std::endl;
            //Meta-Data (input and output)
            cl_mem_ext_ptr_t mext_meta_in = {XCL_MEM_TOPOLOGY | (unsigned int)(3), meta_in->meta(), 0};
                buffer_meta_in = cl::Buffer(context, CL_MEM_EXT_PTR_XILINX | CL_MEM_USE_HOST_PTR | CL_MEM_READ_ONLY, (size_t)(sizeof(ap_uint<512>) * meta_out->_metaDepth),
                                &mext_meta_in);

            cl_mem_ext_ptr_t mext_meta_out = {XCL_MEM_TOPOLOGY | (unsigned int)(17), meta_out->meta(), 0};
                buffer_meta_out = cl::Buffer(context, CL_MEM_EXT_PTR_XILINX | CL_MEM_USE_HOST_PTR | CL_MEM_READ_WRITE, (size_t)(sizeof(ap_uint<512>) * meta_out->_metaDepth),
                                &mext_meta_out);
            // std::cout << "Meta: Device Buffers set" << std::endl;

            din_val = aligned_alloc<ap_uint<64> >((meta_in->getColLen() + 63) / 64);
            // for (int i = 0; i < (meta_in->getColLen() + 63) / 64; i++) {
            //     din_val[i] = 0xffffffffffffffff;
            // }
            // memset(din_val, 0, 8 * (meta_in->getColLen() + 63) / 64);
            cl_mem_ext_ptr_t mext_valid_in_col = {XCL_MEM_TOPOLOGY | (unsigned int)(3), din_val, 0};
            buffer_valid_in_col = cl::Buffer(context, CL_MEM_EXT_PTR_XILINX | CL_MEM_USE_HOST_PTR | CL_MEM_READ_ONLY, ((meta_in->getColLen() + 63) / 64) * sizeof(ap_uint<64>),
                                &mext_valid_in_col);
            // std::cout << "Validation Buffer set" << std::endl << std::endl;
#else 
            cl_mem_ext_ptr_t mext = {XCL_MEM_TOPOLOGY | (unsigned int)(0), cmd, 0};
            buffer = cl::Buffer(context, CL_MEM_EXT_PTR_XILINX | CL_MEM_USE_HOST_PTR | CL_MEM_READ_ONLY, (size_t)(64 * 14),
                            &mext);

            // std::cout << "Config: Device Buffers set" << std::endl;
            //Meta-Data (input and output)
            cl_mem_ext_ptr_t mext_meta_in = {XCL_MEM_TOPOLOGY | (unsigned int)(12), meta_in->meta(), 0};
                buffer_meta_in = cl::Buffer(context, CL_MEM_EXT_PTR_XILINX | CL_MEM_USE_HOST_PTR | CL_MEM_READ_ONLY, (size_t)(sizeof(ap_uint<512>) * meta_out->_metaDepth),
                                &mext_meta_in);

            cl_mem_ext_ptr_t mext_meta_out = {XCL_MEM_TOPOLOGY | (unsigned int)(13), meta_out->meta(), 0};
                buffer_meta_out = cl::Buffer(context, CL_MEM_EXT_PTR_XILINX | CL_MEM_USE_HOST_PTR | CL_MEM_READ_WRITE, (size_t)(sizeof(ap_uint<512>) * meta_out->_metaDepth),
                                &mext_meta_out);
            // std::cout << "Meta: Device Buffers set" << std::endl;

            din_val = aligned_alloc<ap_uint<64> >((meta_in->getColLen() + 63) / 64);
            // for (int i = 0; i < (meta_in->getColLen() + 63) / 64; i++) {
            //     din_val[i] = 0xffffffffffffffff;
            // }
            // memset(din_val, 0, 8 * (meta_in->getColLen() + 63) / 64);
            cl_mem_ext_ptr_t mext_valid_in_col = {XCL_MEM_TOPOLOGY | (unsigned int)(32), din_val, 0};
            buffer_valid_in_col = cl::Buffer(context, CL_MEM_EXT_PTR_XILINX | CL_MEM_USE_HOST_PTR | CL_MEM_READ_ONLY, ((meta_in->getColLen() + 63) / 64) * sizeof(ap_uint<64>),
                                &mext_valid_in_col);
            // std::cout << "Validation Buffer set" << std::endl << std::endl;
#endif            
        }
    };

    void setup(int overlayVersion_) {
        overlayVersion = overlayVersion_;
    }; 
};

class AggrCfgCmd {
   public:
    ap_uint<32>* cmd;
    cl::Buffer buffer;

    //DONE: add function to configure the meta_aggr table and AggrMeta info class (input and output);
    // for example -> xf::database::gqe::MetaTable meta_aggr
    //DONE: add allocateHost() and allocateDeviceBuffers() for meta table

    //ap_uint<512>* meta_aggr;
    MetaTable* meta_aggr;
    cl::Buffer buffer_meta;
    
    int overlayVersion = 0; // 0 - 2020; 1 - 2022
    int inOut = 0;          // 0 - input; 1 - output;

    AggrCfgCmd(){};

    void allocateHost() {
        cmd = aligned_alloc<ap_uint<32> >(128);
        memset(cmd, 0, 4 * 128);

        // meta_aggr = aligned_alloc<ap_uint<512>>(24);
        // memset(meta_aggr, 0, 64 * 24);
        meta_aggr = new MetaTable();
    };

    void allocateDevBuffer(cl::Context& context, int bank) {

        if (inOut == 0) {
            cl_mem_ext_ptr_t mext = {XCL_MEM_TOPOLOGY | (unsigned int)(bank), cmd, 0};
            buffer = cl::Buffer(context, CL_MEM_EXT_PTR_XILINX | CL_MEM_USE_HOST_PTR | CL_MEM_READ_ONLY, (size_t)(4 * 128),
                            &mext);
        } else {
            cl_mem_ext_ptr_t mext = {XCL_MEM_TOPOLOGY | (unsigned int)(bank), cmd, 0};
            buffer = cl::Buffer(context, CL_MEM_EXT_PTR_XILINX | CL_MEM_USE_HOST_PTR | CL_MEM_READ_WRITE, (size_t)(4 * 128),
                            &mext);
        }
        // meta data
        if (overlayVersion == 1) {
            // DONE: if used for input data - use read_only
            if (inOut == 0) {
                cl_mem_ext_ptr_t mext_meta = {XCL_MEM_TOPOLOGY | (unsigned int)(bank), meta_aggr->meta(), 0};
                buffer_meta = cl::Buffer(context, CL_MEM_EXT_PTR_XILINX | CL_MEM_USE_HOST_PTR | CL_MEM_READ_ONLY, (size_t)(sizeof(ap_uint<512>) * meta_aggr->_metaDepth),
                                &mext_meta);
            } else {
            // DONE: if used for output data - use read_write
                cl_mem_ext_ptr_t mext_meta = {XCL_MEM_TOPOLOGY | (unsigned int)(bank), meta_aggr->meta(), 0};
                buffer_meta = cl::Buffer(context, CL_MEM_EXT_PTR_XILINX | CL_MEM_USE_HOST_PTR | CL_MEM_READ_WRITE, (size_t)(sizeof(ap_uint<512>) * meta_aggr->_metaDepth),
                                &mext_meta);
            }
        }
    };

    void setup(int overlayVersion_, int inOut_) {
        overlayVersion = overlayVersion_;
        inOut = inOut_;
    }; 
};


class bfbufferTmp {
   public:
    cl_mem_ext_ptr_t memExt[16];
    ap_uint<64>* htb_buf[8];
    ap_uint<64>* stb_buf[8];
    cl::Buffer buffer[16];
    int overlayVersion = 0;
    int htb_buf_depth;
    int stb_buf_depth;
    int htb_buf_size;
    int stb_buf_size;


    bfbufferTmp(cl::Context& context, int overlayVersion_, int htb, int stb) {
        overlayVersion = overlayVersion_;
        htb_buf_depth = htb;
        stb_buf_depth = stb;
        htb_buf_size = sizeof(ap_uint<256>) * htb_buf_depth / 2;
        stb_buf_size = sizeof(ap_uint<256>) * stb_buf_depth / 2;

        if(overlayVersion == 1){
            for (int i = 0; i < 8; i++) {
                htb_buf[i] = aligned_alloc<ap_uint<64> >(htb_buf_depth / (sizeof(ap_uint<64>) * 8) * 256);
                stb_buf[i] = aligned_alloc<ap_uint<64> >(stb_buf_depth / (sizeof(ap_uint<64>) * 8) * 256);
                // memset(tb_buf[i], 0, HT_BUFF_DEPTH);
            };
        }

#ifdef USE_DDR
        memExt[0].flags = XCL_MEM_DDR_BANK1;
        memExt[1].flags = XCL_MEM_DDR_BANK1;
        memExt[2].flags = XCL_MEM_DDR_BANK1;
        memExt[3].flags = XCL_MEM_DDR_BANK2;
        memExt[4].flags = XCL_MEM_DDR_BANK2;
        memExt[5].flags = XCL_MEM_DDR_BANK2;
        memExt[6].flags = XCL_MEM_DDR_BANK3;
        memExt[7].flags = XCL_MEM_DDR_BANK3;
        memExt[8].flags = XCL_MEM_DDR_BANK1;
        memExt[9].flags = XCL_MEM_DDR_BANK1;
        memExt[10].flags = XCL_MEM_DDR_BANK1;
        memExt[11].flags = XCL_MEM_DDR_BANK2;
        memExt[12].flags = XCL_MEM_DDR_BANK2;
        memExt[13].flags = XCL_MEM_DDR_BANK2;
        memExt[14].flags = XCL_MEM_DDR_BANK3;
        memExt[15].flags = XCL_MEM_DDR_BANK3;
#else

    if (overlayVersion == 1) { // Overlay V2022
        // DONE: fill this later
        memExt[0].flags = (unsigned int)(4) | XCL_MEM_TOPOLOGY;
        memExt[1].flags = (unsigned int)(6) | XCL_MEM_TOPOLOGY;
        memExt[2].flags = (unsigned int)(8) | XCL_MEM_TOPOLOGY;
        memExt[3].flags = (unsigned int)(10) | XCL_MEM_TOPOLOGY;
        memExt[4].flags = (unsigned int)(20) | XCL_MEM_TOPOLOGY;
        memExt[5].flags = (unsigned int)(22) | XCL_MEM_TOPOLOGY;
        memExt[6].flags = (unsigned int)(24) | XCL_MEM_TOPOLOGY;
        memExt[7].flags = (unsigned int)(26) | XCL_MEM_TOPOLOGY;
        memExt[8].flags = (unsigned int)(5) | XCL_MEM_TOPOLOGY;
        memExt[9].flags = (unsigned int)(7) | XCL_MEM_TOPOLOGY;
        memExt[10].flags = (unsigned int)(9) | XCL_MEM_TOPOLOGY;
        memExt[11].flags = (unsigned int)(11) | XCL_MEM_TOPOLOGY;
        memExt[12].flags = (unsigned int)(21) | XCL_MEM_TOPOLOGY;
        memExt[13].flags = (unsigned int)(23) | XCL_MEM_TOPOLOGY;
        memExt[14].flags = (unsigned int)(25) | XCL_MEM_TOPOLOGY;
        memExt[15].flags = (unsigned int)(27) | XCL_MEM_TOPOLOGY;
    }
#endif

        // for (int i = 0; i < 16; i++) {
        //     memExt[i].param = 0;
        //     memExt[i].obj = tb_buf[i];
        // };

        // Map buffers
        if(overlayVersion == 1){

            for (int i = 0; i < 8; i++) {
                memExt[i].param = 0;
                memExt[i].obj = htb_buf[i];
            }; 

            for (int i = 8; i < 16; i++) {
                memExt[i].param = 0;
                memExt[i].obj = stb_buf[i-8];
            }; 

            for (int i = 0; i < 8; i++) {
                buffer[i] = cl::Buffer(context, CL_MEM_READ_WRITE | CL_MEM_EXT_PTR_XILINX | CL_MEM_USE_HOST_PTR,
                                   (size_t)(htb_buf_size), &memExt[i]);
            };
            for (int i = 8; i < 16; i++) {
                buffer[i] = cl::Buffer(context, CL_MEM_READ_WRITE | CL_MEM_EXT_PTR_XILINX | CL_MEM_USE_HOST_PTR,
                                    (size_t)(stb_buf_size), &memExt[i]);
            };
        }
        
    };

    void initBuffer(cl::CommandQueue clq) {
        std::vector<cl::Memory> tb;
        for (int i = 0; i < 16; i++) {
            tb.push_back(buffer[i]);
        }
        clq.enqueueMigrateMemObjects(tb, CL_MIGRATE_MEM_OBJECT_CONTENT_UNDEFINED, nullptr, nullptr);
    }

}; // end of class

class bufferTmp {
   public:
    cl_mem_ext_ptr_t memExt[16];
    ap_uint<64>* tb_buf[16];
    cl::Buffer buffer[16];
    int overlayVersion = 0;

    ap_uint<256>* tb_buf_overlay2022[16];

    bufferTmp(cl::Context& context, int overlayVersion_) {
        overlayVersion = overlayVersion_;

        if(overlayVersion == 0){
            for (int i = 0; i < 8; i++) {
                tb_buf[i] = aligned_alloc<ap_uint<64> >(HT_BUFF_DEPTH);
            };
            for (int i = 8; i < 16; i++) {
                tb_buf[i] = aligned_alloc<ap_uint<64> >(S_BUFF_DEPTH);
            };
        }
        else if(overlayVersion == 1){
            for (int i = 0; i < 8; i++) {
                tb_buf_overlay2022[i] = aligned_alloc<ap_uint<256> >(HT_BUFF_DEPTH);
                // memset(tb_buf_overlay2022[i], 0, HT_BUFF_DEPTH);
            };
            for (int i = 8; i < 16; i++) {
                tb_buf_overlay2022[i] = aligned_alloc<ap_uint<256> >(S_BUFF_DEPTH);
                // memset(tb_buf_overlay2022[i], 0, S_BUFF_DEPTH);
            };
        }

#ifdef USE_DDR
        memExt[0].flags = XCL_MEM_DDR_BANK1;
        memExt[1].flags = XCL_MEM_DDR_BANK1;
        memExt[2].flags = XCL_MEM_DDR_BANK1;
        memExt[3].flags = XCL_MEM_DDR_BANK2;
        memExt[4].flags = XCL_MEM_DDR_BANK2;
        memExt[5].flags = XCL_MEM_DDR_BANK2;
        memExt[6].flags = XCL_MEM_DDR_BANK3;
        memExt[7].flags = XCL_MEM_DDR_BANK3;
        memExt[8].flags = XCL_MEM_DDR_BANK1;
        memExt[9].flags = XCL_MEM_DDR_BANK1;
        memExt[10].flags = XCL_MEM_DDR_BANK1;
        memExt[11].flags = XCL_MEM_DDR_BANK2;
        memExt[12].flags = XCL_MEM_DDR_BANK2;
        memExt[13].flags = XCL_MEM_DDR_BANK2;
        memExt[14].flags = XCL_MEM_DDR_BANK3;
        memExt[15].flags = XCL_MEM_DDR_BANK3;
#else
    if (overlayVersion == 0) {
        /*
       	memExt[0].flags = (unsigned int)(2) | XCL_MEM_TOPOLOGY;
        memExt[1].flags = (unsigned int)(3) | XCL_MEM_TOPOLOGY;
        memExt[2].flags = (unsigned int)(10) | XCL_MEM_TOPOLOGY;
        memExt[3].flags = (unsigned int)(11) | XCL_MEM_TOPOLOGY;
        memExt[4].flags = (unsigned int)(18) | XCL_MEM_TOPOLOGY;
        memExt[5].flags = (unsigned int)(19) | XCL_MEM_TOPOLOGY;
        memExt[6].flags = (unsigned int)(26) | XCL_MEM_TOPOLOGY;
        memExt[7].flags = (unsigned int)(27) | XCL_MEM_TOPOLOGY;
        memExt[8].flags = (unsigned int)(6) | XCL_MEM_TOPOLOGY;
        memExt[9].flags = (unsigned int)(7) | XCL_MEM_TOPOLOGY;
        memExt[10].flags = (unsigned int)(14) | XCL_MEM_TOPOLOGY;
        memExt[11].flags = (unsigned int)(15) | XCL_MEM_TOPOLOGY;
        memExt[12].flags = (unsigned int)(22) | XCL_MEM_TOPOLOGY;
        memExt[13].flags = (unsigned int)(23) | XCL_MEM_TOPOLOGY;
        memExt[14].flags = (unsigned int)(30) | XCL_MEM_TOPOLOGY;
        memExt[15].flags = (unsigned int)(31) | XCL_MEM_TOPOLOGY;
        */
	    memExt[0].flags = (unsigned int)(0) | XCL_MEM_TOPOLOGY;
        memExt[1].flags = (unsigned int)(2) | XCL_MEM_TOPOLOGY;
        memExt[2].flags = (unsigned int)(6) | XCL_MEM_TOPOLOGY;
        memExt[3].flags = (unsigned int)(8) | XCL_MEM_TOPOLOGY;
        memExt[4].flags = (unsigned int)(12) | XCL_MEM_TOPOLOGY;
        memExt[5].flags = (unsigned int)(22) | XCL_MEM_TOPOLOGY;
        memExt[6].flags = (unsigned int)(24) | XCL_MEM_TOPOLOGY;
        memExt[7].flags = (unsigned int)(28) | XCL_MEM_TOPOLOGY;
        memExt[8].flags = (unsigned int)(1) | XCL_MEM_TOPOLOGY;
        memExt[9].flags = (unsigned int)(3) | XCL_MEM_TOPOLOGY;
        memExt[10].flags = (unsigned int)(7) | XCL_MEM_TOPOLOGY;
        memExt[11].flags = (unsigned int)(9) | XCL_MEM_TOPOLOGY;
        memExt[12].flags = (unsigned int)(13) | XCL_MEM_TOPOLOGY;
        memExt[13].flags = (unsigned int)(23) | XCL_MEM_TOPOLOGY;
        memExt[14].flags = (unsigned int)(25) | XCL_MEM_TOPOLOGY;
        memExt[15].flags = (unsigned int)(29) | XCL_MEM_TOPOLOGY;
    } 
    else if (overlayVersion == 1) { // Overlay V2022
    #ifdef USE_U50
        // This is for U50
        memExt[0].flags = (unsigned int)(4) | XCL_MEM_TOPOLOGY;
        memExt[1].flags = (unsigned int)(6) | XCL_MEM_TOPOLOGY;
        memExt[2].flags = (unsigned int)(8) | XCL_MEM_TOPOLOGY;
        memExt[3].flags = (unsigned int)(10) | XCL_MEM_TOPOLOGY;
        memExt[4].flags = (unsigned int)(20) | XCL_MEM_TOPOLOGY;
        memExt[5].flags = (unsigned int)(22) | XCL_MEM_TOPOLOGY;
        memExt[6].flags = (unsigned int)(24) | XCL_MEM_TOPOLOGY;
        memExt[7].flags = (unsigned int)(26) | XCL_MEM_TOPOLOGY;
        memExt[8].flags = (unsigned int)(5) | XCL_MEM_TOPOLOGY;
        memExt[9].flags = (unsigned int)(7) | XCL_MEM_TOPOLOGY;
        memExt[10].flags = (unsigned int)(9) | XCL_MEM_TOPOLOGY;
        memExt[11].flags = (unsigned int)(11) | XCL_MEM_TOPOLOGY;
        memExt[12].flags = (unsigned int)(21) | XCL_MEM_TOPOLOGY;
        memExt[13].flags = (unsigned int)(23) | XCL_MEM_TOPOLOGY;
        memExt[14].flags = (unsigned int)(25) | XCL_MEM_TOPOLOGY;
        memExt[15].flags = (unsigned int)(27) | XCL_MEM_TOPOLOGY;
    #else
        // This is for U280
	    memExt[0].flags = (unsigned int)(0) | XCL_MEM_TOPOLOGY;
        memExt[1].flags = (unsigned int)(2) | XCL_MEM_TOPOLOGY;
        memExt[2].flags = (unsigned int)(6) | XCL_MEM_TOPOLOGY;
        memExt[3].flags = (unsigned int)(8) | XCL_MEM_TOPOLOGY;
        memExt[4].flags = (unsigned int)(12) | XCL_MEM_TOPOLOGY;
        memExt[5].flags = (unsigned int)(22) | XCL_MEM_TOPOLOGY;
        memExt[6].flags = (unsigned int)(24) | XCL_MEM_TOPOLOGY;
        memExt[7].flags = (unsigned int)(28) | XCL_MEM_TOPOLOGY;
        memExt[8].flags = (unsigned int)(1) | XCL_MEM_TOPOLOGY;
        memExt[9].flags = (unsigned int)(3) | XCL_MEM_TOPOLOGY;
        memExt[10].flags = (unsigned int)(7) | XCL_MEM_TOPOLOGY;
        memExt[11].flags = (unsigned int)(9) | XCL_MEM_TOPOLOGY;
        memExt[12].flags = (unsigned int)(13) | XCL_MEM_TOPOLOGY;
        memExt[13].flags = (unsigned int)(23) | XCL_MEM_TOPOLOGY;
        memExt[14].flags = (unsigned int)(25) | XCL_MEM_TOPOLOGY;
        memExt[15].flags = (unsigned int)(29) | XCL_MEM_TOPOLOGY;
    #endif
    }
#endif

        // for (int i = 0; i < 16; i++) {
        //     memExt[i].param = 0;
        //     memExt[i].obj = tb_buf[i];
        // };

        // Map buffers
        if(overlayVersion == 0){

            for (int i = 0; i < 16; i++) {
                memExt[i].param = 0;
                memExt[i].obj = tb_buf[i];
            };

            for (int i = 0; i < 8; i++) {
                buffer[i] = cl::Buffer(context, CL_MEM_READ_WRITE | CL_MEM_EXT_PTR_XILINX | CL_MEM_USE_HOST_PTR,
                                   (size_t)(8 * HT_BUFF_DEPTH), &memExt[i]);
            };
            for (int i = 8; i < 16; i++) {
                buffer[i] = cl::Buffer(context, CL_MEM_READ_WRITE | CL_MEM_EXT_PTR_XILINX | CL_MEM_USE_HOST_PTR,
                                    (size_t)(8 * S_BUFF_DEPTH), &memExt[i]);
            };
        }
        else if(overlayVersion == 1){

            for (int i = 0; i < 16; i++) {
                memExt[i].param = 0;
                memExt[i].obj = tb_buf_overlay2022[i];
            }; 

            for (int i = 0; i < 8; i++) {
                buffer[i] = cl::Buffer(context, CL_MEM_READ_WRITE | CL_MEM_EXT_PTR_XILINX | CL_MEM_USE_HOST_PTR,
                                   (size_t)(32 * HT_BUFF_DEPTH), &memExt[i]);
            };
            for (int i = 8; i < 16; i++) {
                buffer[i] = cl::Buffer(context, CL_MEM_READ_WRITE | CL_MEM_EXT_PTR_XILINX | CL_MEM_USE_HOST_PTR,
                                    (size_t)(32 * S_BUFF_DEPTH), &memExt[i]);
            };
        }
        
    };

    void initBuffer(cl::CommandQueue clq) {
        std::vector<cl::Memory> tb;
        for (int i = 0; i < 16; i++) {
            tb.push_back(buffer[i]);
        }
        clq.enqueueMigrateMemObjects(tb, CL_MIGRATE_MEM_OBJECT_CONTENT_UNDEFINED, nullptr, nullptr);
        clq.finish();
    }

}; // end of class

class AggrBufferTmp {
   public:
    cl_mem_ext_ptr_t memExt[8];
    ap_uint<512>* tb_buf[8];
    cl::Buffer buffer[8];
    int overlayVersion = 0;

    AggrBufferTmp(cl::Context& context, int overlayVersion_) {
        overlayVersion = overlayVersion_;

        for (int i = 0; i < 8; i++) {
            tb_buf[i] = aligned_alloc<ap_uint<512> >(S_BUFF_DEPTH / 8);
        };

#ifdef USE_DDR
        memExt[0].flags = XCL_MEM_DDR_BANK1;
        memExt[1].flags = XCL_MEM_DDR_BANK1;
        memExt[2].flags = XCL_MEM_DDR_BANK1;
        memExt[3].flags = XCL_MEM_DDR_BANK2;
        memExt[4].flags = XCL_MEM_DDR_BANK2;
        memExt[5].flags = XCL_MEM_DDR_BANK2;
        memExt[6].flags = XCL_MEM_DDR_BANK3;
        memExt[7].flags = XCL_MEM_DDR_BANK3;
#else
        if (overlayVersion == 0) {
            // memExt[0].flags = (unsigned int)(8) | XCL_MEM_TOPOLOGY;
            // memExt[1].flags = (unsigned int)(12) | XCL_MEM_TOPOLOGY;
            // memExt[2].flags = (unsigned int)(16) | XCL_MEM_TOPOLOGY;
            // memExt[3].flags = (unsigned int)(20) | XCL_MEM_TOPOLOGY;
            // memExt[4].flags = (unsigned int)(10) | XCL_MEM_TOPOLOGY;
            // memExt[5].flags = (unsigned int)(14) | XCL_MEM_TOPOLOGY;
            // memExt[6].flags = (unsigned int)(18) | XCL_MEM_TOPOLOGY;
            // memExt[7].flags = (unsigned int)(22) | XCL_MEM_TOPOLOGY;
            memExt[0].flags = (unsigned int)(0) | XCL_MEM_TOPOLOGY;
            memExt[1].flags = (unsigned int)(4) | XCL_MEM_TOPOLOGY;
            memExt[2].flags = (unsigned int)(8) | XCL_MEM_TOPOLOGY;
            memExt[3].flags = (unsigned int)(12) | XCL_MEM_TOPOLOGY;
            memExt[4].flags = (unsigned int)(2) | XCL_MEM_TOPOLOGY;
            memExt[5].flags = (unsigned int)(6) | XCL_MEM_TOPOLOGY;
            memExt[6].flags = (unsigned int)(10) | XCL_MEM_TOPOLOGY;
            memExt[7].flags = (unsigned int)(14) | XCL_MEM_TOPOLOGY;
        } else if (overlayVersion == 1) { // Overlay V2022
            //DONE: fill this
            memExt[0].flags = (unsigned int)(0) | XCL_MEM_TOPOLOGY;
            memExt[1].flags = (unsigned int)(4) | XCL_MEM_TOPOLOGY;
            memExt[2].flags = (unsigned int)(8) | XCL_MEM_TOPOLOGY;
            memExt[3].flags = (unsigned int)(12) | XCL_MEM_TOPOLOGY;
            memExt[4].flags = (unsigned int)(2) | XCL_MEM_TOPOLOGY;
            memExt[5].flags = (unsigned int)(6) | XCL_MEM_TOPOLOGY;
            memExt[6].flags = (unsigned int)(10) | XCL_MEM_TOPOLOGY;
            memExt[7].flags = (unsigned int)(14) | XCL_MEM_TOPOLOGY;
        }
#endif

        for (int i = 0; i < 8; i++) {
            memExt[i].param = 0;
            memExt[i].obj = tb_buf[i];
        };

        // Map buffers
        for (int i = 0; i < 8; i++) {
            buffer[i] = cl::Buffer(context, CL_MEM_READ_WRITE | CL_MEM_EXT_PTR_XILINX | CL_MEM_USE_HOST_PTR,
                                   (size_t)(8 * S_BUFF_DEPTH), &memExt[i]);
            // std::cout << "S-BUFF DEPTH: " << S_BUFF_DEPTH << std::endl;
        };
    };

    void BufferInitial(cl::CommandQueue& q) {
        std::vector<cl::Memory> tb;
        for (int i = 0; i < 8; i++) {
            tb.push_back(buffer[i]);
        }
        q.enqueueMigrateMemObjects(tb, CL_MIGRATE_MEM_OBJECT_CONTENT_UNDEFINED, nullptr, nullptr);
    };

}; // end of class

class krnlEngine {
    Table* in1;
    Table* in2;
    Table* out;
    cfgCmd* cfgcmd;
    bufferTmp* buftmp;
    bfbufferTmp* bfbuftmp;

    cl::CommandQueue clq;
    cl::Context context;

    int build_or_probe_flag = 0; // 0 - build; 1 - probe

   public:
    cl::Kernel krnl;
    krnlEngine(){};

    krnlEngine(cl::Program& program, cl::CommandQueue& q, cl::Context& context_, const std::string krnname, int flag = 0) {
        krnl = cl::Kernel(program, krnname.c_str());
        clq = q;
        context = context_;
        build_or_probe_flag = flag;
    };

    krnlEngine(cl::Program& program, cl::CommandQueue& q, const std::string krnname) {
        krnl = cl::Kernel(program, krnname.c_str());
        clq = q;
    };

    void setup(Table& tb1, Table& tb2, Table& tbout, cfgCmd& cmd, bufferTmp& buf) {
        in1 = &tb1;
        in2 = &tb2;
        out = &tbout;
        cfgcmd = &cmd;
        buftmp = &buf;

        // Overlay 2022
        // TODO: abstract build and probe into a single setup() and run()
        // TODO: 1. allocate device buffer vector size (3 input columns for tb1 and tb2, 4 output columns for tbout)
        int j = 0;
        if(in1->overlayVersion == 0) {

            //Jahanvi - allocate device buffers
            in1->allocateDevBuffer(context, 32);
            in2->allocateDevBuffer(context, 32);
            out->allocateDevBuffer(context, 32);

            cfgcmd->allocateDevBuffer(context, 32);

            j = 0;
            krnl.setArg(j++, (in1->buffer));
            krnl.setArg(j++, (in2->buffer));
            krnl.setArg(j++, (out->buffer));
            krnl.setArg(j++, (cfgcmd->buffer));
            for (int r = 0; r < 16; r++) {
                krnl.setArg(j++, (buftmp->buffer)[r]);
            };
        }
        else if(in1->overlayVersion == 1) {
            // (cfgcmd->input_key_columns).resize(3);
            // cfgcmd->input_key_columns[0] = 0;
            // cfgcmd->input_key_columns[1] = -1;
            // cfgcmd->input_key_columns[2] = -1;
            
            // (cfgcmd->output_key_columns).resize(4);
            // cfgcmd->output_key_columns[0] = 0;
            // cfgcmd->output_key_columns[1] = 1;
            // cfgcmd->output_key_columns[2] = 2;
            // cfgcmd->output_key_columns[3] = 3;    
#ifdef USE_U50
            in1->allocateDevBuffer_col(context, 0, (cfgcmd->input_key_columns)[0]);
            in1->allocateDevBuffer_col(context, 1, (cfgcmd->input_key_columns)[1]);
            in1->allocateDevBuffer_col(context, 2, (cfgcmd->input_key_columns)[2]);
            // std::cout << "Input device buffers allocated" << std::endl;

            // std::cout << "Output Num of rows: " << out->nrow << std::endl;
            if(build_or_probe_flag == 0){
                out->allocateDevBuffer_col(context, 14, (cfgcmd->output_key_columns)[0]);
                out->allocateDevBuffer_col(context, 15, (cfgcmd->output_key_columns)[1]);
                out->allocateDevBuffer_col(context, 16, (cfgcmd->output_key_columns)[2]);
                out->allocateDevBuffer_col(context, 17, (cfgcmd->output_key_columns)[3]);
            }
            // std::cout << "Output device buffers allocated" << std::endl;
 
            //ADD: config , meta, valid_en buffers
            cfgcmd->allocateDevBuffer(context, 4); //cfg + meta + valid_en device buffers allocated

            //ADD: Kernel arguments
            j = 0;
            krnl.setArg(j++, (in1->buffer_col)[0]);
            krnl.setArg(j++, (in1->buffer_col)[1]);
            krnl.setArg(j++, (in1->buffer_col)[2]);
            krnl.setArg(j++, (cfgcmd->buffer_valid_in_col));
            krnl.setArg(j++, (cfgcmd->buffer));
            krnl.setArg(j++, (cfgcmd->buffer_meta_in));
            krnl.setArg(j++, (cfgcmd->buffer_meta_out));
            krnl.setArg(j++, (out->buffer_col)[0]);
            krnl.setArg(j++, (out->buffer_col)[1]);
            krnl.setArg(j++, (out->buffer_col)[2]);
            krnl.setArg(j++, (out->buffer_col)[3]);
            for (int r = 0; r < 16; r++) {
                krnl.setArg(j++, (buftmp->buffer)[r]);
            };
            // std::cout << "Kernel Arguments are set" <<std::endl;
#else
// std::cout << "1 \n";
            in1->allocateDevBuffer_col(context, 32, (cfgcmd->input_key_columns)[0]);
            in1->allocateDevBuffer_col(context, 32, (cfgcmd->input_key_columns)[1]);
            in1->allocateDevBuffer_col(context, 32, (cfgcmd->input_key_columns)[2]);
            // std::cout << "Input device buffers allocated" << std::endl;

            // std::cout << "Output Num of rows: " << out->nrow << std::endl;
            if(build_or_probe_flag == 0){
// std::cout << "BUILD: output table nrow (START): " << out->getNumRow() << " ";
                out->allocateDevBuffer_col(context, 32, (cfgcmd->output_key_columns)[0]);
                out->allocateDevBuffer_col(context, 32, (cfgcmd->output_key_columns)[1]);
                out->allocateDevBuffer_col(context, 32, (cfgcmd->output_key_columns)[2]);
                out->allocateDevBuffer_col(context, 32, (cfgcmd->output_key_columns)[3]);
// std::cout << "BUILD: output table nrow (END): " << out->getNumRow() << " ";
            }
            // std::cout << "Output device buffers allocated" << std::endl;
 
// std::cout << "1 \n";
            //ADD: config , meta, valid_en buffers
            cfgcmd->allocateDevBuffer(context, 0); //cfg + meta + valid_en device buffers allocated
// std::cout << "1 \n";
            // //ADD: Kernel arguments
            j = 0;
            krnl.setArg(j++, (in1->buffer_col)[0]);
            krnl.setArg(j++, (in1->buffer_col)[1]);
            krnl.setArg(j++, (in1->buffer_col)[2]);
            krnl.setArg(j++, (cfgcmd->buffer_valid_in_col));
            krnl.setArg(j++, (cfgcmd->buffer));
            krnl.setArg(j++, (cfgcmd->buffer_meta_in));
            krnl.setArg(j++, (cfgcmd->buffer_meta_out));
            krnl.setArg(j++, (out->buffer_col)[0]);
            krnl.setArg(j++, (out->buffer_col)[1]);
            krnl.setArg(j++, (out->buffer_col)[2]);
            krnl.setArg(j++, (out->buffer_col)[3]);
            for (int r = 0; r < 16; r++) {
                krnl.setArg(j++, (buftmp->buffer)[r]);
            };
            // std::cout << "Kernel Arguments are set" <<std::endl;
#endif
        }
    };

    void setup(Table& tb1, Table& tb2, Table& tbout, cfgCmd& cmd, bfbufferTmp& buf) {
        in1 = &tb1;
        in2 = &tb2;
        out = &tbout;
        cfgcmd = &cmd;
        bfbuftmp = &buf;

        // Overlay 2022
        // TODO: abstract build and probe into a single setup() and run()
        // TODO: 1. allocate device buffer vector size (3 input columns for tb1 and tb2, 4 output columns for tbout)
        if(in1->overlayVersion == 1){

            (cfgcmd->input_key_columns).resize(3);
            cfgcmd->input_key_columns[0] = 0;
            cfgcmd->input_key_columns[1] = 1;
            cfgcmd->input_key_columns[2] = 2;
            
            (cfgcmd->output_key_columns).resize(4);
            cfgcmd->output_key_columns[0] = 0;
            cfgcmd->output_key_columns[1] = 1;
            cfgcmd->output_key_columns[2] = 2;
            cfgcmd->output_key_columns[3] = 3;    

            in1->allocateDevBuffer_col(context, 0, (cfgcmd->input_key_columns)[0]);
            in1->allocateDevBuffer_col(context, 1, (cfgcmd->input_key_columns)[1]);
            in1->allocateDevBuffer_col(context, 2, (cfgcmd->input_key_columns)[2]);

            std::cout << "Input device buffers allocated" << std::endl;

            std::cout << "Output Num of rows: " << out->nrow << std::endl;
            out->allocateDevBuffer_col(context, 14, (cfgcmd->output_key_columns)[0]);
            out->allocateDevBuffer_col(context, 15, (cfgcmd->output_key_columns)[1]);
            out->allocateDevBuffer_col(context, 16, (cfgcmd->output_key_columns)[2]);
            out->allocateDevBuffer_col(context, 17, (cfgcmd->output_key_columns)[3]);
            
            std::cout << "Output device buffers allocated" << std::endl;
 
            //ADD: config , meta, valid_en buffers
            cfgcmd->allocateDevBuffer(context, 4); //cfg + meta + valid_en device buffers allocated

            //ADD: Kernel arguments
            int j = 0;
            krnl.setArg(j++, (in1->buffer_col)[0]);
            krnl.setArg(j++, (in1->buffer_col)[1]);
            krnl.setArg(j++, (in1->buffer_col)[2]);
            // krnl.setArg(j++, (in2->buffer));
            krnl.setArg(j++, (cfgcmd->buffer_valid_in_col));
            krnl.setArg(j++, (cfgcmd->buffer));
            krnl.setArg(j++, (cfgcmd->buffer_meta_in));
            krnl.setArg(j++, (cfgcmd->buffer_meta_out));
            krnl.setArg(j++, (out->buffer_col)[0]);
            krnl.setArg(j++, (out->buffer_col)[1]);
            krnl.setArg(j++, (out->buffer_col)[2]);
            krnl.setArg(j++, (out->buffer_col)[3]);
            for (int r = 0; r < 16; r++) {
                krnl.setArg(j++, (bfbuftmp->buffer)[r]);
            };
            std::cout << "Kernel Arguments are set" <<std::endl;
        }
    };

    void setup_buf(Table& tb1, cl::Buffer& tb2, cl::Buffer& tbout, cfgCmd& cmd, bufferTmp& buf) {
        in1 = &tb1;
        cfgcmd = &cmd;
        buftmp = &buf;

        int j = 0;
        krnl.setArg(j++, in1->buffer);
        krnl.setArg(j++, tb2);
        krnl.setArg(j++, tbout);
        krnl.setArg(j++, (cfgcmd->buffer));
        for (int r = 0; r < 16; r++) {
            krnl.setArg(j++, (buftmp->buffer)[r]);
        };
    };

    void setup_hp(const int depth, const int index, const int thres, Table& tbin, Table& tbout, cfgCmd& cmd) {
        in1 = &tbin;
        out = &tbout;
        cfgcmd = &cmd;

        int j = 0;
        krnl.setArg(j++, depth);
        krnl.setArg(j++, index);
        krnl.setArg(j++, thres);
        krnl.setArg(j++, (in1->buffer));
        krnl.setArg(j++, (out->buffer));
        krnl.setArg(j++, (cfgcmd->buffer));
    };

    void updateMeta(int nrow, int ncol) {
        (cfgcmd->meta_in)->setColNum(ncol);
        for(int i = 0; i < ncol; i++){
            (cfgcmd->meta_in)->setCol(i, i, nrow);
        }
        cfgcmd->allocateDevBuffer(context, 0);
        krnl.setArg(3, (cfgcmd->buffer_valid_in_col));
        krnl.setArg(4, (cfgcmd->buffer));
        krnl.setArg(5, (cfgcmd->buffer_meta_in));
        krnl.setArg(6, (cfgcmd->buffer_meta_out));
    };    

    void run(int rc, std::vector<cl::Event>* waitevt, cl::Event* outevt) { clq.enqueueTask(krnl, waitevt, outevt); };
};

class AggrKrnlEngine {
    Table* in;
    Table* out;

    cfgCmd* hpcmd;
    AggrCfgCmd *cfgcmd0, *cfgcmd1;
    AggrBufferTmp* buftmp;

    cl::CommandQueue clq;
    cl::Kernel krnl;
    cl::Context context;

   public:
    AggrKrnlEngine(){};


    AggrKrnlEngine(cl::Program& program, cl::CommandQueue& q, const std::string krnname) {
        krnl = cl::Kernel(program, krnname.c_str());
        clq = q;
    };

    AggrKrnlEngine(cl::Program& program, cl::CommandQueue& q, cl::Context& context_ , const std::string krnname) {
        krnl = cl::Kernel(program, krnname.c_str());
        clq = q;
        context = context_;
    };

    void setup(Table& tb, Table& tbout, AggrCfgCmd& cfg_in, AggrCfgCmd& cfg_out, AggrBufferTmp& buf) {
        in = &tb;
        out = &tbout;
        cfgcmd0 = &cfg_in;
        cfgcmd1 = &cfg_out;
        buftmp = &buf;

        int j = 0;
        if (in->overlayVersion == 0) {
            // DONE: move allocate device buffer calls for input, output, and config here (remove from host.cpp)
            // in->allocateDevBuffer(context, 32, 8);
            // out->allocateDevBuffer(context, 33, 16);
            in->allocateDevBuffer(context, 0, 8);
            out->allocateDevBuffer(context, 1, 16);

            cfgcmd0->setup(0, 0); // setup overlay version, input 
            cfgcmd1->setup(0, 1); // setup overlay version, output
            // cfgcmd0->allocateDevBuffer(context, 32);
            // cfgcmd1->allocateDevBuffer(context, 33);
            cfgcmd0->allocateDevBuffer(context, 0);
            cfgcmd1->allocateDevBuffer(context, 1);

            krnl.setArg(j++, (in->buffer));
            krnl.setArg(j++, (out->buffer));
            krnl.setArg(j++, (cfgcmd0->buffer));
            krnl.setArg(j++, (cfgcmd1->buffer));
            for (int r = 0; r < 8; r++) {
                krnl.setArg(j++, (buftmp->buffer)[r]);
            };
        } 
        else if (in->overlayVersion == 1) {
            in->allocateDevBuffer(context, 0, 8);
            out->allocateDevBuffer(context, 1, 16);

            // cfgcmd0->setup(1, 0); // setup overlay version, input 
            // cfgcmd1->setup(1, 1); // setup overlay version, output
            cfgcmd0->setup(0, 0); // setup overlay version, input 
            cfgcmd1->setup(0, 1); // setup overlay version, output            

            cfgcmd0->allocateDevBuffer(context, 0); // allocates for both cfg + meta
            cfgcmd1->allocateDevBuffer(context, 1); // allocates for both cfg + meta

            krnl.setArg(j++, (in->buffer));
            krnl.setArg(j++, (out->buffer));
            krnl.setArg(j++, (cfgcmd0->buffer));
            krnl.setArg(j++, (cfgcmd1->buffer));
            for (int r = 0; r < 8; r++) {
                krnl.setArg(j++, (buftmp->buffer)[r]);
            }
            // j = 0;
            // for(int k = 0; k < 8; k++) {
            //     krnl.setArg(j++, (in->buffer_col)[k]);
            // }
            // // std::cout << "input_buffer\n";
            // krnl.setArg(j++, (cfgcmd0->buffer_meta));
            // krnl.setArg(j++, (cfgcmd1->buffer_meta));
            // // std::cout << "meta_buffer\n";
            // for(int k = 0; k < 16; k++) {
            //     krnl.setArg(j++, (out->buffer_col)[k]);
            // }
            // // std::cout << "output_buffer\n";
            // krnl.setArg(j++, (cfgcmd0->buffer));
            // krnl.setArg(j++, (cfgcmd1->buffer));
            // // std::cout << "config_buffer\n";
            // for (int k = 0; k < 8; k++) {
            //     krnl.setArg(j++, (buftmp->buffer)[k]);
            // }
        }

    };

    void setup_hp(const int depth, const int index, const int thres, Table& tbin, Table& tbout, cfgCmd& cmd) {
        in = &tbin;
        out = &tbout;
        hpcmd = &cmd;

        int j = 0;
        krnl.setArg(j++, depth);
        krnl.setArg(j++, index);
        krnl.setArg(j++, thres);
        krnl.setArg(j++, (in->buffer));
        krnl.setArg(j++, (out->buffer));
        krnl.setArg(j++, (hpcmd->buffer));
    };

    void updateMeta(int nrow, int ncol)
    {
        (cfgcmd0->meta_aggr)->setColNum(ncol);
        for(int i = 0; i < ncol; i++){
            (cfgcmd0->meta_aggr)->setCol(i, i, nrow);
        }
        (cfgcmd0)->allocateDevBuffer(context, 0);
        krnl.setArg(8, (cfgcmd0->buffer_meta));
        krnl.setArg(26, (cfgcmd0->buffer));

    };

    void run(int rc, std::vector<cl::Event>* waitevt, cl::Event* outevt) { clq.enqueueTask(krnl, waitevt, outevt); };
};

class transEngine {
    std::vector<cl::Memory> ib[2];
    cl::CommandQueue clq;
    int overlayVersion = 0; // 0 - 2020; 1 - 2022

   public:
    transEngine(){};
    transEngine(cl::CommandQueue q) { clq = q; };

    void setq(cl::CommandQueue q, int overlayVersion_) { 
        clq = q; 
        overlayVersion = overlayVersion_;
    };
    // void add(Table* tb) {
    //     ib[0].push_back((tb->buffer));
    //     // ib[1].push_back((tb->buffer)[1]);
    // };
    void add(Table* tb) {
        // DONE: backward compatible with 2020
        if(overlayVersion == 0){
            ib[0].push_back((tb->buffer));
        }
        else if(overlayVersion == 1){
            int ncol = (tb->data_col).size();
            int ncol1 = (tb->buffer_col).size();
            // std::cout << "Data columns number: " << ncol << " Buffer columns number: " << ncol1 << std::endl;
            for(int i = 0; i < ncol1; i++){
                ib[0].push_back((tb->buffer_col)[i]);
            }
        }
    };
    // void add(Table* tb) {
    //     // DONE: backward compatible with 2020
    //     if(overlayVersion == 0){
    //         ib[0].push_back((tb->buffer));
    //     }
    //     else if(overlayVersion == 1){
    //         int ncol = (tb->data_col).size();
    //         std::cout << "alec-added tbl col size(): " << ncol << std::endl;
    //         for(int i = 0; i < ncol; i++){
    //             ib[0].push_back((tb->buffer_col)[i]);
    //         }
    //     }
    // };
    void clear_add(Table* tb) {
        ib[0].clear();
        ib[0].push_back((tb->buffer));
        // ib[1].push_back((tb->buffer)[1]);
    };
    void add(cl::Buffer& buf) {
        ib[0].push_back(buf);
        // ib[1].push_back((tb->buffer)[1]);
    };
    void add(cfgCmd* tb) {
        ib[0].push_back((tb->buffer));
        if(overlayVersion == 1){
            ib[0].push_back((tb->buffer_meta_in));
            ib[0].push_back((tb->buffer_meta_out));
        }
        // ib[1].push_back((tb->buffer)[1]);
    };
    // void add(AggrCfgCmd* tb) {
    //     ib[0].push_back((tb->buffer));
    //     // ib[1].push_back((tb->buffer)[1]);
    // };
    void add(AggrCfgCmd* tb) {
        // DONE: backward compatible with 2020
        ib[0].push_back((tb->buffer));
        if(overlayVersion == 1){
            ib[0].push_back((tb->buffer_meta));
        }
    };
    void clear() { ib[0].clear(); }
    void host2dev(int rc, std::vector<cl::Event>* waitevt, cl::Event* outevt) {
        clq.enqueueMigrateMemObjects(ib[rc], 0, waitevt, outevt);
    };

    void dev2host(int rc, std::vector<cl::Event>* waitevt, cl::Event* outevt) {
        clq.enqueueMigrateMemObjects(ib[rc], CL_MIGRATE_MEM_OBJECT_HOST, waitevt, outevt);
    };
};

#endif

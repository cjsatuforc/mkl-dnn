/*******************************************************************************
* Copyright 2017 Intel Corporation
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
*******************************************************************************/

#ifndef _DNN_TYPES_HPP
#define _DNN_TYPES_HPP

#include <stdlib.h>
#include <stddef.h>
#include <string.h>

#include "common.hpp"
#include "mkldnn_types.h"

enum dir_t {
    DIR_UNDEF = 0,
    FLAG_DAT = 1, FLAG_WEI = 2, FLAG_BIA = 4,
    FLAG_FWD = 32, FLAG_BWD = 64,
    FWD_D = FLAG_FWD + FLAG_DAT,
    FWD_B = FLAG_FWD + FLAG_DAT + FLAG_BIA,
    BWD_D = FLAG_BWD + FLAG_DAT,
    BWD_W = FLAG_BWD + FLAG_WEI,
    BWD_WB = FLAG_BWD + FLAG_WEI + FLAG_BIA,
};
dir_t str2dir(const char *str);
const char *dir2str(dir_t dir);

struct attr_t {
    enum round_mode_t {
        NEAREST = (int)mkldnn_round_nearest,
        DOWN = (int)mkldnn_round_down,
    };

    struct scale_t {
        enum policy_t { NONE = 0, COMMON, PER_OC, POLICY_TOTAL };
        static policy_t str2policy(const char *str);
        static const char *policy2str(policy_t policy);

        int str2scale(const char *str, const char **end_s);
        void scale2str(char *buffer, char **end_b) const;

        bool is_def() const { return this->policy == NONE; }

        policy_t policy = NONE;
        float scale = 1.;
    };

    struct post_ops_t {
        enum kind_t { SUM, RELU, KIND_TOTAL };
        static kind_t str2kind(const char *str);
        static const char *kind2str(kind_t kind);

        struct entry_t {
            kind_t kind;
            union {
                struct { float scale; } sum;
                struct {
                    // eltwise algorithm in future
                    float scale, alpha, beta; // unused now
                } eltwise;
            };
        };

        post_ops_t(): len(0) {}

        int from_str(const char *str, const char **end_s);
        void to_str(char *buffer, char **end_b) const;

        bool is_def() const { return len == 0; }

        enum { capacity = 4 };
        int len;
        entry_t entry[4];
    };

    round_mode_t irmode = NEAREST;
    scale_t oscale;
    post_ops_t post_ops;

    bool is_def() const;
};

const size_t max_attr_len = 128;
int str2attr(attr_t *attr, const char *str);
void attr2str(const attr_t *attr, char *buffer);

mkldnn_primitive_attr_t create_mkldnn_attr(const attr_t &attr, int scale_cnt,
        const float *scales);

#endif
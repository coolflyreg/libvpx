/*
 *  Copyright (c) 2014 The WebM project authors. All Rights Reserved.
 *
 *  Use of this source code is governed by a BSD-style license
 *  that can be found in the LICENSE file in the root of the source
 *  tree. An additional intellectual property rights grant can be found
 *  in the file PATENTS.  All contributing project authors may
 *  be found in the AUTHORS file in the root of the source tree.
 */

#ifndef AV1_ENCODER_COST_H_
#define AV1_ENCODER_COST_H_

#include "aom_dsp/prob.h"
#include "aom/aom_integer.h"
#if CONFIG_ANS
#include "aom_dsp/ans.h"
#endif  // CONFIG_ANS

#ifdef __cplusplus
extern "C" {
#endif

extern const uint16_t av1_prob_cost[256];

// The factor to scale from cost in bits to cost in av1_prob_cost units.
#define AV1_PROB_COST_SHIFT 9

#define av1_cost_zero(prob) (av1_prob_cost[prob])

#define av1_cost_one(prob) av1_cost_zero(256 - (prob))

#define av1_cost_bit(prob, bit) av1_cost_zero((bit) ? 256 - (prob) : (prob))

// Cost of coding an n bit literal, using 128 (i.e. 50%) probability
// for each bit.
#define av1_cost_literal(n) ((n) * (1 << AV1_PROB_COST_SHIFT))

static INLINE unsigned int cost_branch256(const unsigned int ct[2],
                                          aom_prob p) {
  return ct[0] * av1_cost_zero(p) + ct[1] * av1_cost_one(p);
}

static INLINE int treed_cost(aom_tree tree, const aom_prob *probs, int bits,
                             int len) {
  int cost = 0;
  aom_tree_index i = 0;

  do {
    const int bit = (bits >> --len) & 1;
    cost += av1_cost_bit(probs[i >> 1], bit);
    i = tree[i + bit];
  } while (len);

  return cost;
}

void av1_cost_tokens(int *costs, const aom_prob *probs, aom_tree tree);
void av1_cost_tokens_skip(int *costs, const aom_prob *probs, aom_tree tree);

#if CONFIG_ANS
void av1_cost_tokens_ans(int *costs, const aom_prob *tree_probs,
                         const rans_lut token_cdf, int skip_eob);
#endif

#ifdef __cplusplus
}  // extern "C"
#endif

#endif  // AV1_ENCODER_COST_H_